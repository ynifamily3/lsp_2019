#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

void print_file_type(struct stat *statbuf) {
	char *str;

	if (S_ISREG(statbuf->st_mode))
		str = "regular";
	else if (S_ISDIR(statbuf->st_mode))
		str = "directory";
	else if (S_ISCHR(statbuf->st_mode))
		str = "character special";
	else if (S_ISBLK(statbuf->st_mode))
		str = "block special";
	else if (S_ISFIFO(statbuf->st_mode))
		str = "FIFO";
	else if (S_ISLNK(statbuf->st_mode))
		str = "symbolic link";
	else if (S_ISSOCK(statbuf->st_mode))
		str = "socket";
	else
		str = "unknown mode";

	printf("%s\n", str);
}

int main(int argc, char *argv[])
{
	struct stat statbuf;
	char *fname = "symlink";

	if (lstat(fname, &statbuf) < 0) {
		fprintf(stderr, "error\n");
		exit(1);
	}
	print_file_type(&statbuf);
	exit(0);
}
