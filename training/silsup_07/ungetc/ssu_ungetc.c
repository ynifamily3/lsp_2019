#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	char operator;
	FILE *fp;
	int character;
	int number = 0;

	gettimeofday(&start, NULL);

	if ((fp = fopen("ssu_expr.txt", "r")) == NULL) {
		fprintf(stderr, "fopen error for ssu_expr.txt\n");
		exit(1);
	}

	while (!feof(fp)) {
		while ((character = fgetc(fp)) != EOF && isdigit(character))
			number = 10 * number + character - 48;

		fprintf(stdout, " %d\n", number);
		number = 0;

		if (character != EOF) {
			ungetc(character, fp);
			operator = fgetc(fp);
			printf("Operator => %c\n", operator);
		}
	}
	
	fclose(fp);
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

