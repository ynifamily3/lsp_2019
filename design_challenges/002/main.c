#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "debug.h"
#include "arg_read.h"
#include "converter.h"
#include "header_table.h"

char pathname[1024]; // (상대/절대) 파일경로 ./ 또는 /로 시작하거나 asdf/... 
char filename[256]; // 순수 파일명 xx.java

// 0->1 로 되면 파일 생성이고 1->0이 되면 파일 close 이다.
int brace_stack = 0; // 여닫는 중괄호 계층 스택

int is_main = 0; // 현재 변환중인 곳이 main 함수 내이면 1, 아니라면 0

int main(int argc, char *argv[])
{
    read_header_table("header_table.txt");
    
    if (argc < 2) {
        fprintf(stderr, "Usage : %s [java_source_file] [option]...\n", argv[0]);
        exit(1);
    }

    if (access(argv[1], R_OK) < 0) {
        fprintf(stderr, "읽을 수 없는 파일 : %s\n", argv[1]);
        exit(1);
    }

    //parse_args(argc-1, argv+1); // argv[1] (pathname) 제외
    parse_args(argc, argv); // argv[1] (pathname) 제외    

    FILE *fp = fopen(java_file_name, "r"); // 상대 경로 / 절대 경로 지정시 문제
    char input[MAX_RESULT_CODE_LENGTH]; // 소스 코드의 길이 최대 4096 바이트
    char output[MAX_RESULT_CODE_LENGTH];
    size_t f_size = fread(input, sizeof(char), MAX_RESULT_CODE_LENGTH, fp);
    if(f_size) convert_java_to_c(output, input);
    fclose(fp);
    // makefile 생성하기  q1_Makefile
    char noextFn[30];
    char makeFileFn[40];
    for (int i = 0; (java_file_name[i] != '.' && java_file_name[i] != '\0'); i++) {
        noextFn[i] = java_file_name[i];
    }
    sprintf(makeFileFn, "%s_Makefile", noextFn);
    fprintf(stderr, "테스트 : %s .... %s\n", noextFn, makeFileFn);
    fp = fopen(makeFileFn, "w");
    fprintf(fp, "%s : %s.c\n\tgcc %s.c -o %s", noextFn, noextFn, noextFn, noextFn);
    fclose(fp);
    exit(0);
}
