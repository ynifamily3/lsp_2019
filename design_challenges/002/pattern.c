#include <stdio.h>
#include "pattern.h"
#include "debug.h"

int PATT_is_initalized = 0;

const char *C_codes[] = {
    "scanf", "\"%d\"", ",", "&", "*", "calloc", "(", ")", "sizeof", " "
};

_patternChanger patternIndex[NUMBER_OF_PATTERNS];

//////////////////////// 각 Java 패턴 ==> C 패턴 컨버전 정의 ////////////////////////
// 패턴 구조체 변환 필요 : 1) 스트릿하게 일치, 2) 자료형 등의 변환 가능 3) ~로 시작하는 문장 이렇게 세 개의 매칭 가능성을 시사해야 됨
void PATT_init()
{
    // stack = new int[stack_size]; 유형
    _patternChanger *pt0 = &patternIndex[0];
    pt0->pattern_type = EXACT;
    pt0->java_pattern_length = 8;
    pt0->c_pattern_length = 8;
    pt0->java_pattern[0] = IDENTFIER;
    pt0->java_pattern[1] = ASSIGN_OP;
    pt0->java_pattern[2] = IDENTFIER;
    pt0->java_pattern[3] = DOT_OP;
    pt0->java_pattern[4] = NEXTINT_CODE;
    pt0->java_pattern[5] = PARENTHESES_LEFT_OP;
    pt0->java_pattern[6] = PARENTHESES_RIGHT_OP;
    pt0->java_pattern[7] = SEMICOLON_OP;
    pt0->c_pattern[0] = 1000;
    pt0->c_pattern[1] = 5;
    pt0->c_pattern[2] = 1001;
    pt0->c_pattern[3] = 1002;
    pt0->c_pattern[4] = 1003;
    pt0->c_pattern[5] = 0;
    pt0->c_pattern[6] = 6;
    pt0->c_pattern[7] = 7;

    _patternChanger *pt1 = &patternIndex[1];
    pt1->pattern_type = EXACT;
    pt1->java_pattern_length = 8;
    pt1->c_pattern_length = 17;
    pt1->java_pattern[0] = IDENTFIER;
    pt1->java_pattern[1] = ASSIGN_OP;
    pt1->java_pattern[2] = NEW_CODE;
    pt1->java_pattern[3] = INT_CODE;
    pt1->java_pattern[4] = SQUARE_BRACKET_LEFT_OP;
    pt1->java_pattern[5] = IDENTFIER; // 혹은 수치 리터럴이 와도 좋다. 어떻게 처리할까.
    pt1->java_pattern[6] = SQUARE_BRACKET_RIGHT_OP;
    pt1->java_pattern[7] = SEMICOLON_OP;
    pt1->c_pattern[0] = 0; // stack
    pt1->c_pattern[1] = 1; // =
    pt1->c_pattern[2] = 1006; // (
    pt1->c_pattern[3] = 3; // int
    pt1->c_pattern[4] = 1009; // " "
    pt1->c_pattern[5] = 1004; // *
    pt1->c_pattern[6] = 1007; // )
    pt1->c_pattern[7] = 1005; // calloc
    pt1->c_pattern[8] = 1006; // (
    pt1->c_pattern[9] = 5; // STACK_SIZE
    pt1->c_pattern[10] = 1002; // ,
    pt1->c_pattern[11] = 1008; // sizeof
    pt1->c_pattern[12] = 1006; // (
    pt1->c_pattern[13] = 3; // int
    pt1->c_pattern[14] = 1007; // )
    pt1->c_pattern[15] = 1007; // )
    pt1->c_pattern[16] = 7; // ;

    _patternChanger *pt2 = &patternIndex[2];
    pt2->pattern_type = EXACT; // ** 사실 가변인자가 들어가므로 starts with + ends with 패턴으로 조합되어야 한다.
    pt2->java_pattern_length = 9;
    pt2->c_pattern_length = 5;
    pt2->java_pattern[0] = SYSTEM_CODE;
    pt2->java_pattern[1] = DOT_OP;
    pt2->java_pattern[2] = OUT_CODE;
    pt2->java_pattern[3] = DOT_OP;
    pt2->java_pattern[4] = PRINTF_CODE;
    pt2->java_pattern[5] = PARENTHESES_LEFT_OP;
    pt2->java_pattern[6] = STR_LIT;
    pt2->java_pattern[7] = PARENTHESES_RIGHT_OP;
    pt2->java_pattern[8] = SEMICOLON_OP;
    pt2->c_pattern[0] = 4; // printf
    pt2->c_pattern[1] = 5; // (
    pt2->c_pattern[2] = 6; // "--"
    pt2->c_pattern[3] = 7; // )
    pt2->c_pattern[4] = 8; // ;
}

