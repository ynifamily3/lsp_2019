#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

void ssu_signal_handler(int signo);

jmp_buf jump_buffer;

int main(void)
{
	signal(SIGINT, ssu_signal_handler); // Ctrl-C에 대한 시그널 핸들러를 등록한다.

	while (1) {
		if (setjmp(jump_buffer) == 0) { // 프로세스의 현재 상태를 저장한다. 직접 호출된 상태면 0을 리턴
			printf("Hit Ctrl-c at anytime ... \n");
			pause(); // 프로세스를 정지시킨다. 시그널 발생을 기다린다.
		}
	}

	exit(0);
}

void ssu_signal_handler(int signo) {
	char character;

	signal(signo, SIG_IGN); // 해당 시그널 (여기서는 Ctrl-C)을 무시한다.
	printf("Did you hit Ctrl-c?\n" "Do you really want to quit? [y/n] ");
	character = getchar();

	if (character == 'y' || character == 'Y')
		exit(0);
	else {
		signal(SIGINT, ssu_signal_handler); // 해당 시그널 (여기서는 Ctrl-C)를 ssu_signal_handler로 등록한다.
		longjmp(jump_buffer, 1); // setjmp 한 곳으로 돌아간다.
	}
}

