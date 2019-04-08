#include <stdio.h>
#include <stdlib.h>

#include "lex.h"

int main(void)
{
	char input[100] = "Scanner scn = new Scanner(System.in);";
	printf("Hello world!!\n");
	lex(input);
	exit(0);
}

