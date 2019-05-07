#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define STUDENT_NUM 3
#define Q_SIZE 10

typedef struct _student {
	char name[10]; // 이름
	int score; // 점수
	char res[BUFFER_SIZE]; // 학생이 쓴 답
} Student;

char answer[BUFFER_SIZE] = "1233423413"; // test's answer

int main(void)
{
	char *ssu_answer = "ssu_answer.txt";
	char *ssu_res = "ssu_res.txt";
	char tmp_score[BUFFER_SIZE]; // 파일에 쓴걸 출력하기 위한걸걸?
	FILE *fp;
	int i, j = 0;
	Student list[STUDENT_NUM];
	if ((fp = fopen(ssu_answer, "r")) == NULL) {
		fprintf(stderr, "fopen error\n");
		exit(1);
	}
	for (j = 0; j < STUDENT_NUM; j++) {
		list[j].score = 0;
		fgets(list[j].name, 10, fp);
		for (i = 0; list[j].name[i] != '\n'; i++);
		list[j].name[i] = '\0';
		fgets(list[j].res, BUFFER_SIZE, fp);
		list[j].res[Q_SIZE] = '\0';
		for (i = 0; i < Q_SIZE; i++) {
			if (answer[i] == list[j].res[i]) {
				list[j].score += 10;
				list[j].res[i] = 'O';
			} else {
				list[j].res[i] = 'X';
			}
		}
	}
	fclose(fp);
	for (i = 0; i < STUDENT_NUM; i++) {
		printf("Student name : %s , score : %d , res : %s\n", list[i].name, list[i].score, list[i].res);
	}
	if ((fp = fopen(ssu_res, "w")) == NULL) {
		fprintf(stderr, "fopen error\n");
		exit(1);
	}
	for (i = 0; i < STUDENT_NUM; i++) {
		fprintf(fp, "%s |%d| %s\n", list[i].name, list[i].score, list[i].res);
	}
	fclose(fp);
	exit(0);

}
