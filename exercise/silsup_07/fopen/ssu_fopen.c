#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	char *fname = "ssu_test.txt";
	char *mode = "r";

	gettimeofday(&start, NULL);

	if (fopen(fname, mode) == NULL) {
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(1);
	}
	else
		printf("Success!\nFilename: <%s>, mode: <%s>\n", fname, mode);
	gettimeofday(&end, NULL);

	ssu_runtime(&start, &end);

	exit(0);
}

