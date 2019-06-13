#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int glob_val = 10;

int main(void)
{
	pid_t pid;
	int loc_val;

	loc_val = 100;
	fprintf(stdout, "before fork stdout|"); // 표준 출력으로 표출, 라인 버퍼링이므로 버퍼에 저장됨
	fprintf(stderr, "before fork stderr|"); // 먼저 이것이 출력된다 (stderr은 버퍼링이 없다.)

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid == 0) {
		// 자식 프로세스
		glob_val++;
		loc_val++;
	}
	else {
		// 부모 프로세스
		sleep(3);
	}
	printf("pid = %d, glob_val = %d, loc_val = %d\n", getpid(), glob_val, loc_val);

	exit(0);
}

