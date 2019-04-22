#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ssu_runtime.h"

static void ssu_charatatime(char *str);

int main(void)
{
	pid_t pid;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid == 0)
		ssu_charatatime("output from child\n");
	else
		ssu_charatatime("output from parent\n");
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

static void ssu_charatatime(char *str) {
	char *ptr;
	int print_char;

	setbuf(stdout, NULL);

	for (ptr = str; (print_char = *ptr++) != 0; ) {
		putc(print_char, stdout);
		usleep(10);
	}
}

