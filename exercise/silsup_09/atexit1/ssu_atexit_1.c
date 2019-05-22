#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

void ssu_out(void);

int main(void)
{
	struct timeval start, end;
	gettimeofday(&start, NULL);
	if (atexit(ssu_out)) {
		fprintf(stderr, "atexit error\n");
		exit(1);
	}
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

void ssu_out(void) {
	printf("atexit succeeded!\n");
}

