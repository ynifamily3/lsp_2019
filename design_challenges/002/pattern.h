#ifndef PATTERN
#define PATTERN
#include <string.h>
#include <ctype.h>
#include "lex.h"

#define MAX_RESULT_CODE_LENGTH 100 // 줄당 코드 길이 최댓값
#define NUMBER_OF_PATTERNS 3

enum PATTERN_TYPE {EXACT, STARTSWITH, ENDSWITH, STARTS_ENDS_WITH};
// 패턴의 종류 : 정확히 일치, ~로 시작, ~로 끝남,~로 시작하고 ~로 끝남.

typedef struct {
    int pattern_type;
    int java_pattern_length;
    int c_pattern_length;
    int java_pattern[PATTERN_SIZE];
    int c_pattern[PATTERN_SIZE];
} _patternChanger;

void PATT_pattern_compile(const _lexPattern *pattern, char *resultbuf);


#endif