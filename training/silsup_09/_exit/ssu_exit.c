#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	gettimeofday(&start, NULL);
	printf("Good afternoon");
	gettimeofday(&end, NULL);
	putchar('\n');
	ssu_runtime(&start, &end);
	exit(0);
}
