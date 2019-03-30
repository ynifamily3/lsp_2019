#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include "parse_tree.h"
#define NUMBER_OF_OPERATORS 31

int priority[NUMBER_OF_OPERATORS] = {0, 0, 8, 8, 7, 7, 6, 6, 2, 1, 0, 0, 0, 0, 0, 0, 99, 10, 10, 10, 9, 9, 7, 7, 5, 4, 3, 0, 99, 99, -1};
char operator[NUMBER_OF_OPERATORS][4] = {"<<=", ">>=","<<", ">>","<=", ">=",  "==", "!=", "&&", "||", "+=", "*=", "/=", "%=", "&=", "|=", "->", "*", "/", "%", "+", "-",  "<",  ">", "&", "^", "|",  "=", "(", ")", ","};
bool operator_swapable[NUMBER_OF_OPERATORS] = {false, false, false, false, false, false, true, true, true, true, false, false, false, false, false, false, false, true, false, false, true, false, false, false, true, true, true, false, false, false, false};
int operator_convable[NUMBER_OF_OPERATORS] = {-1,-1,-1,-1,-1,5,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, (int)'<',-1,-1,-1,-1,-1,-1,-1};
char operator_onebyte[NUMBER_OF_OPERATORS] = {(char)1, (char)2, (char)3, (char)4, (char)5, (char)6, (char)7, (char)8, (char)9, (char)10, (char)11, (char)12, (char)13, (char)14, (char)15, (char)16, (char)17, '*', '/', '%', '+', '-',  '<',  '>', '&', '^', '|', '=', '(', ')', ','};

typedef struct {
	char *stream;
	int number_of_tokens; // 토큰 갯수
	char *tokens[100]; // 각 토큰들의 시작 위치 포인터
	int tokens_length[100]; // 각 토큰들의 길이 (연산자는 무조건 1일 것이다..)
	bool is_operator[100]; // 토큰이 연산자인 경우에는 true, 아니면 false
} _container;

typedef struct {
	int number_of_tokens; // 토큰 수 (1개로 줄이자!)
	char *tokens[100]; // 토큰들의 포인터 
	bool is_operator[100]; //연산자 여부(0또는 1) 
} _final_container;

void print_op(char op_oneb) {
	if (op_oneb >= 1 && op_oneb <= 17) {
		printf("%s", operator[op_oneb-1]);
	} else {
		putchar(op_oneb);
	}
}

int is_convertable_op(char op_oneb)
{
	for (int i = 0; i < NUMBER_OF_OPERATORS; i++) {
		if (op_oneb == operator_onebyte[i]) {
			if (operator_convable[i] != -1) return operator_convable[i];
			else return -1;
		}
	}
	return false;
}

bool is_swapable_op(char op_oneb)
{
	for (int i = 0; i < NUMBER_OF_OPERATORS; i++) {
		if (op_oneb == operator_onebyte[i]) {
			return operator_swapable[i];
		}
	}
	return false;
}

