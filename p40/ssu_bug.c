#include <stdio.h>
#include <stdlib.h>

#define NUM 5

int score[NUM];

int ssu_sum(int count) {
	int i;
	int sum;

	for (i = 0; i < count; i++)
		sum += score[i];
	return sum;
}

int main(void)
{
	int count = 0;
	int i = 0;

	printf("input scores. input -1 to finish. \n");

	for (i = 0; i < NUM; i++) {
		printf("score #%d : ", count + 1);
		scanf("%d", &score[count]);

		if (score[count] == -1)
			break;

		count++;
	}

	printf("%d scores read.\n", count);
	printf("--- result ---\n");
	printf("sum : %d avg : %d\n", ssu_sum(count), ssu_sum(count) / count);
	exit(0);
}
