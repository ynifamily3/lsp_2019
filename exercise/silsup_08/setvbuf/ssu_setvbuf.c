#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ssu_runtime.h"

#define BUFFER_SIZE 1024

void ssu_setbuf(FILE *fp, char *buf);

int main(void)
{

	struct timeval start, end;

	char buf[BUFFER_SIZE];
	char *fname = "/dev/pts/17";
	FILE *fp;

	gettimeofday(&start, NULL);

	if ((fp = fopen(fname, "w")) == NULL) {
		fprintf(stderr, "fopen error for %s", fname);
		exit(1);
	}

	ssu_setbuf(fp, buf);
	fprintf(fp, "Hello, ");
	sleep(1);
	fprintf(fp, "UNIX!!");
	sleep(1);
	fprintf(fp, "\n");
	sleep(1);
	ssu_setbuf(fp, NULL);
	fprintf(fp, "HOW");
	sleep(1);
	fprintf(fp, " ARE");
	sleep(1);
	fprintf(fp, " YOU?");
	sleep(1);
	fprintf(fp, "\n");
	sleep(1);

	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

void ssu_setbuf(FILE *fp, char *buf) {
	size_t size;
	int fd;
	int mode;

	fd = fileno(fp);

	if (isatty(fd))
		mode = _IOLBF;
	else
		mode = _IOFBF;

	if (buf == NULL) {
		mode = _IONBF;
		size = 0;
	}
	else
		size = BUFFER_SIZE;

	setvbuf(fp, buf, mode, size);
}

