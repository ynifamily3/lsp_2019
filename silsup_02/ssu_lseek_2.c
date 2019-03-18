#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#define SECOND_TO_MICRO 1000000 // 10^6


#define CREAT_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void ssu_runtime(struct timeval* begin_t, struct timeval* end_t) {
	end_t -> tv_sec -= begin_t -> tv_sec; //  

	if (end_t -> tv_usec < begin_t -> tv_usec) {
		end_t -> tv_sec--;
		end_t -> tv_usec += SECOND_TO_MICRO;
	}   

	end_t -> tv_usec -= begin_t -> tv_usec;
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t -> tv_sec, end_t ->tv_usec);
}


char buf1[] = "1234567890";
char buf2[] = "ABCDEFGHIJ";

int main(void)
{
	struct timeval begin_t, end_t;

	gettimeofday(&begin_t, NULL);

	char *fname = "ssu_hole.txt";
	int fd;

	if ((fd = creat(fname, CREAT_MODE)) < 0) { 
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}

	if (write(fd, buf1, 12) != 12) {
		fprintf(stderr, "buf1 write error\n");
		exit(1);
	}

	if (lseek(fd, 15000, SEEK_SET) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	if (write(fd, buf2, 12) != 12) {
		fprintf(stderr, "buf2 write error\n");
		exit(1);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
