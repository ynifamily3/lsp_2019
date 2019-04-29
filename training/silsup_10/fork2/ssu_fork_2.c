#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	pid_t pid;
<<<<<<< HEAD
	struct timeval start, end;
	char character, first, last;
	long i;
	gettimeofday(&start, NULL);

=======
	char character, first, last;
	long i;
	struct timeval start, end;

	gettimeofday(&start, NULL);
>>>>>>> 94c594b692b7c00ebbeffe6d994b73f901e28dcc
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
<<<<<<< HEAD
		for (i = 0; i <= 100000; i++);
	
=======
		for (i = 0; i <= 100000; i++)
			;

>>>>>>> 94c594b692b7c00ebbeffe6d994b73f901e28dcc
		write(1, &character, 1);
	}

	printf("\n");
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}
