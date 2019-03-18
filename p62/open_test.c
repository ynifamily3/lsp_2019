#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>

#include <sys/stat.h>

// O_RDONLY O_WRONLY O_RDWR ... ... Prototype of open
#include <fcntl.h>

// for close
#include <unistd.h>

/*
int open(const char *pathname, int oflag);
int open(const char *pathname, int oflag, mode_t mode);

int close(int filedes);
*/

int main(void)
{
	int fd, i;
	int test;
	const char *pname = "test.txt";
	fd = open(pname, O_RDWR);
	printf("File: %s\nDescriptor: %d is open?\n", pname, fd);
	test = close(fd);
	printf("File: %s\nClosed Code: %d\n", pname, test);
	exit(0);
}

