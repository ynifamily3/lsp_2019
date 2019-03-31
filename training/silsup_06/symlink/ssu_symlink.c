#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ssu_runtime.h"

int main(int argc, char *argv[]) {
	struct timeval begin, end;

	gettimeofday(&begin, NULL);

	if (argc != 3) {
		fprintf(stderr, "usage: %s <actualname> <symname>\n", argv[0]);
		exit(1);
	}

	if (symlink(argv[1], argv[2]) < 0) {
		fprintf(stderr, "symlink error\n");
		exit(1);
	}
	else
		printf("symlink: %s -> %s\n", argv[2], argv[1]);

	gettimeofday(&end, NULL);
	ssu_runtime(&begin, &end);

	exit(0);
}

