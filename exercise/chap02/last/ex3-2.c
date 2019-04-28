#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

void search_directory(char *dir) {
	struct dirent *dentry; // 디렉토리 엔트리
	struct stat statbuf;
	DIR *dirp;
	char cwd[1024];

	getcwd(cwd, 1024);

	if(lstat(dir, &statbuf) < 0) {
		fprintf(stderr, "lstat error\n");
		printf("%s\n", strerror(errno));
		exit(1);
	}

	if(!S_ISDIR(statbuf.st_mode)) {
		// 일반 파일일 시 출력하고 종료
		printf("%s/%s\n", cwd, dir);
		return ;
	}

	if ((dirp = opendir(dir)) == NULL) {
		fprintf(stderr, "opendir error\n");
		printf("%s\n", strerror(errno));
		exit(1);
	}


	chdir(dir); // 해당 dir 로 이동

	while ((dentry = readdir(dirp)) != NULL) {
		if (strcmp(".", dentry->d_name) == 0 || strcmp("..", dentry->d_name) == 0) {
			continue;
		}
		search_directory(dentry->d_name); // 요소마다 검색
	}
	// 파일을 모두 search했으면 자기 자신 출력
	if (strcmp(".", dir) != 0)
	printf("%s/%s\n", cwd,dir);
	 
	chdir(cwd); //원래 위치로 복귀
}

int main(int argc, char *argv[]) {
	if (argc <2 ) {
		fprintf(stderr, "Usage : %s filename\n", argv[0]);
		exit(1);
	}
	search_directory(argv[1]);
	exit(0);
}
