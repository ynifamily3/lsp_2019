#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	char *fname = "ssu_test.txt";
	FILE *fp;
	long position;
	int character;

	gettimeofday(&start, NULL);

	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(1);
	}

	printf("Input number >>");
	scanf("%ld", &position);
	fseek(fp, position - 1, SEEK_SET);
	character = getc(fp);
	printf("%ldth character => %c\n", position, character);
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}
