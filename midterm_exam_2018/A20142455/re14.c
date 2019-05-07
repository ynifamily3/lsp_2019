#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int ssu_write(int fd, char *buf);

int main(void)
{
	char buf[128];
	char pattern[4] = "bcd";
	char *pos1 = buf, *pos2 = buf;
	char *fname_in = "in.txt";
	char *fname_out = "out.txt";
	int size;
	int fd1, fd2; // fd1 인풋, fd2 아웃풋
	int i = 0;

	if ( (fd1 = open(fname_in, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname_in);
		exit(1);
	}

	if ((size = read(fd1, buf, 128)) < 0) {
		fprintf(stderr, "read error for %s\n", fname_in);
		exit(1);
	}
	
	close(fd1);
	
	if((fd2 = open(fname_out, O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0) {
		fprintf(stderr, "creat error for %s\n", fname_out);
		exit(1);
	}

	while ( (pos2 = strstr(pos1, pattern)) != NULL ) {
		*pos2 = '\0';
		ssu_write(fd2, pos1);
		pos1 = pos2 + strlen(pattern);
		lseek(fd2, strlen(pattern), SEEK_CUR);
	}
	ssu_write(fd2, pos1);

	close(fd2);

	return 0;
}

int ssu_write(int fd, char *buf)
{
	return write(fd, buf, strlen(buf));
}
