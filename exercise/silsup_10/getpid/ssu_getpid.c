#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	gettimeofday(&start, NULL);
<<<<<<< HEAD
	printf("Process ID			= %d\n", getpid());
=======
	printf("Process ID		= %d\n", getpid());
>>>>>>> 94c594b692b7c00ebbeffe6d994b73f901e28dcc
	printf("Parent process ID	= %d\n", getppid());
	printf("Real user ID		= %d\n", getuid());
	printf("Effective user ID	= %d\n", geteuid());
	printf("Real group ID		= %d\n", getgid());
<<<<<<< HEAD
	printf("Effective group ID 	= %d\n", getegid());
=======
	printf("Effective group ID	= %d\n", getegid());
>>>>>>> 94c594b692b7c00ebbeffe6d994b73f901e28dcc
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}
<<<<<<< HEAD
=======

>>>>>>> 94c594b692b7c00ebbeffe6d994b73f901e28dcc
