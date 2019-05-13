#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>

struct backup_file_node {
    struct backup_file_node *next;
    char pathname[256];
    
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

void backup_list_init()
{
    backup_list.number_of_nodes = 0;
    backup_list.start = NULL;
}

void backup_list_delete(char *pathname)
{
    if(pathname) {

    }
}

void backup_list_append(char *pathname, int period, int option_m, int option_n, int maxn, int option_t, int store_time)
{
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

void twae(const char *absolute_dir)
{
    static char pathname[512] = "";
    struct dirent *dirp;
    struct stat statbuf;
    char *ptr;
    DIR *dp;
    if (absolute_dir != NULL)
        strncpy(pathname, absolute_dir, 512);
    
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
        printf("%s\n", pathname);
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
            strncpy(ptr, dirp->d_name, 512);
            twae(NULL);
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
    int d_option = 0;
    char *pathname;
    // printf("add 액션 처리\n");
    // i) 옵션 처리 -m -n -t -d 처리하기
    if (argc < 3) {
        fprintf(stderr, "Usage :  add <FILENAME> [PERIOD] [OPTION]\n");
        return;
    }

    if (strcmp("-d", argv[1]) == 0) {
        // add -d testdir 5 같은 경우.
        // pass
        pathname = NULL;
        period = atoi(argv[3]);
    } else {
        // 나중에 상대 경로 처리를 해 줘야 한다. 상대->절대
        pathname = argv[1];
        struct stat statbuf;
        if (stat(argv[1], &statbuf) < 0) {
            fprintf(stderr, "stat error : %s\n", argv[1]);
            return;
        }
        if (!S_ISREG(statbuf.st_mode)) {
            fprintf(stderr, "%s is not regular file\n", argv[1]);
            return;
        }
        period = atoi(argv[2]);
    }
    
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
                fprintf(stderr, "usage : add -d DIRECTORY <period>\n");
                return;
            }
            struct stat statbuf;
            // d옵션에서 인자를 상대경로로 주면 뭐지?
            // 일단 절대 경로 기준으로만 처리하기로 함 나중에 수정
            if (stat(argv[i+1], &statbuf) < 0) {
                fprintf(stderr, "stat error\n");
                return;
            }
            if (!S_ISDIR(statbuf.st_mode)) {
                fprintf(stderr, "%s is not directory\n", argv[i + 1]);
                return;
            }
            printf("%s is directory\n", argv[i + 1]);
        }
    }
    // -d 옵션이 있는 경우 그 파일들을 모두 백업 처리 (옵션은 똑같이 처리하여 구조체에 넣는다.)
    if (d_option) {

    }

    // 리스트에 append
    backup_list_append(pathname, period, m_option, n_option, n_option_number, t_option, t_option_number);
}