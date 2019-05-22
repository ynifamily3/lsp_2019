#include <stdio.h>
#include <stdlib.h>
#include "ssu_runtime.h"

#define BUFFER_MAX 256

int main(void)
{
	struct timeval start, end;
	char command[BUFFER_MAX];
	char *prompt = "Prompt>>";
	
	gettimeofday(&start, NULL);

	while(1) {
		fputs(prompt, stdout);

		if (fgets(command, sizeof(command), stdin) == NULL)
			break;

		system(command);
	}

	fprintf(stdout, "Good bye...\n");
	fflush(stdout);
	gettimeofday(&end, NULL);
	ssu_runtime(&start, &end);
	exit(0);
}

