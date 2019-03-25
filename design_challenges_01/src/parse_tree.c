#include <stdio.h>
#include <string.h>
#include <ctype.h>
// #include "parse_tree.h"

// normalize with ....
char keywords[][] = {"char **", "char *", "char", "int **", "int *", "int", "extern", "class", "for", "while", "else", "if", "#include", "struct"};
char constants[][] = {"O_RDWR", "O_APPEND", "O_TRUNC", "O_EXCL", "O_RDONLY", "O_WRONLY"};

void normalize_statement(char *text)
{
    // " " 안 띄어쓰기 빼고 모두 붙여쓴다.
    /*    */
	int in_double_quotes = 0; // 2중이면 잘 안되긴 하는데 이정도면 충분할 것 같다.
	int i, j;
	for (i = 0, j = 0; text[i] != 0; i++,j++) {
		if ( (text[i] == ' ' && in_double_quotes) || !isspace(text[i]) ) {
			// lower normalize
			char t = text[i];
			if(!in_double_quotes && t == '\"') in_double_quotes = 1;
			else if (in_double_quotes && t == '\"') in_double_quotes = 0;
			if (t >= 'A' && t <= 'Z') t += 32;
			text[j] = t;
		}
		else
			j--;
	}
	text[j] = '\0';
}

int main(void)
{
	char buf[1000] = "char *str=\"Hello World!!\\n\";";
	normalize_statement(buf);
	printf("%s\n", buf);
	return 0;
}