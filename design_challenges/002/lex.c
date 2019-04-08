#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"

#define NUMBER_OF_KEYWORDS 21
#define NUMBER_OF_OPERATORS 20

/*
LEX 분석 참고문헌 : Concepts of Programming Languages Tenth Edition. Robert W. Sebesta
193 page 소스코드를 고쳐서 썼습니다.
*/
const char *LEX_keywords[NUMBER_OF_KEYWORDS] = {
	"import", "public", "class", "static", "void",
	"String", "Scanner", "new", "System.in", "System.out.printf",
	"int", "nextInt", "for", "if", "else", "return", "final", "File",
	 "throws", "IOException", "FileWriter"
};
const char *LEX_operators[NUMBER_OF_OPERATORS] = {
	".", "[", "]", "(" ,")", "{", "}", "%", "\"", ";",
	"==", "<=", ">=", "+=", "++", "--", "!=", "=", "[]", ","
};

void remove_blank() {
	while (isspace(LEX_nextChar)) getChar();
}

int process_string_literals() {
	if (LEX_nextChar == '"') {
		addChar(); // 시작하는 따옴표
		getChar();
		while (LEX_nextChar != '"') {
			addChar(); // 내용물
			getChar();
		}
		addChar(); // 끝나는 따옴표
		getChar();
		LEX_nextToken = STR_LIT;
		return 1;
	}
	return 0;
}

int isoperator(char in)
{
	switch(in)
	{
		case '.': case '[': case ']': case '{' : case '}' :
		case '%' : case '\"': case ';': case '=': case '<' :
		case '>' : case '+' : case '-' : case '!' :
		case '(' : case ')' : case ',' :
		return 1;
		break;
	}
	return 0;
}

/* 한 바이트를 읽어서  */
void getChar()
{
	LEX_nextChar = inText[LEX_inText_pointer++];
	if (LEX_nextChar != '\0' && LEX_nextChar != '\n') {
		if (isalpha(LEX_nextChar))
			LEX_charClass = LETTER;
		else if (isdigit(LEX_nextChar))
			LEX_charClass = DIGIT;
		else if (isoperator(LEX_nextChar))
			LEX_charClass = OPERATOR;
		else LEX_charClass = UNKNOWN;
	}
	else
		LEX_charClass = EOF;
}

void addChar()
{
	if (LEX_lex_length <= 29) {
		LEX_lexeme[LEX_lex_length++] = LEX_nextChar;
		LEX_lexeme[LEX_lex_length] = '\0';
	}
	else {
		fprintf(stderr, "Error - lexeme is too long \n");
		exit(1);
	}
}

void lex(const char *_inText)
{
	inText = _inText;
	LEX_inText_pointer = 0;
	getChar();
	do {
		lex_analysis();
	} while(LEX_nextToken != EOF);
	
}

void lex_analysis()
{
	LEX_lex_length = 0;
	remove_blank();
	int is_str_lit = process_string_literals(); // 문자열은 따로 분리해줌
	if(!is_str_lit) // str_lit가 없었을 경우 switch문 실행
	switch (LEX_charClass) {
		case LETTER:
		addChar();
		getChar();
		while (LEX_charClass == LETTER || LEX_charClass == DIGIT) {
			addChar();
			getChar();
		}
		LEX_nextToken = IDENTFIER;
		break;

		case DIGIT:
		addChar();
		getChar();
		while (LEX_charClass == DIGIT) {
			addChar();
			getChar();
		}
		LEX_nextToken = INT_LIT;
		break;

		case OPERATOR:
		addChar();
		getChar();
		int keepGoing = 0;
		switch (LEX_nextChar) {
			case '=': case '+' : case '-' : case ']' :
			keepGoing = 1;
		}
		// 아닌 이상 while 문을 수행하지 않아도 된다.
		while (LEX_charClass == OPERATOR && keepGoing == 1) {
			addChar();
			getChar();
			keepGoing = 0;
			switch (LEX_nextChar) {
				case '=': case '+' : case '-' : case ']' :
				keepGoing = 1;
			}
		}
		LEX_nextToken = OPERATOR;
		break;

		case UNKNOWN:
		getChar();
		break;

		case EOF:
		LEX_lexeme[0] = '<';
		LEX_lexeme[1] = 'E';
		LEX_lexeme[2] = 'O';
		LEX_lexeme[3] = 'F';
		LEX_lexeme[4] = '>';
		LEX_lexeme[5] = '\0';
		LEX_nextToken = EOF;
		break;
	}
	// 키워드인지 아닌지, 연산자인지 아닌지 조사
	if (LEX_nextToken == OPERATOR) {
		lookup_operator();
	} else if (LEX_nextToken == IDENTFIER) {
		lookup_keyword();
	}
	printf("토큰의 종류 : %d\t분해된 렉심 : %s\n", LEX_nextToken, LEX_lexeme);
}

void lookup_operator()
{
	for (int i = 0; i < NUMBER_OF_OPERATORS; i++) {
		if (strcmp(LEX_lexeme, LEX_operators[i]) == 0) {
			LEX_nextToken = i + 100;
			return;
		}
	}
}

void lookup_keyword()
{
	for (int i = 0; i < NUMBER_OF_KEYWORDS; i++) {
		if (strcmp(LEX_lexeme, LEX_keywords[i]) == 0) {
			LEX_nextToken = i + 20;
			return;
		}
	}
}