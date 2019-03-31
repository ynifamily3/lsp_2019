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
	char *tokens[1024]; // 각 토큰들의 시작 위치 포인터
	int tokens_length[1024]; // 각 토큰들의 길이 (연산자는 무조건 1일 것이다..)
	bool is_operator[1024]; // 토큰이 연산자인 경우에는 true, 아니면 false
} _container;

typedef struct {
	char elem[1024];
	bool is_operator;
	//bool is_pure_swapable;
} _token;

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
			// 함수 호출문을 처리한다.
			case '(':
			break;
		}
		switch (text[i])
		{
			// 단항 연산자의 예외를 처리한다.
			case '-':case'+':case'&':case'*':
			if (i == 0 || text[i-1] == '(' || is_previous_op) {
				goto normal_string;
			}
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
		container->is_operator[container->number_of_tokens] = false; // 명시적으로 안 써줘서 에러 났다
		container->tokens[container->number_of_tokens] = text + pat_start_offset;
		container->tokens_length[container->number_of_tokens] = length_stack;
		container->number_of_tokens++;
	}
}

typedef struct {
	int top;
	_token *stack_data[301]; // 토큰들을 담는다...
} stackString;

void st2_push(stackString *stk, _token* data)
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

_token* st2_top(stackString *stk)
{
	if (st2_empty(stk)) return 0;
	return stk->stack_data[stk->top];
}

