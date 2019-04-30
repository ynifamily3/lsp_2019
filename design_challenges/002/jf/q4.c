#include "Queue.c"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	Queue();
	push(5);
	push(2);
	push(3);
	push(4);
	push(1);
	printQueue();
	pop();
	pop();
	push(15);
	printf("FRONT OF QUEUE : %d\n", peek());
	printQueue();
	pop();
	pop();
	pop();
	pop();
	push(30);
	printQueue();
	exit(0);
}
