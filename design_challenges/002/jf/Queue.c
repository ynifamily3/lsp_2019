#define QUEUE_SIZE 10
#include <stdlib.h>
#include <stdio.h>

// const int QUEUE_SIZE = 10;
int *stack;
int back;
void Queue(){
	back = -1;
	stack=(int *)calloc(QUEUE_SIZE,sizeof(int));
}
int peek() {
	return stack[0];
}
void push(int value) {
	stack[++ back] = value;
	printf("%d PUSH !\n", stack[back]);
}
int pop() {
	printf("%d POP !\n", stack[0]);
	for(int i = 1; i <= back;++ i)
	{
		stack[i - 1] = stack[i];
	}
	return stack[back --];
}
void printQueue() {
	printf("\n-----QUEUE LIST-----\n");
	for(int i = 0; i <= back;++ i)
	{
		printf("%d\n", stack[i]);
	}
	printf("-----END OF LIST-----\n");
}
