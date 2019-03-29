#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include "parse_tree.h"
#define NUMBER_OF_OPERATORS 29

int priority[NUMBER_OF_OPERATORS] = {0, 0, 8, 8, 7, 7, 6, 6, 2, 1, 0, 0, 0, 0, 0, 0, 10, 10, 10, 9, 9, 7, 7, 5, 4, 3, 0, 99, 99};
char operator[NUMBER_OF_OPERATORS][4] = {"<<=", ">>=","<<", ">>","<=", ">=",  "==", "!=", "&&", "||", "+=", "*=", "/=", "%=", "&=", "|=", "*", "/", "%", "+", "-",  "<",  ">", "&", "^", "|",  "=", "(", ")"};
bool operator_swapable[NUMBER_OF_OPERATORS] = {false, false, false, false, false, false, true, true, true, true, false, false, false, false, false, false, true, false, false, true, false, false, false, true, true, true, false, false, false};
char operator_onebyte[NUMBER_OF_OPERATORS] = {(char)1, (char)2, (char)3, (char)4, (char)5, (char)6, (char)7, (char)8, (char)9, (char)10, (char)11, (char)12, (char)13, (char)14, (char)15, (char)16, '*', '/', '%', '+', '-',  '<',  '>', '&', '^', '|', '=', '(', ')'};

typedef struct {
	char *stream;
	int number_of_tokens; // 토큰 갯수
	char *tokens[100]; // 각 토큰들의 시작 위치 포인터
	int tokens_length[100]; // 각 토큰들의 길이 (연산자는 무조건 1일 것이다..)
	bool is_operator[100]; // 토큰이 연산자인 경우에는 true, 아니면 false
} _container;

void normalize2(char *text)
{
	int i, j;
	for (i = 0, j = 0; text[i] != 0; i++,j++) {
		// 텍스트 정답비교 문제로 개행문자까지 무시해 보았다.
		//if (text[i] == '\n' || !isspace(text[i])) {
		if ( (text[i]>=1 && text[i]<=16) || !isspace(text[i])) {
			// lower normalize
			char t = text[i];
			if (t >= 'A' && t <= 'Z') t += 32;
			text[j] = t;
		}
		else
			j--;
	}
	text[j] = '\0';
}

void norm_onebyte_op(_container *container)
{
	// 에일리어싱
	char *text = container->stream;

	// 두 글자 이상의 연산자를 한 글자로 정규화한다. 입력 텍스트는 공백이 없다고 가정한다.
	for (int i = 0; i < 16; i++) {
		// 이 연산자 : operator[i] 를 찾아서 없을때까지 반복
		int op_len = strlen(operator[i]);
		char *pat_start;
		while ( (pat_start = strstr(text, operator[i])) != NULL) {
			*pat_start = operator_onebyte[i];
			for (int j = 1; j < op_len; j++) pat_start[j] = ' ';
		}
	}
	// 연산자를 정규화하느라 생긴 공백을 다시 정규화한다.
	normalize2(text);
	
	// 토큰을 분리한다. 초기 토큰 갯수는 0이라고 가정한다.
	container->number_of_tokens = 0;
	int len = strlen(text);
	int pat_start_offset = 0;
	int length_stack = 0;
	for (int i = 0; i < len; i++) {
		if (text[i] >= (char)1 && text[i] <= (char)16) {
			// 정규화된 연산자.
			if (length_stack > 0) {
				// 그 전의 비-연산자 집합을 처리한다.
				container->is_operator[container->number_of_tokens] = false;
				container->tokens[container->number_of_tokens] = text + pat_start_offset;
				container->tokens_length[container->number_of_tokens] = length_stack;
				pat_start_offset += length_stack;
				container->number_of_tokens++;
				length_stack = 0;
			}
			// 해당 연산자를 처리
			container->is_operator[container->number_of_tokens] = true;
			container->tokens[container->number_of_tokens] = text + i; // 해당 연산자의 위치
			container->tokens_length[container->number_of_tokens] = 1; // 연산자 길이는 1
			container->number_of_tokens++; // 토큰 증가
			pat_start_offset++; // 해당 연산자를 건너뛴 다음
			continue;
		}
		switch (text[i])
		{
			// 원래 1-byte 연산자.
			case '*':case '/':case '%':case '+':case '-':case '<':case '>':case '&':case '^':case '|':case '=': case '(': case ')':
			if (length_stack > 0) {
				container->is_operator[container->number_of_tokens] = false;
				container->tokens[container->number_of_tokens] = text + pat_start_offset;
				printf("오프셋 이동 : %d\n",  pat_start_offset); // 예상 6
				container->tokens_length[container->number_of_tokens] = length_stack;
				pat_start_offset += length_stack;
				container->number_of_tokens++;
				length_stack = 0;
			}
			// 해당 연산자처리
			container->is_operator[container->number_of_tokens] = true;
			container->tokens[container->number_of_tokens] = text + i; // 해당 연산자의 위치
			container->tokens_length[container->number_of_tokens] = 1; // 연산자 길이는 1
			container->number_of_tokens++; // 토큰 증가
			pat_start_offset++; // 해당 연산자를 건너뛴 다음
			continue;
			break;
		}
		// 일반 문자열
		++length_stack; //랭스택을 열심히 쌓는다.
	}
	// 남아있는 랭스택 처리
	if (length_stack > 0) {
		container->tokens[container->number_of_tokens] = text + pat_start_offset;
		container->tokens_length[container->number_of_tokens] = length_stack;
		container->number_of_tokens++;
	}
}

