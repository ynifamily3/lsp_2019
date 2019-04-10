#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "arg_read.h"
#include "lex.h"
#include "pattern.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage %s <java_source_file> -[jcpflr]\n", argv[0]);
        exit(1);
    }

    if (access(argv[1], R_OK) < 0) {
        fprintf(stderr, "읽을 수 없는 파일 : %s\n", argv[1]);
        exit(1);
    }

    parse_args(argc-1, argv+1); // argv[1] (pathname) 제외
/*
    _lexPattern pattern, pattern2, pattern3;
    char input[MAX_RESULT_CODE_LENGTH] = "stk99 = new int[STK_SIZE];";
    char input2[MAX_RESULT_CODE_LENGTH] = "while(abc > 300) {";
    char input3[MAX_RESULT_CODE_LENGTH] = "num = scn.nextInt();";
    char result[MAX_RESULT_CODE_LENGTH];
    char result2[MAX_RESULT_CODE_LENGTH];
    char result3[MAX_RESULT_CODE_LENGTH];

    lex(&pattern, input); // 패턴 분석하고 pattern에 그 정보 저장
    lex(&pattern2, input2);
    lex(&pattern3, input3);

    PATT_pattern_compile(&pattern, result); // pattern을 기반으로 한 C코드로 바꾸고 result에 씀
    PATT_pattern_compile(&pattern2, result2);
    PATT_pattern_compile(&pattern3, result3);

    printf("%s ==> %s\n", input, result);
    printf("%s ==> %s\n", input2, result2);
    printf("%s ==> %s\n", input3, result3);
*/
    exit(0);
}
