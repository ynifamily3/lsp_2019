#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

#ifdef PATH_MAX
static int pathmax = PATH_MAX;
#else
static int pathmax = 0;
#endif
#define MAX_PATH_GUESSED 1024

#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

char *pathname;
char command[LINE_MAX], grep_cmd[LINE_MAX];

int ssu_do_grep(void) {
	struct dirent *dirp;
	struct stat statbuf;
	char *ptr;
	DIR *dp;

	// 디렉토리인지 확인하기. 아니라면 정보 출력 (grep 실행)
	if (lstat(pathname, &statbuf) < 0) {
		fprintf(stderr, "lstat error for %s\n", pathname);
		return 0;
	}

	// dirctroy 가 아닐 시 grep 실행 grep + pathname이면 된다.
	if (S_ISDIR(statbuf.st_mode) == 0) {
		strcpy(command, grep_cmd);
		strcat(command, " ");
		strcat(command, pathname);
		printf("%s : \n", pathname);
		system(command);
		return 0;
	}

	// path pointer 작업
	ptr = pathname + strlen(pathname);
	*ptr++ = '/';
	*ptr = '\0';

		
	if((dp = opendir(pathname)) == NULL) {
		fprintf(stderr, "opendir error for %s\n", pathname);
		return 0;
	}

	while ( (dirp = readdir(dp)) != NULL ) {
		if ( strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0 ) {
			continue;
		}

		strcpy(ptr, dirp->d_name);

		// 패스를 바꿔 재귀호출
		ssu_do_grep();
	}
	// 디렉토리에서 빼 주는 단계
	ptr[-1] = 0;
	closedir(dp);
	return 0;
}

void ssu_make_grep(int argc, char *argv[]) {
	int i;
	strcpy(grep_cmd, " grep");
	for (i = 1; i < argc - 1; i++) {
		strcat(grep_cmd, " ");
		strcat(grep_cmd, argv[i]);
	}

}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s <-CVbchilnsvwx> <-num> <-A num> <-B num> <-f file> <-e> expr <directory>\n", argv[0]);
		exit(1);
	}
	if (pathmax == 0) {
		if ( (pathmax = pathconf("/", _PC_PATH_MAX)) < 0)
			pathmax = MAX_PATH_GUESSED;
		else
			pathmax++;
	}
	if ((pathname = (char *)malloc(pathmax + 1)) == NULL) {
		fprintf(stderr, "malloc error\n");
		exit(1);
	}
	strcpy(pathname, argv[argc-1]);
	ssu_make_grep(argc, argv);
	ssu_do_grep();
	exit(0);
}
