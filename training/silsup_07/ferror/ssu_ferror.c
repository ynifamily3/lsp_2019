#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	struct timeval start, end;
	FILE *fp;
	int character;
	gettimeofday(&start, NULL);

	if (argc != 2) {
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	if ((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", argv[1]);
		exit(1);
	}

	character = fgetc(fp);

	while (!feof(fp)) {
		fputc(character, stdout);

		if (ferror(fp)) {
			fprintf(stderr, "Error detected!!\n");
			clearerr(fp);
		}

		character = fgetc(fp);
	}

	fclose(fp);
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

