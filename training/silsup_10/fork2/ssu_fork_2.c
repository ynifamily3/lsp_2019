#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	pid_t pid;
	struct timeval start, end;
	char character, first, last;
	long i;
	gettimeofday(&start, NULL);

	if ((pid = fork()) > 0) {
		first = 'A';
		last = 'Z';
	}
	else if (pid == 0) {
		first = 'a';
		last = 'z';
	}
	else {
		fprintf(stderr, "%s\n", argv[0]);
		exit(1);
	}

	for (character = first; character <= last; character++) {
		for (i = 0; i <= 100000; i++);
	
		write(1, &character, 1);
	}

	printf("\n");
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}
