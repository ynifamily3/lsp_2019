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

	while (fgets(buf, BUFFER_SIZE, stdin) != NULL)
		if (fputs(buf, stdout) == EOF) {
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

