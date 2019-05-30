#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <limits.h> // realpath(3)
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <pwd.h> // getpwuid(3)
#include <errno.h>

#ifndef R_OK
#define R_OK 4
#endif
#ifndef F_OK
#define F_OK 0
#endif

extern char *binary_directory;
extern char (backup_directory)[512];
extern FILE *log_file;

void *file_backup_thr(void *);
struct backups_head;
struct backup_file_node;
struct backedup_file_queue;

struct backups_head backup_list; // 백업 리스트 전역 변수 선언

// 백업 리스트의 Linked List Header
struct backups_head {
    int number_of_nodes;
    struct backup_file_node *start;
};

// 백업 리스트의 Linked List Node
struct backup_file_node {
    struct backup_file_node *next;
    char pathname[512];
    int period;
    int option_m; // 입력받은 PERIOD마다 파일의 mtime이 수정 되었을 경우 백업 실행
    int option_n; // -n NUMBER : NUMBER는 백업한 파일의 최대 개수. 가장 최근 NUMBER개의 백업파일 외 나머지 파일은 삭제
    int maxn;
    int option_t; // -t TIME : 백업해야할 파일에 대한 백업 디렉토리 내 보관 기간을 TIME만큼 설정
    int store_time; // 보관 기간
    pthread_t backup_thread;
    struct backedup_file_queue *bfq;// 백업 중인 파일 리스트 관리를 위한 큐 포인터 
};

// 백업파일 관리를 위한 순환 queue, 큐 사이즈는 최대 512
struct backedup_file_queue {
    int front, rear;
    int limit; // n옵션의 인자가 된다. (최대 100의 값이 들어온다고 가정)
    int number_of_nodes;
    char *queue[512];
    time_t create_time[512]; // 백업 파일 생성 시간
};

void backup_list_init()
{
    backup_list.number_of_nodes = 0;
    backup_list.start = NULL;
}

void get_absolute_path(char *result, char *path)
{
    char real_path[512];
    // ~로 시작하는 경로이면 홈 디렉토리로 처리한다.
    if (path[0] == '~') {
        char home_path[512];
        char temp_path[512];
        if (path[1] != '/' && path[1] != '\0')
            strcpy(home_path, "/home/");
        else
            strcpy(home_path, getpwuid(getuid())->pw_dir);
        ++path;
         // /home/username + path
         strcpy(temp_path, home_path);
         strcat(temp_path, path);
         realpath(temp_path, real_path);
    } else {
        realpath(path, real_path);
    }
    strncpy(result, real_path, 511);
    result[511] = '\0';
}

// 경로에서 파일 이름만 가져옴
char* get_file_name(char *pathname)
{
    char *fn_ptr;
    fn_ptr = strrchr(pathname, '/');
    if (fn_ptr)
        return fn_ptr + 1;
    else
        return pathname;
}

void backedup_file_init(struct backedup_file_queue *bfq, int n_option_arg)
{
    bfq->number_of_nodes = 0;
    bfq->front = bfq->rear = 0;
    if (n_option_arg < 0)
        bfq->limit = 512;
    else
        bfq->limit = n_option_arg + 2;
}

void backedup_file_add(struct backedup_file_queue *bfq, const char *data)
{
    if ((bfq->rear+1) % bfq->limit == bfq->front) {
        fprintf(stderr, "backedup_file fully added!!\n");
        return;
    }
    ++bfq->number_of_nodes;
    ++bfq->rear;
    bfq->rear %= bfq->limit;
    char *tmp = (char *)calloc(1, strlen(data) + 1); // => 이건 에러 없음.
    bfq->queue[bfq->rear] = tmp;
    time(&bfq->create_time[bfq->rear]); // 백업 생성 시각을 기록해 둔다.
    strncpy(bfq->queue[bfq->rear], data, strlen(data));
    bfq->queue[bfq->rear][strlen(data)] = '\0';
}

// 처음에 추가된 data 가 빠져 나간다. (제일 오래된 백업)
void backedup_file_del(struct backedup_file_queue *bfq) {
    if (bfq->front == bfq->rear) {
        fprintf(stderr, "queue is empty\n");
        return;
    }
    --bfq->number_of_nodes;
    ++bfq->front;
    bfq->front %= bfq->limit;
    // printf("free.. [%d] : %s\n", bfq->front, bfq->queue[bfq->front]);
    free(bfq->queue[bfq->front]); // 쓰이지 않을 데이터는 힙에서 해제
    bfq->queue[bfq->front] = NULL;
}

