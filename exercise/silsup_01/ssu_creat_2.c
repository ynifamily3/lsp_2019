#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval* begin_t, struct timeval* end_t) {
	end_t -> tv_sec -= begin_t -> tv_sec; //  

	if (end_t -> tv_usec < begin_t -> tv_usec) {
		end_t -> tv_sec--;
		end_t -> tv_usec += SECOND_TO_MICRO;
	}   

	end_t -> tv_usec -= begin_t -> tv_usec;
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t -> tv_sec, end_t ->tv_usec);
}


// file create and close and open (RD)
int main(void)
{
	struct timeval begin_t, end_t;
	int i;

	gettimeofday(&begin_t, NULL);

	char *fname = "ssu_test.txt";
	int fd;

	if ((fd = creat(fname, 0666)) < 0) {
		fprintf(stderr, "error creating file : %s\n", fname);
		exit(1);
	}
	else {
		close(fd); // file close and
		fd = open(fname, O_RDWR);
		printf("Succeeded!\n<%s> is new readable and writable\n", fname);
	}
	
	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
