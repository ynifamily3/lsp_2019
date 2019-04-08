#ifndef LEX
#define LEX
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/*
LEX 분석 참고문헌 : Concepts of Programming Languages Tenth Edition. Robert W. Sebesta
193 page 소스코드를 고쳐서 썼습니다.
*/

/* 문자 종류 정의 */
#define LETTER 0 // 문자
#define DIGIT 1 // 숫자
#define OPERATOR 2 // 연산자(조각)
#define UNKNOWN 9999 // 모르는 것들

/* 문자열 종류 정의 */
#define INT_LIT 10 // 숫자열
#define IDENTFIER 11 // (키워드, 연산자, 코드) 아닌 것 => 밑에 세부적으로 정의함
#define STR_LIT 12 // " " 안에 있는 문장

/* 키워드, 자바 내부 메서드 정의 */
#define IMPORT_CODE 20
#define PUBLIC_CODE 21
#define CLASS_CODE 22
#define STATIC_CODE 23
#define VOID_CODE 24
#define STRING_CODE 25
#define SCANNER_CODE 26
#define NEW_CODE 27
#define SYSIN_CODE 28
#define SYSOUT_CODE 29
#define INT_CODE 30
#define NEXTINT_CODE 31
#define FOR_CODE 32
#define IF_CODE 33
#define ELSE_CODE 34
#define RETURN_CODE 35
#define FINAL_CODE 36
#define FILE_CODE 37
#define THROWS_CODE 38
#define IOEXCEPTION_CODE 39
#define FILEWRITER_CODE 40

/* 오퍼레이션 정의 */
#define DOT_OP 100
#define SQUARE_BRACKET_LEFT_OP 101
#define SQUARE_BRACKET_RIGHT_OP 102
#define PARENTHESES_LEFT_OP 103
#define PARENTHESES_RIGHT_OP 104
#define BRACE_LEFT_OP 105
#define BRACE_RIGHT_OP 106
#define REMAIN_OP 107
#define STRING_OP 108
#define SEMICOLON_OP 109
#define EQUAL_OP 110
#define LESS_EQUAL_THAN_OP 111
#define MORE_EQUAL_THAN_OP 112
#define PLUS_ASSIGN_OP 113
#define INCREASE_OP 114
#define DECREASE_OP 115
#define NOT_EQUAL_OP 116
#define ASSIGN_OP 117
#define SQUARE_BRACKET_LR_OP 118
#define COMMA_OP 119

const char *inText;

int LEX_token_length;
char LEX_buffer[31][31];

int LEX_charClass;
int LEX_nextToken;
int LEX_nextChar;
int LEX_inText_pointer;
int LEX_lex_length;

int LEX_in_double_quotes;

char LEX_lexeme[31]; // 한 단위 렉심

void lex_analysis();
void addChar();
void getChar();
void lookup_operator();
void lookup_keyword();
void lex(const char *inText);


#endif
