#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define STUDENT_NUM 3
#define Q_SIZE 10

typedef struct _student {
	char name[10];
	int score;
	char res[BUFFER_SIZE];
} Student;

char answer[BUFFER_SIZE] = "1233423413"; // test's answer

int main(void)
{
	char *ssu_answer = "ssu_answer.txt";
	char *ssu_res = "ssu_res.txt";
	char tmp_score[BUFFER_SIZE];
	FILE *fp;
	int i, j = 0;
	Student list[STUDENT_NUM];

	for (j = 0; j < 
}
