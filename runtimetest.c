#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

void ssu_runtime(struct timeval *start, struct timeval *end) 
{
	end->tv_sec -= start->tv_sec;
	if (end->tv_usec < start->tv_usec)
	{
		end->tv_sec--;
		end->tv_usec += 1000000;
	}
	end->tv_usec -= start->tv_usec;
	printf("runtime : %ld:%06ld\n", end->tv_sec, end->tv_usec);
}

int main(void)
{
	struct timeval start, end;

	gettimeofday(&start, NULL);

	for (int i = 0 ; i < 1000000000; i++);
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}
