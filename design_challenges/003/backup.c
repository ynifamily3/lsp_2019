#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

extern char binary_directory[512]; // in main c

FILE *log_file;

void *file_backup_thr(void *);

struct backup_file_node {
    struct backup_file_node *next;
    char pathname[512];
    
    int period;
    int option_m; // 입력받은 PERIOD마다 파일의 mtime이 수정 되었을 경우 백업 실행

    int option_n; // -n NUMBER : NUMBER는 백업한 파일의 최대 개수. 가장 최근 NUMBER개의 백업파일 외 나머지 파일은 삭제
    int maxn;
    
    int option_t; // -t TIME : 백업해야할 파일에 대한 백업 디렉토리 내 보관 기간을 TIME만큼 설정
    int store_time; // 보관 기간
    // int option_d; // -d : 지정한 디렉토리 내의 모든 파일들을 백업 리스트에 추가
    pthread_t backup_thread;
};

struct backups_head {
    int number_of_nodes;
    struct backup_file_node *start;
};

struct backups_head backup_list; // 백업 리스트 전역 변수 선언


// 백업파일 관리를 위한 순환 queue, 큐 사이즈는 최대 100
struct backuped_file_queue {
    int front, rear;
    int limit; // n옵션의 인자가 된다. (최대 100의 값이 들어온다고 가정)
    int number_of_nodes;
    char *queue[101];

};

void backuped_file_init(struct backuped_file_queue *bfq, int n_option_arg) {
    bfq->number_of_nodes = 0;
    bfq->front = bfq->rear = 0;
    bfq->limit = n_option_arg + 1;
}

void backuped_file_add(struct backuped_file_queue *bfq, const char *data) {
    if ((bfq->rear+1) % bfq->limit == bfq->front) {
        fprintf(stderr, "backuped_file fully added!!\n");
        return;
    }
    ++bfq->number_of_nodes;
    ++bfq->rear;
    bfq->rear %= bfq->limit;
    char *tmp = (char *)calloc(1, strlen(data) + 1); // => 이건 에러 없음.
    bfq->queue[bfq->rear] = tmp;
    strncpy(bfq->queue[bfq->rear], data, strlen(data));
    bfq->queue[bfq->rear][strlen(data)] = '\0';

}

// 처음에 추가된 data 가 빠져 나간다. (제일 오래된 백업)
void backuped_file_del(struct backuped_file_queue *bfq) {
    if (bfq->front == bfq->rear) {
        fprintf(stderr, "queue is empty\n");
        return;
    }
    --bfq->number_of_nodes;
    ++bfq->front;
    bfq->front %= bfq->limit;
    // printf("free.. [%d] : %s\n", bfq->front, bfq->queue[bfq->front]);
    bfq->queue[bfq->front] = NULL;
    free(bfq->queue[bfq->front]); // 쓰이지 않을 데이터는 힙에서 해제
}


void backup_list_init()
{
    backup_list.number_of_nodes = 0;
    backup_list.start = NULL;
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
    while (curr) {
        if (strcmp(curr->pathname, pathname) == 0) {
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
        backup_list_pop(); //헤드에 있는 것
        return;
    }
    prev->next = curr->next;
    free(curr);
    --backup_list.number_of_nodes;
}

void backup_list_append(char *pathname, int period, int option_m, int option_n, int maxn, int option_t, int store_time)
{
    /*
    printf("============ 다음 파일을 백업합니다. ============\n");
    printf("파일명 : %s\n", pathname);
    printf("백업 주기 : %d초마다\n", period);
    if (option_m) {
        printf("파일이 수정될 때에 한해 주기마다 백업을 진행합니다.\n");
    } else {
        printf("파일 수정과 관계없이 주기마다 백업을 진행합니다.\n");
    }
    if (option_n) {
        printf("최대 %d개의 백업본을 만듭니다.\n", maxn);
    } else {
        printf("무제한의 백업 사본을 생성합니다.\n");
    }
    if (option_t) {
        printf("백업 파일은 최대 %d초간 생존합니다.\n", store_time);
    } else {
        printf("백업된 파일은 얼마가 지나든 계속 살아 있습니다.\n");
    }
    printf("=================================================\n");
    */

    // 링크드 리스트 구성하기.
    struct backup_file_node *node = (struct backup_file_node *)calloc(1, sizeof(struct backup_file_node));
    if (backup_list.number_of_nodes != 0)
        node->next = backup_list.start;
    else
        node->next = NULL;
    strncpy(node->pathname, pathname, 511);
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
        // printf("n 옵션이 없으므로 해제할 것도 없음\n");
        return;
    }
    struct backuped_file_queue *bfq = (struct backuped_file_queue *)args;
    // printf("이 파일은 더 이상 백업하지 않으므로 메모리를 해제합니다.");
    // 큐에 들어 있는 모든 것을 pop해야 됨.
    // 하지만, 실제로 파일은 삭제하지 않는다. -n 옵션에서 걸린 것이 아니라, 단순히 백업을 중지하기 때문이다.
    while(bfq->front != bfq->rear)
        backuped_file_del(bfq);
    free(args);
}

