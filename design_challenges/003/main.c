#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "sys/stat.h"
#include "sys/types.h"
#include "command_checker.h"
#include "backup.h"

int main(int argc, char *argv[])
{
    size_t input_command_length;
    char backup_directory[512];
    char input_command[512];
    char *backup_postfix = "ssu_backup_dir";
    if (argc > 2) {
        fprintf(stderr, "usage : %s [backup_directory]\n", argv[0]);
        exit(1);
    }

    if (argc == 2) {
        if (argv[1][0] == '/') {
            strncpy(backup_directory, argv[1], 512);
            if (chdir(backup_directory) < 0) {
                fprintf(stderr, "usage : %s [backup_directory]\n", argv[0]);
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
            if (mkdir(backup_postfix, 0777) < 0) {
                fprintf(stderr, "백업 디렉토리 생성 실패\n");
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
            if (chdir(backup_postfix) < 0) {
                fprintf(stderr, "usage : %s [backup_directory]\n", argv[0]);
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
        } else {
            // 절대 경로가 주어지지 않은 경우
            if (chdir(argv[1]) < 0) {
                fprintf(stderr, "usage : %s [backup_directory]\n", argv[0]);
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
            if (mkdir(backup_postfix, 0777) < 0) {
                fprintf(stderr, "백업 디렉토리 생성 실패\n");
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
            if (chdir(backup_postfix) < 0) {
                fprintf(stderr, "usage : %s [backup_directory]\n", argv[0]);
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
        }
    } else {
        // 인자 x
        if (mkdir(backup_postfix, 0777) < 0) {
                fprintf(stderr, "백업 디렉토리 생성 실패\n");
                fprintf(stderr, "%s\n", strerror(errno));
                //exit(1);
            }
        if (chdir(backup_postfix) < 0) {
            fprintf(stderr, "해당 경로에 백업할 수 없습니다.\n");
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
    }
    getcwd(backup_directory, 512);
    printf("백업 디렉토리 : %s\n", backup_directory);
    setbuf(stdout, NULL);
    backup_list_init();
    while (1) {
        printf("20142455>");
        fgets(input_command, 512, stdin);
        input_command_length = strlen(input_command); // 아무것도 안 치고 엔터만 눌러도 '\n'은 입력된다.
        input_command[--input_command_length] = '\0';
        if (check_exit(input_command)) {
            // 백업 중인 모든 파일 중단 루틴 실행
            exit(0);
        } else if (check_ls(input_command) || check_vim(input_command)) {
            system(input_command);
        } else if (check_add(input_command)) {
            // opt parse 
            int add_argc;
            char *add_argv[11];
            
            // 아규먼트를 위한 메모리 할당 (in parse_args)
            parse_args(input_command, &add_argc, add_argv);
            for (int i = 0; i < add_argc; i++) {
                printf("add_argv[%d] = %s\n", i, add_argv[i]);
            }
            
            // add Action 실행
            add_command_action(add_argc, add_argv);
            
            // 메모리 해제
            for (int i = 0; i < add_argc; i++)
                free(add_argv[i]); // 메모리 해제
        }
        else if (input_command_length != 0) {
            printf("Invalid command.\n");
        }
    }
    exit(0);
}