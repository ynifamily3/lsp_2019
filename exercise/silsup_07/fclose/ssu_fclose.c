#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	char *fname = "ssu_test.txt";
	FILE *fp;

	gettimeofday(&start, NULL);

	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(1);
	}
	else {
		printf("Success!\n");
		printf("Opening \"%s\" in \"r\" mode!\n", fname);
	}



	fclose(fp);
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

