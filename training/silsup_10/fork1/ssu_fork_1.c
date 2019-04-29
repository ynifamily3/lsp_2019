#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ssu_runtime.h"

char glob_str[] = "write to standard output\n";
int glob_val = 10;

int main(void)
{
<<<<<<< HEAD
	struct timeval start, end;
	pid_t pid;
	int loc_val;
	gettimeofday(&start, NULL);
	loc_val = 100;
	if (write(STDOUT_FILENO,
				glob_str, sizeof(glob_str)-1) != sizeof(glob_str)-1) {
=======
	pid_t pid;
	int loc_val;
	struct timeval start, end;
	loc_val = 100;
	gettimeofday(&start, NULL);
	if (write(STDOUT_FILENO, glob_str, sizeof(glob_str)-1) != sizeof(glob_str)-1) {
>>>>>>> 94c594b692b7c00ebbeffe6d994b73f901e28dcc
		fprintf(stderr, "write error\n");
		exit(1);
	}

	printf("before fork\n");

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid == 0) {
		glob_val++;
		loc_val++;
	}
	else
		sleep(3);

<<<<<<< HEAD
	printf("pid = %d, glob_val = %d, loc_val = %d\n",
			getpid(), glob_val, loc_val);
=======
	printf("pid = %d, glob_val = %d, loc_val = %d\n", getpid(), glob_val, loc_val);
>>>>>>> 94c594b692b7c00ebbeffe6d994b73f901e28dcc
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}
<<<<<<< HEAD
=======

>>>>>>> 94c594b692b7c00ebbeffe6d994b73f901e28dcc
