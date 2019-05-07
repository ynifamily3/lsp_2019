#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

void print_file_type(struct stat *statbuf) {
	char *str;
	mode_t stm = statbuf->st_mode;
	if ( S_ISREG(stm)) {
		str = "regular";
	}
	else if ( S_ISDIR(stm)) {
		str = "directory";
	}
	else if ( S_ISCHR(stm)) {
		str = "character special";
	}
	else if ( S_ISBLK(stm)) {
		str = "block special";
	}
	else if ( S_ISFIFO(stm)) {
		str = "FIFO";
	}
	else if ( S_ISLNK(stm)) {
		str = "symbolic link";
	}
	else if ( S_ISSOCK(stm)) {
		str = "socket";
	}
	else {
		str = "unknown";
	}
	printf("%s\n", str);
}

int main(int argc, char *argv[])
{
	struct stat statbuf;
	int i;

	for (i = 1; i <argc; i++) {
		if (lstat(argv[i], &statbuf) < 0) {
			fprintf(stderr, "lstat error for %s\n", argv[i]);
		} else {
			print_file_type(&statbuf);
		}
	}

	exit(0);
}

