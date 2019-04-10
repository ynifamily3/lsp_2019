#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug.h"
#include "arg_read.h"
#include "converter.h"

int brace_stack = 0; // 여닫는 중괄호 계층 스택
// 0->1 로 되면 파일 생성이고 1->0이 되면 파일 close 이다.

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
            printf("브레이스스택 :%d\n", brace_stack);
            if(strlen(output))
                printf("%s\n", output);
        }
        // getchar();
    }
    /*
    char input2[100] = "g";
    char output2[100];
    convert_java_to_c(output2, input2);
    printf("%s\n", output2);
    */

    fclose(fp);
    exit(0);
}

/*
    char input[100] = "for(int i = 0; i < 88; i+=99*abc);"; // 버그 찾음 * operation 인식을 안함
    char output[100];
    convert_java_to_c(output, input);
    printf("%s\n", output);
*/


