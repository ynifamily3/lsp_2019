#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "command_checker.h"
#include "backup.h"

char binary_directory[512]; // 바이너리 기준 현재 위치의 절대 경로
char backup_directory[512]; // 백업 디렉토리 위치의 절대 경로
extern FILE *log_file;

int main(int argc, char *argv[])
{
    size_t input_command_length;
    char input_command[1024];
    char *sys_init_command = "rm -rf ssu_backup_dir/* > /dev/null 2>&1";
    char *backup_postfix = "ssu_backup_dir";
    if (argc > 2) {
        fprintf(stderr, "usage : %s [backup_directory]\n", argv[0]);
        exit(1);
    }

    backup_list_init(); // 초기화
    getcwd(binary_directory, 511); // 해당 프로그램이 있는 현재 경로의 절댓값.

    if (argc == 2) {
        if (argv[1][0] == '/') {
            strncpy(backup_directory, argv[1], 511);
            if (chdir(backup_directory) < 0) {
                fprintf(stderr, "usage : %s [backup_directory]\n", argv[0]);
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
            system(sys_init_command); // 기존 백업 폴더 내부 내용 삭제
            mkdir(backup_postfix, 0777);
            if (chdir(backup_postfix) < 0) {
                fprintf(stderr, "백업 디렉토리 %s/%s에 접근 오류\n", backup_directory, backup_postfix);
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
            system(sys_init_command); // 기존 백업 폴더 내부 내용 삭제
            mkdir(backup_postfix, 0777);
            if (chdir(backup_postfix) < 0) {
                fprintf(stderr, "백업 디렉토리 %s에 접근 오류\n", backup_postfix);
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
        }
    } else {
        // 인자 x
        system(sys_init_command); // 기존 백업 폴더 내부 내용 삭제
        mkdir(backup_postfix, 0777);
        if (chdir(backup_postfix) < 0) {
            fprintf(stderr, "백업 디렉토리 %s에 접근 오류\n", backup_postfix);
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
    }
    // create log file
    log_file = fopen("log.txt", "a");
    setbuf(log_file, NULL); // 로그 버퍼 없음

    getcwd(backup_directory, 511);
    printf("현재 경로 : %s\n", binary_directory);
    printf("백업 경로 : %s\n", backup_directory);
    setbuf(stdout, NULL);
    backup_list_init();
    chdir(binary_directory); // 바이너리 위치 기준으로 현재 위치 설정함.

    while (1) {
        printf("20142455>");
        fgets(input_command, 1023, stdin);
        input_command_length = strlen(input_command); // 아무것도 안 치고 엔터만 눌러도 '\n'은 입력된다.
        input_command[--input_command_length] = '\0';
        if (check_exit(input_command)) {
            // 백업 중인 모든 파일 중단 루틴 실행
            fclose(log_file);
            exit(0);
        } else if (check_ls(input_command) || check_vim(input_command)) {
            system(input_command);
        } else if (check_add(input_command)) {
            // opt parse 
            int add_argc;
            char *add_argv[11];
            // 아규먼트를 위한 메모리 할당 (in parse_args)
            parse_args(input_command, &add_argc, add_argv);
            // add Action 실행
            add_command_action(add_argc, add_argv);
            // 메모리 해제
            for (int i = 0; i < add_argc; i++)
                free(add_argv[i]); // 메모리 해제
        } else if (check_list(input_command)) {
            list_command_action();
        } else if (check_remove(input_command)) {
            int remove_argc;
            char *remove_argv[11];
            parse_args(input_command, &remove_argc, remove_argv);
            remove_command_action(remove_argc, remove_argv);
            for (int i = 0; i < remove_argc; i++)
                free(remove_argv[i]);
        } else if (check_compare(input_command)) {
            int compare_argc;
            char *compare_argv[11];
            parse_args(input_command, &compare_argc, compare_argv);
            compare_command_action(compare_argc, compare_argv);
            for (int i = 0; i < compare_argc; i++)
                free(compare_argv[i]);
        } else if (check_recover(input_command)) {
            int recover_argc;
            char *recover_argv[11];
            parse_args(input_command, &recover_argc, recover_argv);
            recover_command_action(recover_argc, recover_argv);
            for (int i = 0; i < recover_argc; i++)
                free(recover_argv[i]);
        }
        else if (input_command_length != 0) {
            printf("Invalid command.\n");
        }
    }
    exit(0);
}