#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 128

int main(int argc, char *argv[])
{
	char buf[BUFFER_SIZE];
	int fd1, fd2;
	ssize_t size, size2;
	
	if (argc != 3) {
		fprintf(stderr, "Usage : %s <original_file> <dest_file>\n", argv[0]);
		exit(1);
	}

	if ((fd1 = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	if (( size = read(fd1, buf, BUFFER_SIZE)) < 0) {
		fprintf(stderr, "read error for %s\n", argv[1]);
		exit(1);
	}

	if ((fd2 = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC, 0644)) < 0) {
		fprintf(stderr, "creat error for %s\n", argv[2]);
		exit(1);
	}

	if ( (size2 = write(fd2, buf, size)) != size) {
		fprintf(stderr, "write error for %s\n", argv[2]);
		exit(1);
	}
	close(fd1);
	close(fd2);
	exit(0);
}