int PATT_is_match(const _lexPattern *pattern)
{
    // 매칭시 해당 패턴 인덱스, 미 매칭시 -1
    // _patternChanger 는 내부 변수로 관리할 필요가 있음.
    // 추후에 세 번째 인자에 match type 결정하도록 수정
    for (int i = 0; i < NUMBER_OF_PATTERNS; ++i) {
        _patternChanger *patt = &patternIndex[i];
        int matched = 1;
        for (int j = 0; j < pattern->pattern_length; ++j) {
            if (pattern->pattern[j] != patt->java_pattern[j]) {matched = 0; break;}
        }
        if (matched) return i;
    }
    return -1;
}

void PATT_pattern_compile(const _lexPattern *pattern, char *resultbuf)
{
    if (PATT_is_initalized == 0) {
        PATT_is_initalized = 1;
        PATT_init();
    }

    memset(resultbuf, 0x00, MAX_RESULT_CODE_LENGTH); // 문자열 배열 비움

    int matchedIndex = PATT_is_match(pattern);

    if(matchedIndex >= 0) {
        _patternChanger *patt = &patternIndex[matchedIndex];
        for (int i = 0; i < patt->c_pattern_length; ++i) {
            if (patt->c_pattern[i] >= 1000) {
                strcat(resultbuf, C_codes[patt->c_pattern[i]-1000]);
            } else {
                strcat(resultbuf, pattern->buffer[patt->c_pattern[i]]);
            }
        }
    } else {
        // 그대로 붙여 출력
        for (int i = 0; i < pattern->pattern_length; ++i) {
            strcat(resultbuf, pattern->buffer[i]);
            // 다음 토큰이 수치리터럴, 아이덴티파이어, 연산자면 뛴다. 전위 후위, ; 연산자면 안뛴다. ) 가 뒤에 와도 안 뛴다. ','도 안 뛴다. '.'도 안 뛴다.
            // 특정 연산자 (.등) 뒤에는 안 뛴다.
            DBGMSG("패턴 : %s 패턴번호 %d",pattern->buffer[i], pattern->pattern[i]);
            if (
                pattern->pattern[i] != 100 &&
                pattern->pattern[i] != 103 &&
                i+1 < pattern->pattern_length &&
                (
                    pattern->pattern[i+1] == 10 ||
                    pattern->pattern[i+1] == 11 ||
                    pattern->pattern[i+1] == 2  || // 단순연산자
                    (
                        pattern->pattern[i+1] >= 100 &&
                        pattern->pattern[i+1] != 114 &&
                        pattern->pattern[i+1] != 115 &&
                        pattern->pattern[i+1] != 109 &&
                        pattern->pattern[i+1] != 103 &&
                        pattern->pattern[i+1] != 104 &&
                        pattern->pattern[i+1] != 119 &&
                        pattern->pattern[i+1] != 100
                    ) ||
                    (
                        pattern->pattern[i+1] >= 20 &&
                        pattern->pattern[i+1] <= 40
                    )
                )
            ) {
                strcat(resultbuf, " ");
            }
        }
    }
}