void *file_backup_thr(void *args)
{
    struct backup_file_node *node = (struct backup_file_node *)args;
    struct backuped_file_queue *bfq = NULL; //  백업된 파일 리스트
    struct tm *tm_p;
    char backuped_pathname[512];
    time_t now;
    time(&now);
    tm_p = localtime(&now);

    if (node->option_n) {
        // n option enabled
        bfq = (struct backuped_file_queue *)calloc(1, sizeof(struct backuped_file_queue));
        backuped_file_init(bfq, node->maxn);
        // 쓰레드가 끝나기 전에 bfq에 할당된 동적 메모리가 반납되어야 한다.
    }

    pthread_cleanup_push(cleanup, bfq);

    fprintf(log_file, "[%02d%02d%02d %02d%02d%02d] %s added\n", (tm_p->tm_year+1900)%100,tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, node->pathname);
    while (1) {
        sleep(node->period); // period 마다 백업을 진행함
        time(&now);
        tm_p = localtime(&now);
        if (node->option_n) {
            // 여기서부터 구현하면 된다.
            if (bfq->number_of_nodes >= bfq->limit - 1) {
                backuped_file_del(bfq);
            }
        }
        sprintf(backuped_pathname, "%s_%02d%02d%02d%02d%02d%02d", node->pathname, (tm_p->tm_year+1900)%100, tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec);
        fprintf(log_file, "[%02d%02d%02d %02d%02d%02d] %s generated\n", (tm_p->tm_year+1900)%100, tm_p->tm_mon + 1, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, backuped_pathname);
        if (node->option_n) {
            // 백업된 파일 리스트에 추가한다.
            // printf("백업됨 : %s\n", backuped_pathname);
            backuped_file_add(bfq, backuped_pathname);
        }
    }

    pthread_cleanup_pop(1);

    return NULL;
}

void backup_list_print()
{

}


/*
백업해야할 파일(FILENAME)을 백업 리스트에 새롭게 추가
백업 리스트는 링크드 리스트로 구현
백업 리스트 구조체는 1. 파일의 절대경로, 2. 백업 주기, 3. 백업 옵션 등을 포함

라. add 명령어 10
마. add –m 옵션 4
바. add –n 옵션 4
사. add –t 옵션 5
아. add –d 옵션 

스트럭트 *백업파일헤드 {
    백업파일개수
    
    스트럭트 * 백업파일첫노드
}


스트럭트 백업파일노드 {
    스트럭트* 백업파일노드 넥스트
    파일의 절대경로
    백업 주기
    백업 옵션 (mntd)
    담당쓰레드 정보
}

*/

/*
 * 절대경로로 디렉토리가 주어지면, 모든 파일 리스트를 뱉음. (재귀적으로)
 */

