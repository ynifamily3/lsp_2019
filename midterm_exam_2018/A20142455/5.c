#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

void print_file_type(struct stat *statbuf) {
	char *str;
	str = "unknown";
	if ( S_ISSOCK(statbuf->st_mode))
		str = "socket";
	else if ( S_ISREG(statbuf->st_mode)) 
		str = "regular";
	else if ( S_ISDIR(statbuf->st_mode))
		str = "directory";
	else if ( S_ISCHR(statbuf->st_mode))
		str = "character special";
	else if ( S_ISBLK(statbuf->st_mode))
		str = "block special";
	else if ( S_ISFIFO(statbuf->st_mode))
		str = "fifo";
	else if ( S_ISLNK(statbuf->st_mode))
		str = "symlink";
	printf("%s\n", str);

}

int main(int argc, char *argv[])
{
	int i;
	struct stat statbuf;
	for (i = 1; i < argc; i++) {
		if (lstat(argv[i], &statbuf) < 0) {
			fprintf(stderr, "lstat error for %s\n", argv[i]);
			exit(1);
		}
		print_file_type(&statbuf);
	}
	exit(0);
}
