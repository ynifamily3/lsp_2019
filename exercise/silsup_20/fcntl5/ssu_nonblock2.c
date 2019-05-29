#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_CALL 100
#define MAX_BUF 20000

void serror(char *str)
{
	fprintf(stderr, "%s\n", str);
	exit(1);
}

int main(void)
{
	int nread, nwrite, val, i = 0;
	char *ptr;
	char buf[MAX_BUF];

	int call[MAX_CALL];

	nread = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %d bytes\n", nread);

	if ((val = fcntl(STDIN_FILENO, F_GETFL, 0)) < 0)
		serror("fcntl F_GETFL error");
	val |= O_NONBLOCK;
