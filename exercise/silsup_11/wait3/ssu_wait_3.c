#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	gettimeofday(&start, NULL);

	if (fork() == 0)
		execl("/bin/echo", "echo", "this is", "message one", (char *)0);

	if (fork() == 0)
		execl("/bin/echo", "echo", "this is", "message Two", (char *)0);

	printf("parent: waiting for children\n");

	while (wait((int *)0) != -1);

	printf("parent: all children terminated\n");
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

