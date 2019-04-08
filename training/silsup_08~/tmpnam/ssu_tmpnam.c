#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

#define MAX_LINE 4096

int main(void)
{
	struct timeval start, end;
	char buf[MAX_LINE];
	char name[L_tmpnam];
	FILE *fp;

	gettimeofday(&start, NULL);

	printf("temp file 1 : %s\n", tmpnam(NULL));
	tmpnam(name);
	printf("temp file 2 : %s\n", name);

	if ((fp = tmpfile()) == NULL) {
		fprintf(stderr, "tmpfile error\n");
		exit(1);
	}

	fputs("tmpfile create temporary file.\n", fp);
	fseek(fp, 0, SEEK_SET);

	if (fgets(buf, sizeof(buf), fp) == NULL) {
		fprintf(stderr, "fgets error\n");
		exit(1);
	}

	fputs(buf, stdout);
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

