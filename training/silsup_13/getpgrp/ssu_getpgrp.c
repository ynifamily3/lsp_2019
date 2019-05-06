#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	pid_t pgid; // 프로세스 그룹 ID
	pid_t pid; // 프로세스 ID

	pid = getpid();
	pgid = getpgrp();
	printf("pid: %d, pgid: %d\n", pid, pgid);
	exit(0);
}

