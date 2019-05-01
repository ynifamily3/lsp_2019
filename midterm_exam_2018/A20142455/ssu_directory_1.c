#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define PATH_SIZE MAXNAMLEN

int main(int argc, char *argv[])
{
	DIR *dirp;
	struct dirent *dentry;
	struct stat statbuf;
	char filename[PATH_SIZE + 1];

	if (argc != 2) {
		fprintf(stderr, "usage : %s <directory_name>\n", argv[0]);
		exit(1);
	}

	if ((dirp = opendir(argv[1])) == NULL || chdir(argv[1]) == -1) {
		fprintf(stderr, "opendir, chdir error for %s\n", argv[1]);
		exit(1);
	}

	while ( (dentry = readdir(dirp)) != NULL) {
		if (dentry->d_ino == 0) continue;
		memcpy(filename, dentry->d_name, PATH_SIZE);
		if (stat(filename, &statbuf) < 0) {
			fprintf(stderr, "stat error for %s\n", filename);
			exit(1);
		}
		if (S_ISREG(statbuf.st_mode)) {
			printf("%-14s %ld\n", filename, statbuf.st_size);
		}
		else {
			printf("%-14s\n", filename);
		}
	}

	closedir(dirp);
	exit(0);
}
