#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "debug.h"
#include "arg_read.h"
#include "converter.h"
#include "header_table.h"

int brace_stack = 0; // 여닫는 중괄호 계층 스택
// 0->1 로 되면 파일 생성이고 1->0이 되면 파일 close 이다.

void remove_str_in_str(char *dest, const char *anti_pattern)
{
    char *origin_dest = dest;
    char temp[MAX_RESULT_CODE_LENGTH];
    int anti_pattern_len = strlen(anti_pattern);
    char *cp = strstr(dest, anti_pattern);
    if (cp != NULL) {
        while (cp != NULL) {
            *cp = '\0'; // 해당 패턴 시작부분을 자른다.
            strcat(temp, dest); // 그 전과 지금 - 1까지의 모든 것들을 넣는다.
            dest = cp + anti_pattern_len; // 그 다음 검사해야 할 부분
            cp = strstr(dest, anti_pattern);
            if (!cp) {
                strcat(temp, dest);
            }
        }
        strncpy(origin_dest, temp, MAX_RESULT_CODE_LENGTH);   
    }
}

void print_repeat(char p, int cnt) {
    while(cnt--) {
        putchar(p);
    }
}

int main(int argc, char *argv[])
{

    read_header_table("header_table.txt");
/*
    if (argc < 2) {
        fprintf(stderr, "Usage : %s [java_source_file] [option]...\n", argv[0]);
        exit(1);
    }

    if (access(argv[1], R_OK) < 0) {
        fprintf(stderr, "읽을 수 없는 파일 : %s\n", argv[1]);
        exit(1);
    }

    parse_args(argc-1, argv+1); // argv[1] (pathname) 제외

    FILE *fp = fopen(argv[1], "r");
    char input[MAX_RESULT_CODE_LENGTH]; // 소스 코드의 길이 최대 4096 바이트
    char output[MAX_RESULT_CODE_LENGTH];
    size_t f_size = fread(input, sizeof(char), MAX_RESULT_CODE_LENGTH, fp);
    if(f_size) convert_java_to_c(output, input);
    fclose(fp);*/
    exit(0);
}
