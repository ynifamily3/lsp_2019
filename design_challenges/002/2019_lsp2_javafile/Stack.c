#ifndef STACK
#define STACK
#include <stdlib.h>
#include <stdio.h>

	int top;
	int* stack;

	const int STACK_SIZE = 10;

	void Stack(){
		top = -1;
		stack = (int *)calloc(STACK_SIZE, sizeof(int));
	}

	int peek(){
		return stack[top];
	}

	void push(int value){
		stack[++top] = value;
		printf("%d PUSH !\n", stack[top]);
	}

	int pop(){
		printf("%d POP !\n", stack[top]);
		return stack[top--];
	}

	void printStack(){
		printf("\n-----STACK LIST-----\n");

		for(int i=top; i>=0; i--){
			printf("%d\n",stack[i]);
		}

		printf("-----END OF LIST-----\n");
	}
#endif