void normalize2(char *text)
{
	int i, j;
	for (i = 0, j = 0; text[i] != 0; i++,j++) {
		// 텍스트 정답비교 문제로 개행문자까지 무시해 보았다.
		//if (text[i] == '\n' || !isspace(text[i])) {
		if ( (text[i]>=1 && text[i]<=17) || !isspace(text[i])) {
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

void print_opop(const char *text)
{
	// 토큰 분리 시 괄호도 잘라 준다.
	int len = strlen(text);
	// op를 적절히 변환하여 한 줄씩 출력 1~16은 매핑된것이다.
	for (int i = 0; i < len; i++) {
		if (text[i] >= (char)1 && text[i] <= (char)17) {
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

void print_token(_container *container, int index)
{
	for (int j = 0; j < container->tokens_length[index]; j++) {
		char t = *(container->tokens[index] + j);
		if (t >= 1 && t <= 16) {
			printf("%s", operator[t-1]);
		} else 
			putchar( *(container->tokens[index] + j) );
	}
}

char *make_string(char *text, int len)
{
	char *alloc = calloc(len+1, sizeof(char));
	for (int i = 0; i < len; i++) {
		alloc[i] = text[i];
	}
	return alloc;
}

int main(void)
{
	_container container;
	stack stk;
	stk.top = -1;
	char buf[] = "-1==open(fd,O_CREAT|O_RDONLY)="; // raw Input
	char result[500]; // 후위표현식으로 전환된 문자열
	container.stream = buf;
	printf("%s\n", container.stream);
	// 공백 제거, 연산자 1-byte정규화, 토큰분리
	norm_onebyte_op(&container);


	// 후위식으로 전환	
	// 후위로 된 것들을 모아놓는 무언가 컨테이너
	int len = container.number_of_tokens;
	_final_container FC;
	FC.number_of_tokens = 0;
	for (int i = 0; i < len; i++) {
		if (!container.is_operator[i]) {
			putchar('[');
			FC.tokens[FC.number_of_tokens] =  make_string(container.tokens[i], container.tokens_length[i]);
			FC.is_operator[FC.number_of_tokens] = 0;
			FC.number_of_tokens++;
			// 디버깅프린트
			print_token(&container, i);
			putchar(']');
		} else {
			char op = container.tokens[i][0];
			switch(op)
			{
				case '(':
					st_push(&stk, op);
				break;
				// case 우선순위 높은 것 --> 낮은 것 --> 닫는 괄호
				case ')':
					while (!st_empty(&stk) && st_top(&stk) != '(')
                    {
						char c = st_top(&stk);
						FC.tokens[FC.number_of_tokens] =  make_string(&c, 1);
						FC.is_operator[FC.number_of_tokens] = 1;
						FC.number_of_tokens++;
						putchar('[');
                        print_op(c);
						putchar(']');
                        st_pop(&stk);
                    }
                    st_pop(&stk);
                    break;
				break;
				default:
					//    > or >=?
					while (!st_empty(&stk) && (get_priority(st_top(&stk)) > get_priority(op) ))
					{
						if (st_top(&stk) != '(') {
						char c = st_top(&stk);
						FC.tokens[FC.number_of_tokens] =  make_string(&c, 1);
						FC.is_operator[FC.number_of_tokens] = 1;
						FC.number_of_tokens++;
							putchar('[');
                        	print_op(c);
							putchar(']');
						}
						st_pop(&stk);
                    }
                    st_push(&stk, op);
				break;
			}
		}
	}
	// 스택에 남아있는것을 모두 뺀다.
	while (!st_empty(&stk)) {
		if (st_top(&stk) != '(') {
			char c = st_top(&stk);
			FC.tokens[FC.number_of_tokens] =  make_string(&c, 1);
			FC.is_operator[FC.number_of_tokens] = 1;
			FC.number_of_tokens++;	
			putchar('[');
            print_op(c);
			putchar(']');
		}
		st_pop(&stk);
	}
	// FC 테스트
	// FC 에 있는 것들을 살펴보면서 연산자가 나올 때까지 대기,
	// 연산자가 나오면 앞 2개를 바인딩한다.
	// 연산자의 swapable/convertable을 따져 적절히 순서를 바꾸고 연산자를 바꾼다.
	// 그 3개(보다 적을 수도 있는)를 압축한다.
	// 토큰 사이즈를 줄인다.
	// 2개가 아니거나 조금 이상하더라도 어쨌든 바인딩한다.
	// 배열 사이즈를 줄일 때 start지점을 조절한다.
	int origin_size = FC.number_of_tokens;
	int prev_ptr = 0; // 마지막으로 합친 곳
	printf("토큰 : %d개\n", FC.number_of_tokens);
	//while (FC.number_of_tokens >= 2) {
		for (int i = 0; i < origin_size; i++) {
			if (FC.is_operator[i]) {
				//printf("연산자임 : ");
				//printf("[%s] [%s]", FC.tokens[i-1], FC.tokens[i-2]);
				//print_op(FC.tokens[i][0]);
				//printf("prev ptr : %d\n", prev_ptr);
				//printf("i-1 : %d i-2 : %d", i-1, i-2);
				//printf("%d에서 연산자 발견\n", i);
				if (i-1 >= 0 && i-2 >= 0) {
					//printf("앞 2개의 엘리먼트 존재 : [%s] [%s]\n", FC.tokens[i - 1], FC.tokens[i - 2]);
					// 앞 2개가 다 있으면 ()
					// 연산자의 swapable을 조회
					if (is_swapable_op(FC.tokens[i][0])) {
						// 스왑어블 하다면 앞 두개를 정렬함.(정규화)
						if (strcmp(FC.tokens[i - 1], FC.tokens[i - 2]) > 0) {
							// i-1 i-2 i 순서로 만듦
							char *new_token = calloc(strlen(FC.tokens[i - 1]) + strlen(FC.tokens[i - 2]) + strlen(FC.tokens[i]) + 1, sizeof(char));
							strcat(new_token, FC.tokens[i - 1]);
							strcat(new_token, FC.tokens[i - 2]);
							strcat(new_token, FC.tokens[i]);
							//free(FC.tokens[i - 1]);
							//free(FC.tokens[i - 2]);
							//free(FC.tokens[i]);
							FC.tokens[i] = new_token;
						} else {
							// i-2 i-1 i 순서로 만듦
							char *new_token = calloc(strlen(FC.tokens[i - 2]) + strlen(FC.tokens[i - 1]) + strlen(FC.tokens[i]) + 1, sizeof(char));
							strcat(new_token, FC.tokens[i - 2]);
							strcat(new_token, FC.tokens[i - 1]);
							strcat(new_token, FC.tokens[i]);
							//free(FC.tokens[i - 2]);
							//free(FC.tokens[i - 1]);
							//free(FC.tokens[i]);
							FC.tokens[i] = new_token;
						}
					} else if (is_convertable_op(FC.tokens[i][0]) != -1) {
						// 컨버터블 하다면 op 의 방향을 전환
						FC.tokens[i][0] = is_convertable_op(FC.tokens[i][0]);
						// 앞 두개의 순서를 바꿔 하나로 병합
						char *new_token = calloc(strlen(FC.tokens[i - 2]) + strlen(FC.tokens[i - 1]) + strlen(FC.tokens[i]) + 1, sizeof(char));
						strcat(new_token, FC.tokens[i - 1]);
						strcat(new_token, FC.tokens[i - 2]);
						strcat(new_token, FC.tokens[i]);
						//free(FC.tokens[i - 1]);
						//free(FC.tokens[i - 2]);
						//free(FC.tokens[i]);
						FC.tokens[i] = new_token;
					}
					else {
						// 스왑어블도 아니고 컨버터블도 아님
						// 그대로 병합
						char *new_token = calloc(strlen(FC.tokens[i - 2]) + strlen(FC.tokens[i - 1]) + strlen(FC.tokens[i]) + 1, sizeof(char));
						strcat(new_token, FC.tokens[i - 2]);
						strcat(new_token, FC.tokens[i - 1]);
						strcat(new_token, FC.tokens[i]);
						//free(FC.tokens[i - 2]);
						//free(FC.tokens[i - 1]);
						//free(FC.tokens[i]);
						FC.tokens[i] = new_token;
					}
					//start_pos = i; // 병합했으므로 스타팅 포인트를 땡김
					FC.number_of_tokens -= 2; // 세개를 먹어서 하나를 만들었으므로 두개를 줄임
					FC.is_operator[i] = false; // 병합했으므로 연산자 취급 아님
					printf("병 합 완 료 : %s\n", FC.tokens[i]);
					//i-1을 prev_ptr 스왑
					FC.tokens[i-1] = FC.tokens[prev_ptr];
					prev_ptr = i; // 병합한 곳을 포인터로

				} else if (i-1 >= prev_ptr) {
					// 앞에 1개가 있으면
					// 그대로 합친다.
					char *new_token = calloc(strlen(FC.tokens[i - 1]) + strlen(FC.tokens[i]) + 1, sizeof(char));
					strcat(new_token, FC.tokens[i - 1]);
					strcat(new_token, FC.tokens[i]);
					//free(FC.tokens[i - 1]);
					//free(FC.tokens[i]);
					FC.tokens[i] = new_token;
					FC.is_operator[i] = false;
					//start_pos = i; // 병합했으므로 스타팅 포인트를 땡김
					FC.number_of_tokens-=1;
					//i-1을 prev_ptr 스왑
					FC.tokens[i-1] = FC.tokens[prev_ptr];
					prev_ptr = i; // 최근에 병합한 곳으로 동기화
				} else {
					// 달랑 연산자만 출몰...
					// 연산자 여부만 풀어버린다.
					FC.is_operator[i] = false;
				}
			} else {
				// not - op
				// printf("연산자아님 : %s\n", FC.tokens[i]);
				//start_pos++;
				//prev_ptr++;

			}
		}
		// break;
	//	getchar();
	//}
	printf("최종 식 : [%s]\n", FC.tokens[origin_size-1]);


	// fc token 에 있는 모든 것들을 free 해줘야 한다.
	return 0;
}
