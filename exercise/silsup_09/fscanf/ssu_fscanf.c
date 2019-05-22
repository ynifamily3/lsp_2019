#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

#define BUFFER_SIZE 1024

int main(void)
{
	struct timeval start, end;
	char *fname = "ssu_test.dat";
	char name[BUFFER_SIZE];
	FILE *fp;
	int age;
	gettimeofday(&start, NULL);

	fp = fopen(fname, "r");
	fscanf(fp, "%s%d", name, &age);
	fclose(fp);
	fp = fopen(fname, "w");
	fprintf(fp, "%s is %d years old\n", name, age);
	fclose(fp);
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}
