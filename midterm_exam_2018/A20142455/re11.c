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

	if ( lstat(pathname, &statbuf) < 0) {
		fprintf(stderr, "lstat error\n");
		return 0;
	}

	if ( !S_ISDIR(statbuf.st_mode)) {
		// not directory
		// do grep command
		printf("%s : \n", pathname);
		strcpy(command, grep_cmd);
		strcat(command, " ");
		strcat(command, pathname);
		system(command);
		return 0;
	}

	ptr = pathname + strlen(pathname);
	*ptr++ = '/';
	*ptr = '\0';

	if((dp = opendir(pathname)) == NULL) {
		fprintf(stderr, "opendir error\n");
		return 0;
	}

	while ((dirp = readdir(dp)) != NULL) {
		if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
			continue;
		}
		strcpy(ptr, dirp->d_name);
		ssu_do_grep();
	}
	ptr[-1] = '\0';
	closedir(dp);
	return 0;



	

}

void ssu_make_grep(int argc, char *argv[]) {
	int i;
	strcpy(grep_cmd, "grep ");
	for (i = 1; i < argc - 1; i++) {
		strcat(grep_cmd, argv[i]);
		strcat(grep_cmd, " ");
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s <-CVbchilnsvwx> <-num> <-A num> <-B num> <-f file> <-e> expr <directory>\n", argv[0]);
		exit(1);
	}
	if (pathmax == 0) {
		if ( (pathmax = pathconf("/", _PC_PATH_MAX)) < 0) {
			pathmax = MAX_PATH_GUESSED;
		}
		else
			pathmax++;
	}
	pathname = (char *)malloc(sizeof(char) * (pathmax+1));
	strcpy(pathname, argv[argc-1]);
	ssu_make_grep(argc, argv);
	ssu_do_grep();
	exit(0);
}
