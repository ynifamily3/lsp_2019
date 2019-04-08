#include <stdio.h>
#include <stdlib.h>

#include "lex.h"

int main(void)
{
	char input[100] = "public static void main(String[] args){ 10  \"Lucky stars\" ";
	printf("Hello world!!\n");
	lex(input);
	exit(0);
}