void twae(const char *absolute_dir, int period, int option_m, int option_n, int maxn, int option_t, int store_time)
{
    static char pathname[512] = "";
    struct dirent *dirp;
    struct stat statbuf;
    char *ptr;
    DIR *dp;
    if (absolute_dir != NULL)
        strncpy(pathname, absolute_dir, 511);
    
    if (strlen(pathname) > 255) {
        fprintf(stderr, "path length is too long !\n");
        return;
    }
    
    if (lstat(pathname, &statbuf) < 0) {
        fprintf(stderr, "lstat error for %s\n", pathname);
        return;
    }

    if (S_ISREG(statbuf.st_mode)) {
        // 일반 FILE
        // printf("%s\n", pathname);
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
            strncpy(ptr, dirp->d_name, 511);
            twae(NULL, period, option_m, option_n, maxn, option_t, store_time);
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
    int n_option_number = 0; // 백업 파일의 최대 개수
    int t_option = 0;
    int t_option_number = 0; // 백업 파일의 보관 기간
    int d_option = 0, d_path_pos;
    char pathname[512];
    // printf("add 액션 처리\n");
    // i) 옵션 처리 -m -n -t -d 처리하기
    if (argc < 3) {
        fprintf(stderr, "Usage :  add <FILENAME> [PERIOD] [OPTION]\n");
        return;
    }

    // 절대 경로화
    if (argv[1][0] != '/') {
        strncpy(pathname, binary_directory, 511);
        strncat(pathname, "/", 511);
        strncat(pathname, argv[1], 511);
    } else {
        strncpy(pathname, argv[1], 511);
    }
    struct stat statbuf;
    if (lstat(pathname, &statbuf) < 0 && access(pathname, R_OK) != 0) {
        fprintf(stderr, "해당 파일에 접근할 수 없습니다. : %s\n", pathname);
        return;
    }

    if (!S_ISREG(statbuf.st_mode)) {
        fprintf(stderr, "%s is not regular file\n", pathname);
        return;
    }

    period = atoi(argv[2]);
    
    if (strchr(argv[2], '.') != NULL) {
        fprintf(stderr, "PERIOD : positive integer ONLY\n");
        return;
    }
    if (period <= 0) {
        fprintf(stderr, "PERIOD error\n");
        return;
    }

    for (int i = 0; i < argc; i++) {
        /*
        -m : 입력받은 PERIOD마다 파일의 mtime이 수정 되었을 경우 백업 실행
        */
        if (!m_option && strcmp("-m", argv[i]) == 0) {
            fprintf(stderr, "m option enabled\n");
            m_option = 1;
        }
        /*
        -n NUMBER : NUMBER는 백업한 파일의 최대 개수. 가장 최근 NUMBER개의 백업파일 외 나머지 파일은 삭제
        (1) NUMBER는 정수형이며, 정수형이 아닌 실수형 입력 시 에러 처리 후 프롬프트로 제어가 넘어감
        (2) NUMBER 입력 없을 시 에러 처리 후 프롬프트로 제어가 넘어감
        (3) 채점의 편의를 위해 제출할 예제 프로그램에서는 1≤NUMBER≤100로 설정하여 실행시켜 제출
        */
        if (!n_option && strcmp("-n", argv[i]) == 0) {
            fprintf(stderr, "n option enabled\n");
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
                fprintf(stderr, "invalid NUMBER : %s\n", argv[i + 1]);
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
            fprintf(stderr, "t option enabled\n");
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
                fprintf(stderr, "invalid TIME : %s\n", argv[i + 1]);
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
            fprintf(stderr, "d option enabled\n");
            d_option = 1;
            if (i + 1 >= argc) {
                fprintf(stderr, "usage : add <filename> <period> -d DIRECTORY\n");
                return;
            }
            struct stat statbuf;
            if (lstat(argv[i+1], &statbuf) < 0) {
                fprintf(stderr, "lstat error\n");
                return;
            }
            if (!S_ISDIR(statbuf.st_mode)) {
                fprintf(stderr, "%s is not directory\n", argv[i + 1]);
                return;
            }
            d_path_pos = i + 1;
            printf("%s is directory\n", argv[i + 1]);
        }
    }
    // 리스트에 append
    backup_list_append(pathname, period, m_option, n_option, n_option_number, t_option, t_option_number);

    // -d 옵션이 있는 경우 그 파일들을 모두 백업 처리 (옵션은 똑같이 처리하여 구조체에 넣는다.)
    // 절대 경로화 필요.
    if (d_option) {
        // 일단 모든 파일을 출력해보기 테스트
        // 절대 경로화
        if (argv[d_path_pos][0] != '/') {
            strncpy(pathname, binary_directory, 511);
            strncat(pathname, "/", 511);
            strncat(pathname, argv[d_path_pos], 511);
        } else {
            strncpy(pathname, argv[d_path_pos], 511);
        }
        twae(pathname, period, m_option, n_option, n_option_number, t_option, t_option_number);

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
        printf("%s\t%d\n", curr->pathname, curr->period);
        curr = curr->next;
    }
}

void remove_command_action(int argc, char **argv)
{
    char pathname[512];
    if (argc != 2) {
        fprintf(stderr, "usage :\nremove -a\nremove <filename>\n");
        return;
    }
    if (strlen(argv[1]) == 2 && argv[1][0] == '-' && argv[1][1] == 'a') {
        // 모두 삭제 (백업 중인 작업 중지)
        struct backup_file_node *curr = backup_list.start;
        while (curr) {
            printf("백업 중지 : %s\n", curr->pathname);
            pthread_cancel(curr->backup_thread);
            curr = curr->next;
        }

        // 모두 삭제 (링크드 리스트에서 백업 리스트 없애기)
        while (backup_list.number_of_nodes != 0) {
            backup_list_pop();
        }
    } else {
        // 지정한 <filename> 삭제
        // 절대 경로화
        if (argv[1][0] != '/') {
            strncpy(pathname, binary_directory, 511);
            strncat(pathname, "/", 511);
            strncat(pathname, argv[1], 511);
        } else {
            strncpy(pathname, argv[1], 511);
        }
        backup_list_delete(pathname); // 해당 파일 백업을 중단
    }
}