// 모두 삭제를 위한 내부함수
void backup_list_pop()
{
    if (backup_list.number_of_nodes == 0) {
        fprintf(stderr, "이미 백업 리스트 비었음\n");
        return;
    }
    --backup_list.number_of_nodes;
    struct backup_file_node *tmp = backup_list.start;
    backup_list.start = backup_list.start->next;
    free(tmp);
}

void backup_list_delete(char *pathname)
{
    // 백업 중인 작업 중지
    int find = 0;
    struct backup_file_node *prev = NULL;
    struct backup_file_node *curr = backup_list.start;
    struct tm lt;
    struct tm *tm_p;
    time_t now;
    time(&now);
    tm_p = localtime_r(&now, &lt);

    while (curr) {
        if (strcmp(curr->pathname, pathname) == 0) {
            pthread_detach(curr->backup_thread);
            pthread_cancel(curr->backup_thread); // 내부 백업 과정을 중지시킴
            find = 1;
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    if (!find) {
        fprintf(stderr, "%s는 백업되지 않음\n", pathname);
        return;
    }
    // curr에 있는 것 메모리 해제함. 링크를 이어줌.

    // 찾은 노드가 head에 있는 것이라면
    if (curr == backup_list.start) {
        fprintf(log_file, "[%02d%02d%02d %02d%02d%02d] %s deleted\n", (tm_p->tm_year+1900)%100,tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, backup_list.start->pathname);
        backup_list_pop(); //헤드에 있는 것
        return;
    }
    prev->next = curr->next;
    fprintf(log_file, "[%02d%02d%02d %02d%02d%02d] %s deleted\n", (tm_p->tm_year+1900)%100,tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, curr->pathname);
    free(curr);
    --backup_list.number_of_nodes;
}

void backup_list_append(char *pathname, int period, int option_m, int option_n, int maxn, int option_t, int store_time)
{
    // 이미 같은 pathname에 대해 백업이 진행중이라면 에러.
    struct backup_file_node *curr = backup_list.start;
    while (curr) {
        if (strcmp(curr->pathname, pathname) == 0) {
            fprintf(stderr, "이미 같은 파일이 백업 중입니다. -> %s\n", curr->pathname);
            return;
        }
        curr = curr->next;
    }

    // 링크드 리스트 구성하기.
    struct backup_file_node *node = (struct backup_file_node *)calloc(1, sizeof(struct backup_file_node));
    if (backup_list.number_of_nodes != 0)
        node->next = backup_list.start;
    else
        node->next = NULL;
    strncpy(node->pathname, pathname, 511);
    node->pathname[511] = '\0';
    node->period = period;
    node->option_m = option_m;
    node->option_n = option_n;
    node->maxn = maxn;
    node->option_t = option_t;
    node->store_time = store_time;
    backup_list.start = node;
    // thread on
    pthread_create(&node->backup_thread, NULL, file_backup_thr, (void *)node);
    ++backup_list.number_of_nodes;
}

void cleanup(void *args)
{
    if (args == NULL) {
        return;
    }
    struct backedup_file_queue *bfq = (struct backedup_file_queue *)args;
    while(bfq->front != bfq->rear) {
        // printf("%s => 이 파일은 더 이상 백업하지 않으므로 메모리를 해제합니다.\n", bfq->queue[(bfq->front + 1)%bfq->limit]);
        backedup_file_del(bfq);
    }
    free(args);
}

void *file_backup_thr(void *args)
{
    struct backup_file_node *node = (struct backup_file_node *)args;
    struct stat statbuf;
    char backedup_pathname[512];
    time_t now;
    struct tm *tm_p;
    struct tm lt;
    time_t recent_backup_mtime; //최근에 백업한 시간
    int isBackup;
    char sysCmd[1024];
    time(&now);
    tm_p = localtime_r(&now, &lt); // make localtime thread-safe
    isBackup = 1;
    node->bfq = (struct backedup_file_queue *)calloc(1, sizeof(struct backedup_file_queue));
    backedup_file_init(node->bfq, node->maxn);
    // 쓰레드가 끝나기 전에 bfq에 할당된 동적 메모리가 반납되어야 한다.
    pthread_cleanup_push(cleanup, node->bfq);

    if (lstat(node->pathname, &statbuf) < 0) {
        fprintf(stderr, "백업할 파일이 없습니다.\n");
        return NULL;
    }
    recent_backup_mtime = statbuf.st_mtime; // 원본 파일의 수정 시간 기록

    fprintf(log_file, "[%02d%02d%02d %02d%02d%02d] %s added\n", (tm_p->tm_year+1900)%100,tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, node->pathname);
    while (1) {
        sleep(node->period); // period 마다 백업을 진행함
        time(&now);
        tm_p = localtime_r(&now, &lt);
        // 1. m 옵션 확인 후 파일이 수정되지 않았다면 백업 처리를 하지 않음
        if (node->option_m) {
            if (lstat(node->pathname, &statbuf) < 0) {
                fprintf(stderr, "백업할 파일이 없습니다.\n");
                return NULL;
            }
            if (recent_backup_mtime == statbuf.st_mtime) {
                // 수정 시간이 안 바뀌어서 백업을 진행하지 않음
                isBackup = 0;
            } else {
                // 수정 시간이 바뀌어 백업을 진행함. 시간은 갱신함.
                recent_backup_mtime = statbuf.st_mtime;
                isBackup = 1;
            }
        }

        if (isBackup) {
            sprintf(backedup_pathname, "%s/%s_%02d%02d%02d%02d%02d%02d", backup_directory, get_file_name(node->pathname), (tm_p->tm_year+1900)%100, tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec);
            // 백업 파일을 실질적으로 생성함
            sprintf(sysCmd, "cp %s %s -p > /dev/null 2>&1", node->pathname, backedup_pathname); // -p 옵션은 mtime 등을 preserve 한다.
            // sprintf(sysCmd, "cp %s %s", node->pathname, backedup_pathname);
            if (system(sysCmd) != 0) {
                // 백업하려는 파일이 접근불가 한 경우
                //fprintf(stderr, "[warning] %s 파일에 더 이상 접근할 수 없습니다.\n 복구하시려면 recover %s ... 명령을 사용하십시오.\n", node->pathname, node->pathname);
                printf("명령 실패\n");
            } else {
                fprintf(log_file, "[%02d%02d%02d %02d%02d%02d] %s generated\n", (tm_p->tm_year+1900)%100, tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, backedup_pathname);
                backedup_file_add(node->bfq, backedup_pathname); // 백업된 파일에 추가한다.
            }
        }
        // 백업 한계에 다다른 경우 오래된 백업부터 삭제
        if (node->bfq->number_of_nodes >= node->bfq->limit - 1) {
            int front = node->bfq->front + 1;
            front %= node->bfq->limit;
            sprintf(sysCmd, "rm %s", node->bfq->queue[front]);
            if (system(sysCmd) != 0) {
                fprintf(stderr, "백업된 파일 삭제 실패.\n%s\n", node->bfq->queue[front]);
            }
            backedup_file_del(node->bfq);
        }

        // 매 PERIOD마다 백업 파일을 생성하고 기존 모든 백업 파일의 생성시간을 확인. 확인한 파일의 생성시간이 주어진 TIME보다 크면 해당 파일 삭제
        if (node->option_t) {
            // 큐를 선회함
            for (int i = 0; i < node->bfq->number_of_nodes; i++) {
                int idx = node->bfq->front + i + 1;
                idx %= node->bfq->limit;
                if (now - node->bfq->create_time[idx] > node->store_time) { // 부등호를 >= 으로 설정하면 정확히 그 시점에 파일이 삭제됨.
                    // 이 파일은 오래되어서 삭제
                    // 개념상 큐의 첫 부분임이 분명하므로 deque로 충분하다.
                    // printf("삭제 %s\n", bfq->queue[idx]);
                    sprintf(sysCmd, "rm %s", node->bfq->queue[idx]);
                    if (system(sysCmd) != 0) {
                        fprintf(stderr, "백업된 파일 삭제 실패.\n%s\n", node->bfq->queue[idx]);
                    }
                    backedup_file_del(node->bfq);
                } // end of if time
            } // end of for queue
        } // end of if option t
    } // end of while
    pthread_cleanup_pop(1);
    return NULL;
}

void backup_recursive_dir(const char *absolute_dir, int period, int option_m, int option_n, int maxn, int option_t, int store_time)
{
    static char pathname[512] = "";
    struct dirent *dirp;
    struct stat statbuf;
    char *ptr;
    DIR *dp;
    if (absolute_dir != NULL) {
        strncpy(pathname, absolute_dir, 511);
        pathname[511] = '\0';
    }
    if (strlen(pathname) > 255) {
        fprintf(stderr, "%s 경로는 너무 깁니다. %lu/255\n", pathname, strlen(pathname));
        return;
    }
    
    if (lstat(pathname, &statbuf) < 0) {
        fprintf(stderr, "lstat error for %s\n", pathname);
        return;
    }

    if (S_ISREG(statbuf.st_mode)) {
        // 일반 FILE
        backup_list_append(pathname, period, option_m, option_n, maxn, option_t, store_time); // 백업리스트에 추가
        return;
    }
    else if (!S_ISDIR(statbuf.st_mode))
        return;
    
    // 디렉토리
    ptr = pathname + strlen(pathname);
    *ptr++ = '/';
    *ptr = '\0';

    if ((dp = opendir(pathname)) == NULL) {
        fprintf(stderr, "opendir error for %s\n", pathname);
        return;
    }

    while ((dirp = readdir(dp)) != NULL)
        if (strcmp(dirp->d_name, ".") && strcmp(dirp->d_name, "..")) {
            if (strlen(pathname) + strlen(dirp->d_name) > 255) {
                fprintf(stderr, "건너뜀 : %s (path length is too long)\n", dirp->d_name);
            } else {
                strcpy(ptr, dirp->d_name);
                backup_recursive_dir(NULL, period, option_m, option_n, maxn, option_t, store_time);
            }
        }
    ptr[-1] = 0;
    closedir(dp);
    return;
}

// add명령어에 해당하는 것을 처리
void add_command_action(int argc, char **argv)
{
    int period = 0;
    int m_option = 0;
    int n_option = 0;
    int n_option_number = -1; // 백업 파일의 최대 개수
    int t_option = 0;
    int t_option_number = -1; // 백업 파일의 보관 기간
    int d_option = 0;
    char pathname[512];
    struct stat statbuf;
    // printf("add 액션 처리\n");
    // i) 옵션 처리 -m -n -t -d 처리하기
    if (argc < 3) {
        fprintf(stderr, "Usage :  add <FILENAME> <PERIOD> [OPTION]\n");
        return;
    }

    // 절대 경로화
    get_absolute_path(pathname, argv[1]);
    // 절대 경로의 길이가 255 byte 초과 시 백업 수행 하지 못함.
    if (strlen(pathname) > 255) {
        fprintf(stderr, "%s 경로는 너무 깁니다 %ld/255\n", pathname, strlen(pathname));
        return;
    }

    if (lstat(pathname, &statbuf) < 0 || access(pathname, R_OK) != 0) {
        fprintf(stderr, "해당 파일에 접근할 수 없습니다. : %s\n", pathname);
        return;
    }

    if (!(S_ISREG(statbuf.st_mode) || S_ISDIR(statbuf.st_mode))) {
        fprintf(stderr, "%s is not regular file or directory\n", pathname);
        return;
    }

    period = atoi(argv[2]);
    
    if (strchr(argv[2], '.') != NULL) {
        fprintf(stderr, "PERIOD : positive integer ONLY\n");
        return;
    }
    if (period <= 0) {
        fprintf(stderr, "PERIOD : positive integer ONLY\n");
        return;
    }
    if (!(period >= 5 && period <= 10)) {
        fprintf(stderr, "PERIOD : 5 ~ 10 사이의 값을 입력하십시오.\n");
        return;
    }

    for (int i = 0; i < argc; i++) {
        /*
        -m : 입력받은 PERIOD마다 파일의 mtime이 수정 되었을 경우 백업 실행
        */
        if (!m_option && strcmp("-m", argv[i]) == 0) {
            // fprintf(stderr, "m option enabled\n");
            m_option = 1;
        }
        /*
        -n NUMBER : NUMBER는 백업한 파일의 최대 개수. 가장 최근 NUMBER개의 백업파일 외 나머지 파일은 삭제
        (1) NUMBER는 정수형이며, 정수형이 아닌 실수형 입력 시 에러 처리 후 프롬프트로 제어가 넘어감
        (2) NUMBER 입력 없을 시 에러 처리 후 프롬프트로 제어가 넘어감
        (3) 채점의 편의를 위해 제출할 예제 프로그램에서는 1≤NUMBER≤100로 설정하여 실행시켜 제출
        */
        if (!n_option && strcmp("-n", argv[i]) == 0) {
            // fprintf(stderr, "n option enabled\n");
            n_option = 1;
            if (i + 1 >= argc) {
                fprintf(stderr, "usage : add <filename> <period> -n NUMBER\n");
                return;
            }
            if (strchr(argv[i + 1], '.') != NULL) {
                fprintf(stderr, "NUMBER : positive integer ONLY\n");
                return;
            }
            n_option_number = atoi(argv[i + 1]);
            if (n_option_number <= 0) {
                fprintf(stderr, "NUMBER : positive integer ONLY\n");
                return;
            }
            if (!(n_option_number >= 1 && n_option_number <= 100)) {
                fprintf(stderr, "NUMBER : 1 ~ 100 사이의 값을 입력하십시오.\n");
                return;
            }
        }

        /*
        -t TIME : 백업해야할 파일에 대한 백업 디렉토리 내 보관 기간을 TIME만큼 설정
        (1) TIME는 정수형이며 초를 나타냄. 정수형이 아닌 실수형 입력 시 에러 처리 후 프롬프트로 제어가 넘어감
        (2) TIME 입력 없을 시 에러 처리 후 프롬프트로 제어가 넘어감
        (3) 매 PERIOD마다 백업 파일을 생성하고 기존 모든 백업 파일의 생성시간을 확인. 확인한 파일의 생성시간이 주어진 TIME보다 크면 해당 파일 삭제
        (4) 채점의 편의를 위해 제출할 예제 프로그램에서는 60≤TIME≤1200로 설정하여 실행시켜 제출
        */
        if (!t_option && strcmp("-t", argv[i]) == 0) {
            // fprintf(stderr, "t option enabled\n");
            t_option = 1;
            if (i + 1 >= argc) {
                fprintf(stderr, "usage : add <filename> <period> -t TIME\n");
                return;
            }
            if (strchr(argv[i + 1], '.') != NULL) {
                fprintf(stderr, "TIME : positive integer ONLY\n");
                return;
            }
            t_option_number = atoi(argv[i + 1]);
            if (t_option_number <= 0) {
                fprintf(stderr, "TIME : positive integer ONLY\n");
                return;
            }
            if (!(t_option_number >= 60 && t_option_number <= 1200)) {
                fprintf(stderr, "TIME : 60 ~ 1200 사이의 값을 입력하십시오.\n");
                return;
            }
        }

        /*
        -d : 지정한 디렉토리 내의 모든 파일들을 백업 리스트에 추가
        (1) 한번에 인자로 줄 수 있는 디렉토리는 최대 1개이며 해당 디렉토리 내 모든 파일을 리스트에 추가. 디렉토리 내의 서브디렉토리 내 모든 파일까지 모두 리스트에 추가
        (2) FILENAME이 디렉토리가 아닐 경우 에러 처리 후 프롬프트로 제어가 넘어감
        (3) 디렉토리 내에 있는 파일이 이미 백업 리스트에 존재할 경우 해당 파일은 리스트에 추가하지 않고 건너뜀
        */
        if (!d_option && strcmp("-d", argv[i]) == 0) {
            // fprintf(stderr, "d option enabled\n");
            d_option = 1;
            if (lstat(pathname, &statbuf) < 0) {
                fprintf(stderr, "lstat error\n");
                fprintf(stderr, "%s\n", strerror(errno));
                return;
            }
            if (!S_ISDIR(statbuf.st_mode)) {
                fprintf(stderr, "%s is not directory\n", argv[1]);
                return;
            }
        }
    }

    if (d_option) {
        backup_recursive_dir(pathname, period, m_option, n_option, n_option_number, t_option, t_option_number);
    } else {
        // 리스트에 append
        if(S_ISREG(statbuf.st_mode))
            backup_list_append(pathname, period, m_option, n_option, n_option_number, t_option, t_option_number);
        else  { // 디렉토리 등
            fprintf(stderr, "일반 파일만 백업할 수 있습니다. 디렉터리는 -d 옵션 사용.\n");
            return;
        }
    }
}

// list 명령어 처리
void list_command_action(void)
{
    // LL을 탐색하면서 어떤게 백업 됐는지 출력함.
    // 현재 백업 실행중인 모든 백업 리스트 출력
    // 한 줄에 한 개 파일의 절대경로, PERIOD, OPTION 출력
    struct backup_file_node *curr = backup_list.start;
    while (curr) {
        printf("%s\t[백업 주기 %3d초]\t", curr->pathname, curr->period);
        printf("%s", curr->option_m ? "[수정 시 백업]\t" : "");
        if (curr->option_n)
            printf("[백업본 개수 제한 %3d개]\t", curr->maxn);
        if (curr->option_t)
            printf("[백업본 %3d초간 보존]\t", curr->store_time);
        printf("\n");
        curr = curr->next;
    }
}

void remove_command_action(int argc, char **argv)
{
    char pathname[512];
    struct tm *tm_p;
    struct tm lt;
    time_t now;
    time(&now);
    tm_p = localtime_r(&now, &lt);
    if (argc != 2) {
        fprintf(stderr, "usage :\nremove -a\nremove <filename>\n");
        return;
    }
    if (strlen(argv[1]) == 2 && argv[1][0] == '-' && argv[1][1] == 'a') {
        // 모두 삭제 (백업 중인 작업 중지)
        struct backup_file_node *curr = backup_list.start;
        while (curr) {
            fprintf(log_file, "[%02d%02d%02d %02d%02d%02d] %s deleted\n", (tm_p->tm_year+1900)%100,tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, curr->pathname);
            pthread_detach(curr->backup_thread);
            pthread_cancel(curr->backup_thread);
            curr = curr->next;
        }

        // 모두 삭제 (링크드 리스트에서 백업 리스트 없애기)
        while (backup_list.number_of_nodes != 0) {
            backup_list_pop();
        }
    } else {
        // 지정한 <filename> 삭제
        get_absolute_path(pathname, argv[1]);
        backup_list_delete(pathname); // 해당 파일 백업을 중단
    }
}

void compare_command_action(int argc, char **argv)
{
    char absPath1[512], absPath2[512];
    struct stat statbuf1, statbuf2;
    if (argc != 3) {
        fprintf(stderr, "usage: compare <FILENAME1> <FILENAME2>\n");
        return;
    }
    get_absolute_path(absPath1, argv[1]);
    get_absolute_path(absPath2, argv[2]);
    if (strlen(absPath1) > 255 || strlen(absPath2) > 255) {
        fprintf(stderr, "경로 이름이 너무 깁니다!\n");
        return;
    }
    if (lstat(absPath1, &statbuf1) < 0 || lstat(absPath2, &statbuf2) < 0) {
        fprintf(stderr, "올바르지 않은 파일 경로\n");
        return;
    }
    if (!S_ISREG(statbuf1.st_mode) || !S_ISREG(statbuf2.st_mode)) {
        fprintf(stderr, "일반 파일만 입력가능합니다.\n");
        return;
    }
    if (statbuf1.st_size != statbuf2.st_size || statbuf1.st_mtime != statbuf2.st_mtime) {
        printf("두 파일은 다릅니다.\n%s\tsize : [%ld]\tmtime : [%ld]\n%s\tsize : [%ld]\tmtime : [%ld]\n", absPath1, statbuf1.st_size, statbuf1.st_mtime, absPath2, statbuf2.st_size, statbuf2.st_mtime);
        return;
    }
    printf("두 파일은 동일합니다.\n");
}

void recover_command_action(int argc, char **argv)
{
    char pathname[512];
    char new_pathname[512];
    char sysCmd[1024];
    int number_of_pathList = 0;
    char **recover_pathList = NULL; // 현재 상태를 capture 해 두기 위해서 필요함.
    long *recover_fsizeList = NULL; // file size 저장용
    struct backup_file_node *curr = backup_list.start;
    struct stat statbuf;
    char user_input[4]; // 번호 선택 용
    int user_input_int;
    struct tm lt;
    struct tm *tm_p;
    time_t now;

    // (2) NEWFILE 입력 없을 시 에러 처리 후 프롬프트로 제어가 넘어감
    if (argc != 2 && argc != 4) {
        fprintf(stderr, "usage :\nrecover <filename>\nrecover <filename> -n <newfile>\n");
        return;
    }
    
    get_absolute_path(pathname, argv[1]);
    if (argc == 4) get_absolute_path(new_pathname, argv[3]);

    if (argc == 4 && strcmp(argv[2], "-n") != 0) {
        fprintf(stderr, "usage :\nrecover <filename>\nrecover <filename> -n <newfile>\n");
        return;
    }
    // (3) NEWFILE이 이미 존재한다면 에러 처리 후 프롬프트로 제어가 넘어감
    if (argc == 4 && access(new_pathname, F_OK) == 0) {
        fprintf(stderr, "%s already exists!!\n", new_pathname);
        return;
    }

    if (access(pathname, F_OK) != 0) {
        // 변경할 파일이 존재하지 않으면 에러 처리 후 프롬프트로 제어가 넘어감
        // => 백업과 복원이라는 특성상 파일이 삭제되어도 복원 가능해야 할 거 같은데..
        fprintf(stderr, "파일이 존재하지 않습니다.\n");
        return;    
    }

    while (curr) {
        if (strcmp(curr->pathname, pathname) == 0) break;
        curr = curr->next;
    }

    if (!curr) {
        fprintf(stderr, "이 파일은 백업 중인 파일이 아닙니다.\n");
        return;
    }

    printf("  0. exit\n");
    number_of_pathList = curr->bfq->number_of_nodes;
    recover_pathList = (char **)calloc(number_of_pathList, sizeof(char *));
    recover_fsizeList = (long *)calloc(number_of_pathList, sizeof(long));
    for (int i = 0; i < number_of_pathList; i++) {
        recover_pathList[i] = (char *)calloc(512, sizeof(char));
        int idx = curr->bfq->front + i + 1;
        idx %= curr->bfq->limit;
        // lstat으로 크기를 가져오기.
        lstat(curr->bfq->queue[idx], &statbuf);
        recover_fsizeList[i] = statbuf.st_size;
        strcpy(recover_pathList[i], curr->bfq->queue[idx]);
    }
    for (int i = 0; i < number_of_pathList; i++) {
        printf("%3d. %s\t%10ldbytes\n", i + 1, strrchr(recover_pathList[i], '_') + 1, recover_fsizeList[i]);
    }
    printf("Choose file to recover : ");
    scanf("%3s", user_input);
    while ((user_input_int = getchar()) != EOF && user_input_int != '\n'); // stdin buffer를 비운다.
    user_input_int = atoi(user_input);
    if (user_input_int < 0 || user_input_int > number_of_pathList) {
        fprintf(stderr, "Wrong Choice.\n");
    } else if (user_input_int > 0) {
        if (argc == 2)
            sprintf(sysCmd, "cp %s %s -p > /dev/null 2>&1", recover_pathList[user_input_int - 1], pathname);
        else
            sprintf(sysCmd, "cp %s %s -p > /dev/null 2>&1", recover_pathList[user_input_int - 1], new_pathname);
        if (system(sysCmd) == 0) {
            // 변경할 파일이 현재 백업 리스트에 존재한다면 백업 수행 '종료' 후 복구 진행
            backup_list_delete(pathname); // 해당 파일 백업을 중단
            time(&now);
            tm_p = localtime_r(&now, &lt); // make localtime thread-safe
            if (argc == 2)
                fprintf(log_file, "[%02d%02d%02d %02d%02d%02d] %s recovered\n", (tm_p->tm_year+1900)%100,tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, pathname);
            else 
                fprintf(log_file, "[%02d%02d%02d %02d%02d%02d] %s->%s recovered\n", (tm_p->tm_year+1900)%100,tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, pathname, new_pathname);
            printf("Recovery success\n");
        } else {
            printf("Recovery failed : [백업본이 이미 삭제됨.]\n");
        }
    }
    for (int i = 0; i < number_of_pathList; i++)
        free(recover_pathList[i]);
    if (recover_pathList)
        free(recover_pathList);
    if (recover_fsizeList)
        free(recover_fsizeList);
}
