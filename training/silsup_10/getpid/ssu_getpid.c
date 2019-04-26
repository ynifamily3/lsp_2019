#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	gettimeofday(&start, NULL);
	printf("Process ID		= %d\n", getpid());
	printf("Parent process ID	= %d\n", getppid());
	printf("Real user ID		= %d\n", getuid());
	printf("Effective user ID	= %d\n", geteuid());
	printf("Real group ID		= %d\n", getgid());
	printf("Effective group ID	= %d\n", getegid());
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

