#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "pattern.h"

int main(void)
{
	_lexPattern pattern;
	char input[100] = "stack = new int[STACK_SIZE];";
	lex(&pattern, input);
	printf("패턴 : ");
	for (int i = 0; i < pattern.pattern_length; i++) {
		printf("%d ", pattern.pattern[i]);
	}
	printf("\n자바 코드 : ");
	for (int i = 0; i < pattern.pattern_length ; i++) {
		printf("%s", pattern.buffer[i]);
	}

	_patternChanger *patternIndex[10]; // java to c 컨버전 규칙 개수


	_patternChanger patt; // 이 중에 한 패턴 정의해봄.
	_patternChanger patt2;


	patternIndex[0] = &patt; // 리스트에 넣음.
	patternIndex[1] = &patt2;

	// 여기서부터 패턴정의
    patt.java_pattern_length = 8;
    patt.c_pattern_length = 8;
    patt.java_pattern[0] = IDENTFIER;
    patt.java_pattern[1] = ASSIGN_OP;
    patt.java_pattern[2] = IDENTFIER;
    patt.java_pattern[3] = DOT_OP;
    patt.java_pattern[4] = NEXTINT_CODE;
    patt.java_pattern[5] = PARENTHESES_LEFT_OP;
    patt.java_pattern[6] = PARENTHESES_RIGHT_OP;
    patt.java_pattern[7] = SEMICOLON_OP;
    // c pattern : ~999 ==> pass (java_pattern과 동일)
    // c pattern 1000 ~ ==> 변환되는 C식으로 매쳐
    patt.c_pattern[0] = 1000;
    patt.c_pattern[1] = 5;
    patt.c_pattern[2] = 1001;
    patt.c_pattern[3] = 1002;
    patt.c_pattern[4] = 1003;
    patt.c_pattern[5] = 0;
    patt.c_pattern[6] = 6;
    patt.c_pattern[7] = 7;
	
	memcpy(patt.java_pattern_buffer, pattern.buffer, sizeof(pattern.buffer));
	// end of pattern

	// 여기서부터 패턴정의2
	// stack = new int[STACK_SIZE]; ==> stack = (int *)calloc(STACK_SIZE, sizeof(int));
	// 패턴 구조체 변환 필요 : 1) 스트릿하게 일치, 2) 자료형 등의 변환 가능 3) ~로 시작하는 문장 이렇게 세 개의 매칭 가능성을 시사해야 됨
    patt2.java_pattern_length = 8;
    patt2.c_pattern_length = 17;
    patt2.java_pattern[0] = IDENTFIER;
    patt2.java_pattern[1] = ASSIGN_OP;
    patt2.java_pattern[2] = NEW_CODE;
    patt2.java_pattern[3] = INT_CODE;
    patt2.java_pattern[4] = SQUARE_BRACKET_LEFT_OP;
    patt2.java_pattern[5] = IDENTFIER;
    patt2.java_pattern[6] = SQUARE_BRACKET_RIGHT_OP;
    patt2.java_pattern[7] = SEMICOLON_OP;
    // c2 pattern : ~999 ==> pass (java_pattern과 동일)
    // c2 pattern 1000 ~ ==> 변환되는 C식으로 매쳐
    patt2.c_pattern[0] = 0; // stack
    patt2.c_pattern[1] = 1; // =
	patt2.c_pattern[2] = 1006; // (
	patt2.c_pattern[3] = 3; // int
	patt2.c_pattern[4] = 1009; // " "
    patt2.c_pattern[5] = 1004; // *
	patt2.c_pattern[6] = 1007; // )
    patt2.c_pattern[7] = 1005; // calloc
    patt2.c_pattern[8] = 1006; // (
    patt2.c_pattern[9] = 5; // STACK_SIZE
    patt2.c_pattern[10] = 1002; // ,
    patt2.c_pattern[11] = 1008; // sizeof
	patt2.c_pattern[12] = 1006; // (
	patt2.c_pattern[13] = 3; // int
	patt2.c_pattern[14] = 1007; // )
	patt2.c_pattern[15] = 1007; // )
	patt2.c_pattern[16] = 7; // ;
	memcpy(patt2.java_pattern_buffer, pattern.buffer, sizeof(pattern.buffer));
	// end of pattern2
	

	int a;
	a = is_match(2, patternIndex, &pattern);

	char result[100];
	convertJavatoC(patternIndex[a], result);
	printf(" ==> C언어 코드로 변환시 : %s\n", result);

	exit(0);
}

