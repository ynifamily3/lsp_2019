#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "usage %s <oldfile> <newfile>\n", argv[0]);
		exit(1);
	}

	if (link(argv[1], argv[2]) == -1) {
		fprintf(stderr, "link error for %s -> %s\n", argv[1], argv[2]);
		exit(1);
	}

	if (unlink(argv[1]) == -1) {
		fprintf(stderr, "unlink error for %s\n", argv[1]);
		exit(1);
	}


	exit(0);
}
