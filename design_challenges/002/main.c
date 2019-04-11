#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "debug.h"
#include "arg_read.h"
#include "converter.h"

int brace_stack = 0; // 여닫는 중괄호 계층 스택
// 0->1 로 되면 파일 생성이고 1->0이 되면 파일 close 이다.

void remove_str_in_str(char *dest, const char *anti_pattern)
{
    // char haha[100] = "System.out.printf(\"%d\", st.top());";
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
    char input[MAX_RESULT_CODE_LENGTH];
    char output[MAX_RESULT_CODE_LENGTH];
    while(fgets(input, sizeof(input), fp)) {
        // 마지막 개행 (lf, crlf, lfcr, cr) 제거
        input[strcspn(input, "\r\n")] = '\0';
        if(strlen(input)) {
            convert_java_to_c(output, input);
            // printf("브레이스스택 :%d\n", brace_stack);
            if(strlen(output)) {
                //print_repeat('\t', brace_stack);
                printf("%s\n%s\n------------------------\n", input, output);
                // *최신 :  탭이나 공백으로만 이루어진 line에 버그가 있다. 그 외에 한 글자 키워드에 버그 있음.
            }
        }
        // getchar();
    }
    fclose(fp);



    // 문자열 내 특정 문자열을 제거하는 함수. (^^)
    char haha[100] = "1234";
    remove_str_in_str(haha, "st.");
    printf("[%s]\n", haha);
    /*
    char input2[100] = "inta=3023;";
    char output2[100];
    convert_java_to_c(output2, input2);
    printf("%s\n", output2);
    */
    exit(0);
}

/*
    char input[100] = "for(int i = 0; i < 88; i+=99*abc);"; // 버그 찾음 * operation 인식을 안함
    char output[100];
    convert_java_to_c(output, input);
    printf("%s\n", output);
*/


