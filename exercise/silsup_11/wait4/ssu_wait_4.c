#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	pid_t child1, child2;
	int pid, status;
	
	gettimeofday(&start, NULL);

	if ((child1 = fork()) == 0)
		execlp("date", "date", (char *)0);

	if ((child2 = fork()) == 0)
		execlp("who", "who", (char *)0);

	printf("parent: waiting for children\n");

	while ((pid = wait(&status)) != -1) {
		if (child1 == pid)
			printf("parent: first child: %d\n", (status >> 8));
		else if (child2 == pid)
			printf("parent: second child: %d\n", (status >> 8));
	}

	printf("parent: all children terminated\n");
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}
