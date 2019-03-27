#include <stdio.h>
#include <string.h>
#define SIZE 100
typedef char element;
typedef struct{
	element stack[SIZE];
	int top;
}STACK;
void init(STACK *s)
{
	s->top = -1;
}
int is_empty(STACK *s)
{
	return ((s->top) == -1);
}
void push(STACK *s, element item)
{
	s->stack[++(s->top)] = item;
}
element pop(STACK *s)
{
	return s->stack[(s->top)--];
}
element peek(STACK *s)
{
	return s->stack[s->top];
}
int prec(char op)
{
	switch (op){
	case'(': case')': return 0; break;
	case'+': case'-': return 1; break;
	case'*': case'/': return 2; break;
	default: return -1; break;
	}
}
void in_to_po(char exp[])
{
	int i = 0;
	char ch, top_op;
	int len = strlen(exp);
	STACK s;
	init(&s);

	for (i = 0; i < len; i++){
		ch = exp[i];
		switch (ch){
		case'+': case'-': case'*': case'/':
			while (!is_empty(&s) && (prec(ch) <= prec(peek(&s))))
				printf("%c", pop(&s));
			push(&s, ch);
			break;
		case'(':
			push(&s, ch);
			break;
		case')':
			top_op = pop(&s);
			while (top_op != '('){
				printf("%c", top_op);
				top_op = pop(&s);
			}
			break;
		default:
			printf("%c", ch);
			break;
		}
	}
	while (!is_empty(&s))
		printf("%c", pop(&s));
}
int main()
{
	char infix[SIZE] = "";
	scanf("%s", infix);
	in_to_po(infix);
}