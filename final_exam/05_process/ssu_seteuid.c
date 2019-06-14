#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void)
{
	int fd, state;

	state = seteuid(1000); // make effective uid 1001
	if (state < 0) {
		fprintf(stderr, "seteuid error\n");
		exit(1);
	}

	state = setegid(1000);
	if (state < 0) {
		fprintf(stderr, "setegid error\n");
		perror("error? ");
		exit(1);
	}

	if ((fd = open("ssu_test_2.text", O_CREAT | O_RDWR, S_IRWXU)) < 0) {
		fprintf(stderr, "open error\n");
		exit(1);
	}

	close(fd);
	exit(0);
}