void print_opop(const char *text)
{
	// 토큰 분리 시 괄호도 잘라 준다.
	int len = strlen(text);
	// op를 적절히 변환하여 한 줄씩 출력 1~16은 매핑된것이다.
	for (int i = 0; i < len; i++) {
		if (text[i] >= (char)1 && text[i] <= (char)16) {
			printf("[%s(%d)]", operator[text[i]-1], (int)operator_onebyte[text[i]-1]);
			continue;
		}
		switch (text[i])
		{
			case '*':case '/':case '%':case '+':case '-':case '<':case '>':case '&':case '^':case '|':case '=': case '(': case ')':
			putchar('[');
			putchar(text[i]);
			putchar(']');
			continue;
			break;
		}
		printf("%c", (char)text[i]);
	}
	putchar('\n');
}

typedef struct {
	int top;
	char stack_data[301]; // 여기에 연산자 또는 괄호를 담는다.
} stack;

void st_push(stack *stk, char data)
{
	stk->stack_data[++stk->top] = data;
}

bool st_empty(stack *stk)
{
	if (stk->top < 0) return true;
	return false;
}

int st_size(stack *stk)
{
	return stk->top + 1;
}

char st_top(stack *stk)
{
	if (st_empty(stk)) return 0;
	return stk->stack_data[stk->top];
}

char st_pop(stack *stk)
{
	if (st_empty(stk)) return 0;
	char ret = stk->stack_data[stk->top--];
	return ret;
}

int get_priority(char op)
{
	for (int i = 0; i < NUMBER_OF_OPERATORS; i++) {
		if (operator_onebyte[i] == op) return priority[i];
	}
	return -1;
}



int main(void)
{
	_container container;
	stack stk;
	stk.top = -1;
	char buf[] = "(stop>signal1)&&(stop<signal2)"; // raw Input
	char result[500]; // 후위표현식으로 전환된 문자열
	container.stream = buf;
	printf("%s\n", container.stream);
	// 공백 제거, 연산자 1-byte정규화, 토큰분리
	norm_onebyte_op(&container);

	// 후위식으로 전환
	int len = strlen(buf);
	for (int i = 0; i < len; i++) {

	}

	// 테스트용 출력
	print_opop(buf);

	// 테스트용 출력 2
	printf("컨테이너에 분리된 토큰 개수 : %d\n", container.number_of_tokens);
	for (int i = 0; i < container.number_of_tokens; i++) {
		// printf("%d: [%ld, %d] ", container.is_operator[i], container.tokens[i] - container.stream, container.tokens_length[i]);
		
	}
	return 0;
}
