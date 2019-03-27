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

void lex_go(const char *statement, struct statement_container * container)
{

}

char *cut_lex(char *start)
{
	// 다음 괄호나 연산자까지 자르고 오른편 포인터offset를 리턴
	char *curr = start;
	int count_braket_open = 0;
	for (;curr != '\0';curr++) {
		if (*curr == '(') count_braket_open++;
		if (*curr == ')' && count_braket_open<=1) return curr;
		else if (*curr == ')') count_braket_open--;
		
		// 각 연산자와의 strcmp를 통해 디텍트한다.
		if (count_braket_open == 0) {
			int i, j;
			for (i = 0; i < NUMBER_OF_OPERATORS; i++) {
				int opsize = strlen(operator[i]);
				int is_op = 1;
				for (j = 0; j < opsize; j++) {
					if (*curr != operator[i][j]) {is_op = 0; break;}
				}
				if (is_op) {
					return curr+j;
				}
			}
		}
	}
	return curr;
}

char buf[1000] = "((t+3)&&stop>a||a<stop)&&(b||c)";

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

	char *ptr1, *ptr2;
	ptr1 = container.elem[0]->expression;
	ptr2 = cut_lex(ptr1);
	// 하나의 파스트리를 생성한다.
	container.elem[container.size]->is_leaf = 0;
	container.elem[container.size]->expression = (*ptr1 == '(' ? ptr1+1 : ptr1);
	container.elem[container.size]->expression_strlen = (*ptr2 == ')' ? (int)(ptr2 - ptr1 - 1) : (int)(ptr2 - ptr1));
	container.size++;

	printf("[");
	for (int i = 0; i < container.elem[0]->expression_strlen; i++) {
		printf("%c", container.elem[0]->expression[i]);
	}
	printf("] ");

	// 다음 파스트리를 생성한다.
	ptr1 = (*ptr2 == ')' ? ptr2+1 : ptr2);
	ptr2 = cut_lex(ptr1);
	container.elem[container.size]->is_leaf = 0;
	container.elem[container.size]->expression = (*ptr1 == '(' ? ptr1+1 : ptr1);
	container.elem[container.size]->expression_strlen = (*ptr2 == ')' ? (int)(ptr2 - ptr1 - 1) : (int)(ptr2 - ptr1));
	container.size++;

	printf("[");
	for (int i = 0; i < container.elem[1]->expression_strlen; i++) {
		printf("%c", container.elem[1]->expression[i]);
	}
	printf("] ");

	// 마지막 파스트리를 생성한다.
	ptr1 = (*ptr2 == ')' ? ptr2+1 : ptr2);
	ptr2 = cut_lex(ptr1);
	container.elem[container.size]->is_leaf = 0;
	container.elem[container.size]->expression = (*ptr1 == '(' ? ptr1+1 : ptr1);
	container.elem[container.size]->expression_strlen = (*ptr2 == ')' ? (int)(ptr2 - ptr1 - 1) : (int)(ptr2 - ptr1));
	container.size++;

	printf("[");
	for (int i = 0; i < container.elem[2]->expression_strlen; i++) {
		printf("%c", container.elem[2]->expression[i]);
	}
	printf("] ");


	printf("\n");
	/*
	 1. 전체적 텍스트에서 괄호와 연산자를 기준으로 자른다. (괄호-비괄호)
	 [stop>a||a<stop][&&][b||c]
	 2. 연산자가 있을 경우 왼쪽 오른쪽 원소를 뺏어와 파스트리를 만든다.
	 <parse_tree>
	 3. 파스트리 혹은 구문이 하나만 남을때까지 계속한다.
	*/
	return 0;
}