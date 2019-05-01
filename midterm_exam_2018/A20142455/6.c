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
		fprintf(stderr, "open error for %s\n", ssu_answer);
		exit(1);
	}

	for (j = 0; j < STUDENT_NUM; j++) {
		
		list[j].score = 0; // 학생 점수 0화
		if ( fgets(list[j].name, BUFFER_SIZE, fp) == NULL ) {
			fprintf(stderr, "fgets error\n");
			exit(1);
		}
		i = 0;
		for (; list[j].name[i] != '\n'; i++);
		list[j].name[i] = '\0';
		if ( fgets(tmp_score, BUFFER_SIZE, fp) == NULL ) {
			fprintf(stderr, "fgets error\n");
			exit(1);
		}
		i = 0;
		for (; tmp_score[i] != '\n'; i++);
		tmp_score[i] = '\0';
		for (i = 0; answer[i] != '\0'; i++) {
			if (answer[i] != tmp_score[i])
				list[j].res[i] = 'X';
			else {
				list[j].res[i] = 'O';
				list[j].score += 10;
			}
		}
		list[j].res[i] = '\0';
		printf("Student name : %s , score : %d , res : %s\n", list[j].name, list[j].score, list[j].res);
	}
	fclose(fp);
	if((fp = fopen(ssu_res, "wb")) == NULL) {
		fprintf(stderr, "open error for %s\n", ssu_res);
		exit(1);
	}
	for (j = 0; j < STUDENT_NUM; j++)
		fprintf(fp, "%s |%d| %s\n", list[j].name, list[j].score, list[j].res);
	fclose(fp);
	exit(0);
}
