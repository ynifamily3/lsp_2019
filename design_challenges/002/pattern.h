#ifndef PATTERN
#define PATTERN
#include <string.h>
#include <ctype.h>
#include "lex.h"

#define NUMBER_OF_PATTERNS 3

typedef struct {
    int java_pattern_length;
    int c_pattern_length;
    int java_pattern[PATTERN_SIZE];


    char java_pattern_buffer[PATTERN_SIZE][LEX_SIZE + 1]; // 패턴 실제 내용

    int c_pattern[PATTERN_SIZE];
    // c_pattern 에 음수가 올 경우 문자열 리터럴 순서를 바꿀수 있게 하기 위함 (collision 방지)
    // 0이면 어떡함?
} _patternChanger;

int is_match(int numPattern, _patternChanger **__patt, _lexPattern *pattern);
void convertJavatoC(_patternChanger *patt, char *resultbuf);


#endif