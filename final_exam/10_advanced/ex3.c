#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	struct flock lock;
	char *fname = "test.txt";
	int fd;
	pid_t pid;
	char c;

	if ((fd = open(fname, O_RDWR, 0644)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	lock.l_type = F_WRLCK;

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	} else if (pid == 0) {
		sleep(1);
		int i = 0;
		while (read(fd, &c, 1) > 0) {
			lock.l_whence = SEEK_SET;
			lock.l_start = i++; // lock alaredy readed 1 byte
			lock.l_len = 1;
			if (fcntl(fd, F_SETLKW, &lock) == -1) {
				fprintf(stderr, "fcntl error\n");
				exit(1);
			}
			fprintf(stderr, "%c", c);
		}
		exit(1);
	} else {
		// child
		if (fcntl(fd, F_SETLKW, &lock) == -1) {
			fprintf(stderr, "fcntl error\n");
			exit(1);
		}
		while (1); // no 
	}
}
