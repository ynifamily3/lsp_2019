#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "parse_tree.h"
#define NUMBER_OF_OPERATORS 9
char operator[NUMBER_OF_OPERATORS][32] = {"&&", "||", "==", ">=", "<=", "<", ">", "|", "&"}; // swap-able operators
// 연산자 우선순위가 높은 순서대로 배치한다.

struct parse_tree {

	int is_leaf;
	// leaf
	char *expression; // char a = 'hi'; 등등
	int expression_strlen;

	// non-leaf
	char *operator; // >= 등등
	struct parse_tree *left;
	struct parse_tree *right;
};

struct statement_container {
	int size;
	const char *statement;
	int statement_strlen;
	struct parse_tree **elem;
};

typedef struct char_op_pair {
	char *p;
	int isop;
} cop;

cop cut_lex(char *start)
{
	// 다음 괄호나 연산자까지 자르고 오른편 포인터offset를 리턴
	cop ret;
	ret.p = start;
	ret.isop = 0;
	char *curr = start;
	int count_braket_open = 0;
	for (;*curr != '\0';curr++) {
		if (*curr == '(') count_braket_open++;
		if (*curr == ')' && count_braket_open<=1) {ret.p = curr; return ret;}
		else if (*curr == ')') count_braket_open--;
		
		// 각 연산자와의 strcmp를 통해 디텍트한다.
		
		if (count_braket_open == 0) {
			int i, j;
			for (i = 0; i < NUMBER_OF_OPERATORS; i++) {
				int opsize = strlen(operator[i]);
				int is_op = 1;
				for (j = 0; j < opsize; j++) {
					if (*(curr+j) != operator[i][j]) {is_op = 0; break;}
				}
				if (is_op) {
					ret.p = curr+j;
					ret.isop = 1;
					return ret;
				}
			}
		}
	}
	return ret;
}

void make_parse_tree(struct statement_container * container)
{
	// 초기 cut 과정
	char *ptr1, *ptr2 = NULL;
	ptr1 = container->elem[0]->expression;
	// 해야 할 것 : 구문 뒤에 연산자 올 경우 그거 판별해서 알아서 자르기
	for (int cc = 0; cc < 3; cc++) {
		cop us; // 리턴된 값을 받기 위해서
		ptr1 = ptr2 ? ((*ptr2 == ')' ? ptr2+1 : ptr2)) : ptr1;
		us = cut_lex(ptr1);
		ptr2 = us.p;
		// 하나의 파스트리를 생성한다.
		container->elem[container->size]->is_leaf = us.isop;
		container->elem[container->size]->expression = (*ptr1 == '(' ? ptr1+1 : ptr1);
		container->elem[container->size]->expression_strlen = (*ptr2 == ')' ? (int)(ptr2 - ptr1 - 1) : (int)(ptr2 - ptr1));
		container->size++;
		printf ("operation : %c - ", us.isop ? 'O':'X');
		for (int i = 0; i < container->elem[cc]->expression_strlen; i++) {
			printf("%c", container->elem[cc]->expression[i]);
		}
		printf("\n");
	}
}

void make_recur_parse_tree(struct parse_tree * ptree)
{
	// 파스 트리 내부에서 또다른 수식을 평가하여 LR로 나눈다.
}

char buf[1000] = "const char *a= \"test\";&&b";

int main(void)
{
	struct statement_container container;
	// __init__
	container.size = 0;
	container.statement = buf;
	container.statement_strlen = strlen(buf);
	container.elem = (struct parse_tree **)calloc(16, sizeof(struct parse_tree *));
	for (int i = 0; i < 16; i++) {
		container.elem[i] = (struct parse_tree *)calloc(1, sizeof(struct parse_tree));
	}
	container.elem[0]->is_leaf = 1;
	container.elem[0]->expression = buf;
	// end of __init__
	
	printf("원본 : %s\n", container.elem[0]->expression);

	make_parse_tree(&container);

	/*
	 1. 전체적 텍스트에서 괄호와 연산자를 기준으로 자른다. (괄호-비괄호)
	 [stop>a||a<stop][&&][b||c]
	 2. 연산자가 있을 경우 왼쪽 오른쪽 원소를 뺏어와 파스트리를 만든다.
	 <parse_tree>
	 3. 파스트리 혹은 구문이 하나만 남을때까지 계속한다.
	*/
	return 0;
}