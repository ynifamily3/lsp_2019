#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	char *fname = "ssu_test.txt";
	FILE *fp;
	long fsize;

	gettimeofday(&start, NULL);
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	printf("The size of <%s> is %ld bytes\n", fname, fsize);
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

