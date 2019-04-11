#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void)
{
	int x;
	x = 0;
	printf("포크 전엔 어떤 게 실행 되는가? %d\n", x);
	pid_t pid = fork();
	if (pid > 0) {
		while(1) {
			sleep(1);
			printf("부모 프로세스\n");
		}
	} 
	else if (pid == 0) {
		while(1) {
		sleep(1);
		printf("자식 프로세스\n");
		}
	}
	else {
		fprintf(stderr, "fork failed\n");
		exit(1);
	}
	exit(0);
}

