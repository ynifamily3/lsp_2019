#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	int i;
	struct timeval start, end;
	gettimeofday(&start, NULL);

	for (i = 0; i < argc; i++)
		printf("argv[%d] : %s\n", i, argv[i]);
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

