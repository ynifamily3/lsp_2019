#include <stdio.h>
#include <stdlib.h>

#include "lex.h"

int main(void)
{
	_lexPattern pattern;
	char input[100] = "num = scn.nextInt();";
	lex(&pattern, input);
	printf("패턴 : ");
	for (int i = 0; i < pattern.pattern_length; i++) {
		printf("%d ", pattern.pattern[i]);
	}
	printf("\n패턴 문자열 : ");
	for (int i = 0; i < pattern.pattern_length ; i++) {
		printf("%s", pattern.buffer[i]);
	}
	printf("\n");
	exit(0);
}

