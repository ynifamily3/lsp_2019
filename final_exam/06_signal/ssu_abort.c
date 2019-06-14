#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	printf("abort terminate this program\n");
	abort();
	printf("this line never reached\n");
	exit(0);
}

