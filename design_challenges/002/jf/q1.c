#include <stdio.h>
#include <stdlib.h>

int main(void) {
	printf("Enter the number : ");
	int num;
	scanf("%d", &num);
	int even = 0, odd = 0;
	for(int i = 1; i <= num; i++)
	{
		// Checking...
		if(i % 2 == 0)
		{
			even += i;
		}
		else {
			odd += i;
		}
	}
	printf("Sum of Even number : %d\n", even);
	printf("Sum of Odd number : %d\n", odd);
	exit(0);
}
