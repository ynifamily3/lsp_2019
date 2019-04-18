#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	int character;

	gettimeofday(&start, NULL);

	while ((character = getc(stdin)) != EOF)
		if (putc(character, stdout) == EOF) {
			fprintf(stderr, "standard output error\n");
			exit(1);
		}

	if (ferror(stdin)) {
		fprintf(stderr, "standard input error\n");
		exit(1);
	}

	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