_token* st2_pop(stackString *stk)
{
	if (st2_empty(stk)) return 0;
	_token* ret = stk->stack_data[stk->top--];
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

typedef struct {
	char **tokens;
	bool is_operator[1024];
	int length;
} _tokens;

// 입력 : 문자열 , 출력 : 토큰들의 집합, 토큰들 갯수
_tokens token_seperation(_container *container, char *buf) {
	
	_tokens tokens;
	container->stream = buf;
	// 공백 제거, 연산자 1-byte정규화, 토큰분리
	norm_onebyte_op(container);
	int len = container->number_of_tokens;
	char **tokenElem = (char **)calloc(len, sizeof(char *));
	// _token **newTokenElem = (_token **)calloc(len, sizeof(_token *));// 후위로 된걸 연산하는 buffer
	// int chasePtr = 0;
	for (int i = 0; i < len; i++) {
		tokens.is_operator[i] = container->is_operator[i];
		tokenElem[i] = make_string(container->tokens[i], container->tokens_length[i]);
	}
	tokens.length = len;
	tokens.tokens = tokenElem;
	return tokens;
}

// 입력 : 들어온 토큰들, 출력 : 포스트픽스로 정렬된 토콘, 그 토큰 수
_token **postfix_expression(_tokens *tokens, int *ret_len) {
	stackString stk;
	stk.top = -1;
	int len = tokens->length;
	_token **newTokenElem = (_token **)calloc(tokens->length, sizeof(_token *));// 후위로 된걸 연산하는 buffer
	int chasePtr = 0;
	for (int i = 0; i < len; i++) {
		if (tokens->tokens[i][0] == '(') {
			_token *a = (_token *)calloc(1, sizeof(_token));
			strncpy(a->elem, tokens->tokens[i], 100);
			a->is_operator = true;
			//a->is_pure_swapable = false;
			st2_push(&stk, a);
		}
		else if (tokens->tokens[i][0] == ')') {
			while (!st2_empty(&stk)) {
				_token *a = st2_pop(&stk);
				if (a->elem[0] != '(') {
					newTokenElem[chasePtr++] = a;
				}
				else {
					free(a);
					break;
				}
			}
		}
		else if (tokens->is_operator[i]) {
			while (!st2_empty(&stk) && (get_priority_str( st2_top(&stk)->elem ) >= get_priority_str(tokens->tokens[i]))) {
				_token *a = st2_pop(&stk);
				if (a->elem[0] != '(')
					newTokenElem[chasePtr++] = a;
				else
					free(a);
			}
			_token *a = (_token *)calloc(1, sizeof(_token));
			strncpy(a->elem, tokens->tokens[i], 100);
			a->is_operator = true;
			//a->is_pure_swapable = operator_swapable[is_operation_str(tokens->tokens[i])];
			st2_push(&stk, a);
		}
		else {
			_token *a = (_token *)calloc(1, sizeof(_token));
			strncpy(a->elem, tokens->tokens[i], 100);
			a->is_operator = false;
			//a->is_pure_swapable = false;
			newTokenElem[chasePtr++] = a;
		}
	}
	while (!st2_empty(&stk)) {
		_token *a = st2_pop(&stk);
		newTokenElem[chasePtr++] = a;
	}
	*ret_len = chasePtr;
	return newTokenElem;
}

int p_str_compare(const void* a, const void* b)
{
    if (*(char*)a > *(char*)b)
        return 1;
    else if (*(char*)a < *(char*)b)
        return -1;
    else
        return 0;
}

// 입력 : _token ** newTokenElem 의 토큰 데이터, 토큰 수 , 출력 : 완성된 char *
char *postfix_regulization(_token **newTokenElem, int len) {
	stackString stk2;
	stk2.top = -1;
	for (int i = 0; i < len; i++) {
		int op = is_operation_str(newTokenElem[i]->elem);
		if (op == -1) {
			// 연산자 아님
			st2_push(&stk2, newTokenElem[i]);
		} else {
			// 연산자임
			_token *curr = newTokenElem[i];
			_token *pop1 = st2_pop(&stk2);
			_token *pop2 = st2_pop(&stk2);
			// 유저는 어떤 수식을 넣을지 모르기 때문에 두 개가 팝되리라는 보장이 없다.
			if (pop1 && pop2) {
				_token *newElem = (_token *)calloc(1, sizeof(_token));
				if (operator_convable[op] != -1) {
					// 컨버저블
					newElem->is_operator = false;
					//newElem->is_pure_swapable = false;
					strcat(newElem->elem, pop1->elem);
					strcat(newElem->elem, pop2->elem);
					strcat(newElem->elem, operator[operator_convable[op]]);
				} else if (operator_swapable[op]) {
					newElem->is_operator = false;
					if (strcmp (pop1->elem, pop2->elem) < 0 ) {
						strcat(newElem->elem, pop2->elem);
						strcat(newElem->elem, pop1->elem);
					} else {
						strcat(newElem->elem, pop1->elem);
						strcat(newElem->elem, pop2->elem);
					}
						strcat(newElem->elem, curr->elem);
					// 피연산자 둘다 스왑어블이면 둘을 섞는다.
					/*if (pop1->is_pure_swapable && pop2->is_pure_swapable) {
						newElem->is_pure_swapable = true;
						// 섞어준다.
						printf("쉐킷");
						qsort(newElem->elem, strlen(newElem->elem), sizeof(char), p_str_compare);
					}*/
				} else {
					newElem->is_operator = false;
					//newElem->is_pure_swapable = false;
					strcat(newElem->elem, pop2->elem);
					strcat(newElem->elem, pop1->elem);
					strcat(newElem->elem, curr->elem);
				}
				st2_push(&stk2, newElem);
			} else {
				_token *newElem = (_token *)calloc(1, sizeof(_token));
				newElem->is_operator = false;
				//newElem->is_pure_swapable = false;
				if (pop1) {
					strcat(newElem->elem, pop1->elem);
					strcat(newElem->elem, curr->elem);
					st2_push(&stk2, newElem);
				} else if (pop2) {
					strcat(newElem->elem, pop2->elem);
					strcat(newElem->elem, curr->elem);
					st2_push(&stk2, newElem);
				} else {
					_token *newElem = (_token *)calloc(1, sizeof(_token));
					newElem->is_operator = false;
					//newElem->is_pure_swapable = false;
					strcat(newElem->elem, curr->elem);
					st2_push(&stk2, newElem);
				}
			}
		}
	}
	char *ret = (char *)calloc(1024, sizeof(char));
	char *t;
	strcat(ret, st2_pop(&stk2)->elem);
	while(t = st2_pop(&stk2)->elem) {
		strcat(ret, t);
	}
	return ret;
}

// 이걸 하나의 함수로 만들고 재귀적으로 부르면 괄호 / 함수 문제 해결할 수 있을 것 같다.

int main(int argc, char *argv[])
{
	_container container; // 토큰 분리 전
	_tokens tokens; // 토큰 분리하고 나서 후위 전 // 안쪽에 char ** 있음
	_token **newTokenElem; // 후위 연산 후
	char *result;
	char buf[100];
	strcpy(buf, argv[1]);
	tokens = token_seperation(&container, buf);
	int len;
	newTokenElem = postfix_expression(&tokens, &len);
	result = postfix_regulization(newTokenElem, len);
	printf("%s\n", result);
	
	// 메모리 해제
	for (int i = 0; i < tokens.length; i++) {
		free(tokens.tokens[i]);
	}
	for (int i = 0; i < len; i++) {
		free(newTokenElem[i]);
	}
	free(tokens.tokens);
	free(newTokenElem);
	return 0;
}
