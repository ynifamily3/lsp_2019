#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#define NUMBER_OF_OPERATORS 31


int priority[NUMBER_OF_OPERATORS] = {0, 0, 8, 8, 7, 7, 6, 6, 2, 1, 0, 0, 0, 0, 0, 0, 99, 10, 10, 10, 9, 9, 7, 7, 5, 4, 3, 0, 99, 99, -1};
char operator[NUMBER_OF_OPERATORS][4] = {"<<=", ">>=","<<", ">>","<=", ">=",  "==", "!=", "&&", "||", "+=", "*=", "/=", "%=", "&=", "|=", "->", "*", "/", "%", "+", "-",  "<",  ">", "&", "^", "|",  "=", "(", ")", ","};
bool operator_swapable[NUMBER_OF_OPERATORS] = {false, false, false, false, false, false, true, true, true, true, false, false, false, false, false, false, false, true, false, false, true, false, false, false, true, true, true, false, false, false, false};
int operator_convable[NUMBER_OF_OPERATORS] = {-1,-1,-1,-1,-1,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 22,-1,-1,-1,-1,-1,-1,-1};
char operator_onebyte[NUMBER_OF_OPERATORS] = {(char)1, (char)2, (char)3, (char)4, (char)5, (char)6, (char)7, (char)8, (char)9, (char)10, (char)11, (char)12, (char)13, (char)14, (char)15, (char)16, (char)17, '*', '/', '%', '+', '-',  '<',  '>', '&', '^', '|', '=', '(', ')', ','};


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
		if ( (text[i]>=1 && text[i]<=17) || !isspace(text[i])) {
			char t = text[i];
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
	for (int i = 0; i < 17; i++) {
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
	bool is_previous_op = false;
	for (int i = 0; i < len; i++) {
		if (text[i] >= (char)1 && text[i] <= (char)17) {
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
			is_previous_op = true;
			continue;
		}
		switch (text[i])
		{
			// 단항 연산자의 예외를 처리한다.
			case '-':case'+':case'&':case'*':
			// printf("단항인가요? : %c\n", text[i]);
			if (i == 0 || text[i-1] == '(' || is_previous_op) {
				goto normal_string;
			}
			// printf("%c는 단항이 아닌거 같아요.\n", text[i]);
			break;
		}
		switch (text[i])
		{
			// 원래 1-byte 연산자.

			case '*':case '/':case '%':case '+':case '-':case '<':case '>':case '&':case '^':case '|':case '=': case '(': case ')': case ',':
			if (length_stack > 0) {
				container->is_operator[container->number_of_tokens] = false;
				container->tokens[container->number_of_tokens] = text + pat_start_offset;
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
			is_previous_op = true;
			continue;
			break;
		}
		normal_string:
		// 일반 문자열
		++length_stack; // 랭스택을 열심히 쌓는다.
		is_previous_op = false; // 직전에 연산자가 안옴
	}
	// 남아있는 랭스택 처리
	if (length_stack > 0) {
		container->tokens[container->number_of_tokens] = text + pat_start_offset;
		container->tokens_length[container->number_of_tokens] = length_stack;
		container->number_of_tokens++;
	}
}

typedef struct {
	int top;
	char *stack_data[301]; // 스트링들을 담는다.
} stackString;

void st2_push(stackString *stk, char* data)
{
	stk->stack_data[++stk->top] = data;
}

bool st2_empty(stackString *stk)
{
	if (stk->top < 0) return true;
	return false;
}

int st2_size(stackString *stk)
{
	return stk->top + 1;
}

char* st2_top(stackString *stk)
{
	if (st2_empty(stk)) return 0;
	return stk->stack_data[stk->top];
}

char* st2_pop(stackString *stk)
{
	if (st2_empty(stk)) return 0;
	char* ret = stk->stack_data[stk->top--];
	return ret;
}

int get_priority(char op)
{
	for (int i = 0; i < NUMBER_OF_OPERATORS; i++) {
		if (operator_onebyte[i] == op) return priority[i];
	}
	return -1;
}

char *make_string(char *text, int len)
{
	char *alloc = calloc(len+4, sizeof(char));
	if (text[0] >= 1 && text[0] <= 17) {
		// onebyte 연산자 처리
		//operator[text[0]-1]
		strcpy(alloc, operator[text[0]-1]);
		return alloc;
	}
	for (int i = 0; i < len; i++) {
		alloc[i] = text[i];
	}
	return alloc;
}

int get_priority_str(char *opstring) {
	for (int i = 0; i < NUMBER_OF_OPERATORS; i++) {
		if (strcmp(opstring, operator[i]) == 0) return priority[i];
	}
	return -1;
}

// 문자열이 operation인 경우 해당 operation의 위치를 알려주고 operation이 아닌 경우 -1을 리턴한다.
int is_operation_str(char *opstring) {
	for (int i = 0; i < NUMBER_OF_OPERATORS; i++) {
		if (strcmp(opstring, operator[i]) == 0) return i;
	}
	return -1;
}

int main(int argc, char *argv[])
{
	_container container;
	char *buf = argv[1];
	container.stream = buf;
	// 공백 제거, 연산자 1-byte정규화, 토큰분리
	norm_onebyte_op(&container);
	int len = container.number_of_tokens;
	stackString stk2;
	stk2.top = -1;
	char **tokenElem = (char **)calloc(len, sizeof(char *));
	char *tokenOP = (char *)calloc(len, sizeof(char)); // op인 경우 해당 one-byte operation, 아닌 경우 -1
	char **newTokenElem = (char **)calloc(len, sizeof(char *));// 후위로 된걸 연산하는 buffer
	int chasePtr = 0;
	for (int i = 0; i < len; i++) {
		//tokenElem[i] = (char *)calloc(container.tokens_length[i], sizeof(char) );
		tokenElem[i] = make_string(container.tokens[i], container.tokens_length[i]);
		printf("[%s] ", tokenElem[i]);
	}
	putchar('\n');

	// 후위 표기식으로 전환
	for (int i = 0; i < len; i++) {
		if (tokenElem[i][0] == '(')
			st2_push(&stk2, tokenElem[i]);
		else if (tokenElem[i][0] == ')') {
			while (!st2_empty(&stk2)) {
				char *test = st2_pop(&stk2);
				if (test[0] != '(') {
					newTokenElem[chasePtr++] = test;
				} else { 
					// printf("여는괄호 버려");
					break; 
				}
			}
		}
		else if (container.is_operator[i]) {
			// op 인 경우
			while (!st2_empty(&stk2) && (get_priority_str( st2_top(&stk2) ) >= get_priority(container.tokens[i][0] ) )) {
				char *test = st2_pop(&stk2);
				if (test[0] != '(')
					newTokenElem[chasePtr++] = test;
			}
			// 자신을 푸시
			st2_push(&stk2, tokenElem[i]);
		}
		else {
			// printf("낫 연산자 : %s\n", tokenElem[i]);
			// char *test = st2_pop(&stk2);
			newTokenElem[chasePtr++] = tokenElem[i];
		}
	}
	while (!st2_empty(&stk2)) {
		char *test = st2_pop(&stk2);
		newTokenElem[chasePtr++] = test;
	}

	for (int i = 0; i < chasePtr; i++) {
		if ( is_operation_str(newTokenElem[i]) != -1 )
			printf("[%s] ", newTokenElem[i]);
		else
			printf("[%s] ", newTokenElem[i]);
	}
	putchar('\n');

	// 연산하고 스택에 넣는...
	/*
		1. 숫자를 만나면 숫자는 스택에 푸시한다.
		2. 연산자를 만나면 스택에서 팝을 두 번하여 그 두 데이터를 가지고 연산한 다음 그 결과를 스택에 다시 푸시한다.
	*/
	for (int i = 0; i < chasePtr; i++) {
		char op = is_operation_str(newTokenElem[i]);
		if (op == -1) {
			// 연산자 아님
			st2_push(&stk2, newTokenElem[i]);
		} else {
			// 연산자임
			char *curr = newTokenElem[i];
			char *pop1 = st2_pop(&stk2);
			char *pop2 = st2_pop(&stk2);
			
			// 유저는 어떤 수식을 넣을지 모르기 때문에 두 개가 팝되리라는 보장이 없다.
			if (pop1 && pop2) {
				char *newElem = (char *)calloc( strlen(pop1) + strlen(pop2) + strlen(curr) + 1, sizeof(char) );
				if (operator_convable[op] != -1) {
					strcat(newElem, pop1);
					strcat(newElem, pop2);
					strcat(newElem, operator[operator_convable[op]]);
				} else if (operator_swapable[op]) {
					//printf("%s 스왑어블\n", newTokenElem[i]);
					if (strcmp (pop1, pop2) < 0 ) {
						strcat(newElem, pop2);
						strcat(newElem, pop1);
					} else {
						strcat(newElem, pop1);
						strcat(newElem, pop2);
					}
						strcat(newElem, curr);
				} else {
					strcat(newElem, pop2);
					strcat(newElem, pop1);
					strcat(newElem, curr);
				}
				st2_push(&stk2, newElem);
				free(curr);
				free(pop1);
				free(pop2);
				curr = NULL;
				pop1 = NULL;
				pop2 = NULL;
			} else {
				char *newElem;
				if (pop1) {
					newElem = (char *)calloc( strlen(pop1) + strlen(curr) + 1, sizeof(char) );
					strcat(newElem, pop1);
					strcat(newElem, curr);
					st2_push(&stk2, newElem);
					free(curr);
					free(pop1);
					curr = NULL;
					pop1 = NULL;
				} else if (pop2) {
					newElem = (char *)calloc( strlen(pop2) + strlen(curr) + 1, sizeof(char) );
					strcat(newElem, pop2);
					strcat(newElem, curr);
					st2_push(&stk2, newElem);
					free(curr);
					free(pop2);
					curr = NULL;
					pop2 = NULL;
				} else {
					newElem = (char *)calloc(strlen(curr) + 1, sizeof(char) );
					strcat(newElem, curr);
					st2_push(&stk2, newElem);
					free(curr);
					curr = NULL;
				}
			}
		}
	}
	printf("최종 식 : %s", st2_pop(&stk2));
	char *t;
	while(t = st2_pop(&stk2)) {
		printf("%s", t);
	}
	printf("\n");

	// 메모리 free 해야됨 
	return 0;
}
