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

	if ((fp = fopen(ssu_answer, "rb")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", ssu_answer);
		exit(1);
	}

	for (j = 0; j < STUDENT_NUM; j++) {
		list[j].score = 0;
		if (fgets(list[j].name, BUFFER_SIZE, fp) == NULL) {
			fprintf(stderr, "fgets error for %s\n", ssu_answer);
			exit(1);
		}
		i = 0;

		while((list[j].name[i] != '\n'))
			i++;
		list[j].name[i] = '\0';

		if (fgets(list[j].res, BUFFER_SIZE, fp) == NULL) {
			fprintf(stderr, "fgets error for %s\n", ssu_answer);
			exit(1);
		}
		i = 0;

		while((list[j].res[i] != '\n')) i++;

		list[j].res[i] = '\0';
		i = 0;

		for (; i < Q_SIZE; i++) {
			if (list[j].res[i] == answer[i]) {
				list[j].score += 10;
				list[j].res[i] = 'O';
			}
			else
				list[j].res[i] = 'X';
		}
		printf("Student name : %s, score : %d, res : %s \n", list[j].name, list[j].score, list[j].res);
	}

	fclose(fp);
	if ((fp = fopen(ssu_res, "wb")) == NULL) {
		fprintf(stderr, "fopen error for %s \n", ssu_res);
		exit(1);
	}

	for (i = 0; i < STUDENT_NUM; i++) {
		if (fputs(list[i].name, fp) == EOF) {
			fprintf(stderr, "fputs error for %s \n", "student's name" );
			exit(1);
		}

		sprintf(tmp_score, " |%d| ", list[i].score);
		if (fputs(tmp_score, fp) == EOF) {
			fprintf(stderr, "fputs error for %s \n", "tmp_score");
			exit(1);
		}

		if (fputs(list[i].res, fp) == EOF) {
			fprintf(stderr, "fputs error for %s \n", "student's res");
			exit(1);
		}
		fputc('\n', fp);
	}

	fclose(fp);
	exit(0);
}


