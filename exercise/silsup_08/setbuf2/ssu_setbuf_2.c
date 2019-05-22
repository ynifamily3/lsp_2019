#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

#define BUFFER_SIZE 1024

int main(void)
{
	struct timeval start, end;
	gettimeofday(&start, NULL);
	char buf[BUFFER_SIZE];
	int a, b;
	int i;

	setbuf(stdin, buf);
	scanf("%d %d", &a, &b);

	for (i = 0; buf[i] != '\n'; i++)
		putchar(buf[i]);

	putchar('\n');
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}
