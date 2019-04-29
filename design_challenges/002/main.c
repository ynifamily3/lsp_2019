#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "debug.h"
#include "arg_read.h"
#include "converter.h"
#include "header_table.h"
#include "ssu_runtime.h"

char dir_path[512]; // (상대/절대) 파일경로 ./ 또는 /로 시작하거나 asdf/... 
char filename[256]; // 순수 파일명 xx.java
char makefilename[256]; // xx_Makefile
char pathname[767]; // dir_path + filename

char input[MAX_RESULT_CODE_LENGTH]; // 소스 코드의 길이 최대 4096 바이트
char output[MAX_RESULT_CODE_LENGTH];

// 0->1 로 되면 파일 생성이고 1->0이 되면 파일 close 이다.
int brace_stack = 0; // 여닫는 중괄호 계층 스택

int is_main = 0; // 현재 변환중인 곳이 main 함수 내이면 1, 아니라면 0

extern int produces_c_source_file_numbers; // 프로듀스된 C들
extern char (c_source_file_names)[5][767]; // C언어 파일 이름들 (full path)
extern char (c_source_file_names_print)[5][256]; // C언어 파일 이름들 (순수 path)
extern int go_include;
char gccs[1280]; // gcc 파일 리스트 쭈욱 이어 붙인 것
// 프로듀스 된 C들 >= 2 && go_include 여부 판별 1시 => make 옵션을 다르게 줌

int main(int argc, char *argv[])
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    read_header_table("header_table.txt");
    
    if (argc < 2) {
        fprintf(stderr, "Usage : %s [java_source_file] [option]...\n", argv[0]);
        exit(1);
    }

    if (access(argv[1], R_OK) < 0) {
        fprintf(stderr, "읽을 수 없는 파일 : %s\n", argv[1]);
        exit(1);
    }

    parse_args(argc, argv); // argv[1] (pathname) 제외

    // 상대 경로 부분과 절대 경로 부분 추출
    int last = strlen(java_file_name) - 1;
    while (last >= 0 && java_file_name[last] != '/')
        --last;
    ++last;
    strncpy(dir_path, java_file_name, last); // path
    strcpy(filename, java_file_name + last); // name
    strcpy(pathname, dir_path); // pathname
    strcat(pathname, filename); // pathname
    DBGMSG("path : %s\nname : %s\npathname : %s\n", dir_path, filename, pathname);

    
    FILE *fp = fopen(pathname, "r");
    size_t f_size = fread(input, sizeof(char), MAX_RESULT_CODE_LENGTH, fp);
    if(f_size) convert_java_to_c(output, input);
    fclose(fp);
    // makefile 생성하기  q1_Makefile
    char noextFn[256];
    memset(noextFn, 0x00, 256);
    for (int i = 0; (filename[i] != '.' && filename[i] != '\0'); ++i) {
        noextFn[i] = filename[i];
    }
    sprintf(makefilename, "%s%s_Makefile", dir_path, noextFn);
    // DBGMSG("test : %s\n", makefilename);
    DBGMSG("go_include 여부 : %d", go_include);
    fp = fopen(makefilename, "w");
    if (produces_c_source_file_numbers >= 2 && go_include == 1) {
        //q2 : q2.c Stack.c\n\tgcc q2.c Stack.c -w -o q2
        strcpy(gccs, c_source_file_names_print[0]);
        for (int i = 1; i < produces_c_source_file_numbers; ++i) {
            strcat(gccs, " ");
            strcat(gccs, c_source_file_names_print[i]);
        }
        fprintf(fp, "%s : %s\n\tgcc %s -std=c99 -w -o %s", noextFn, gccs, gccs, noextFn);
    } else {
        fprintf(fp, "%s : %s.c\n\tgcc %s.c -std=c99 -o %s", noextFn, noextFn, noextFn, noextFn);
    }
    fclose(fp);
    gettimeofday(&end, NULL);
    ssu_runtime(&start, &end);
    exit(0);
}
