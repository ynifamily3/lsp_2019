#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ssu_runtime.h"

#define BUFFER_SIZE 1024

int main(void)
{
	struct timeval start, end;
	char buf[BUFFER_SIZE];

	gettimeofday(&start, NULL);

	setbuf(stdout, buf);
	printf("Hello, ");
	sleep(1);
	printf("OSLAB!!");
	sleep(1);
	printf("\n");
	sleep(1);

	setbuf(stdout, NULL);
	printf("How");
	sleep(1);
	printf(" are");
	sleep(1);
	printf(" you?");
	sleep(1);
	printf("\n");
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

