#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

#define BUFFER_SIZE 1024

int main(void)
{
	struct timeval start, end;
	char buf[BUFFER_SIZE];
	char name[L_tmpnam];
	FILE *fp;

	gettimeofday(&start, NULL);

	printf("Temporary filename <<%s>>\n", tmpnam(name));

	if ((fp = tmpfile()) == NULL) {
		fprintf(stderr, "tmpfile create error!!\n");
		exit(1);
	}

	fputs("create tmpfile success!!\n", fp);
	rewind(fp);
	fgets(buf, sizeof(buf), fp);
	puts(buf);

	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

