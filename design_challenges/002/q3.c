#include <stdio.h>
#include <stdlib.h>

int main(void) {
	char *file = "q3java.txt";
	/****** 두번째 매개변수 ******/
	/****** true : 기존 파일의 내용 이후부터 쓰여짐 ******/
	/****** false : 처음부터 쓰여짐 ******/
	FILE *writer = fopen(file, "a"); if(!writer)exit(1);
	fprintf(writer, "2019 OSLAB\n");
	fprintf(writer, "Linux System Programming\n");
	fflush(writer);
	printf("DONE\n");
	if(writer != NULL)
	fclose(writer);
	exit(0);
}
