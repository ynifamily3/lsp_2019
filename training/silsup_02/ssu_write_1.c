#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024

#define SECOND_TO_MICRO 1000000 // 10^6

void ssu_runtime(struct timeval* begin_t, struct timeval* end_t) {
	end_t -> tv_sec -= begin_t -> tv_sec; //  

	if (end_t -> tv_usec < begin_t -> tv_usec) {
		end_t -> tv_sec--;
		end_t -> tv_usec += SECOND_TO_MICRO;
	}   

	end_t -> tv_usec -= begin_t -> tv_usec;
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t -> tv_sec, end_t ->tv_usec);
}


int main(void)
{
	struct timeval begin_t, end_t;

	gettimeofday(&begin_t, NULL);

	char buf[BUFFER_SIZE];
	int length;

	length = read(0, buf, BUFFER_SIZE);
	write(1, buf, length);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
