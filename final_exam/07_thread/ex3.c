#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define N 4

int matrix1[N][N] = {{1,2,3,4},{2,3,4,5},{3,4,5,6},{4,5,6,7}};
int matrix2[N][N] = {{7,6,5,4},{6,5,4,3},{5,4,3,2},{4,3,2,1}};

struct arg {
		int x;
		int y;
};

void *ssu_thread(void *arg) {
		struct arg str_arg = *(struct arg*) arg;
		int result;
		int i;

		return (void *)result;
}

int main(void)
{
		struct arg arg[N][N];
		pthread_t tid[N][N];
		int result[N][N];
		int i, j;

		for (i = 0; i < N; i++) {
				for (j = 0; j < N; j++) {
						arg[i][j].x = i;
						arg[i][j].y = j;
						if (pthread_create(&tid[i][j], NULL, ssu_thread, (void *)arg[i][j]) < 0) {
								fprintf(stderr, "pthread_create error\n");
								exit(1);
						}
				}
		}

}

