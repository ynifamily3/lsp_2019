#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>

int ssu_daemon_init(void);
int main(void)
{
	pid_t pid;

	pid = getpid();
	printf("parent process : %d\n", pid);
	printf("daemon process initialization\n");

	if (ssu_daemon_init() < 0) {
		fprintf(stderr, "ssu_daemon_init failed\n");
		exit(1);
	}
	exit(0);
}

int ssu_daemon_init(void) {
	pid_t pid;
	int fd, maxfd;

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid != 0)
		exit(0); // 부모 프로세스의 경우 종료한다.

	pid = getpid(); // 자기 자신(데몬)의 프로세스 번호 구하기
	printf("process %d running as daemon\n", pid);
	setsid(); // 프로세스 그룹에서 탈퇴, 새로운 프로세스 그룹을 생성한다.
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN); // 터미널 입출력 시그널을 무시한다.
	maxfd = getdtablesize(); // 파일 디스크립터의 최대 개수를 구할 수 있다.

	for (fd = 0; fd < maxfd; fd++)
		close(fd); // 모든 파일 디스크립터를 닫는다.

	umask(0);
	chdir("/");
	fd = open("/dev/null", O_RDWR); // return 0 (stdin)
	dup(0); // dup 0 and allocate to fd 1 (stdout)
	dup(0); // dup 0 and allocate to fd 2 (stderr)
	/*
	   do daemon works...
	 */
	return 0;
}

