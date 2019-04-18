#include <stdio.h>
#include <unistd.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval start, end;
	gettimeofday(&start, NULL);
	printf("Good afternoon?");
	gettimeofday(&end, NULL);
	//ssu_runtime(&start, &end);
	_exit(0);
}

