#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* 전역 변수 선언들 */

int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int nextToken;
FILE *in_fp, *fopen();
char keyword_buffer[20]; // char 와 같은 키워드를 판별하기 위한 buffer
int keyLen; // keword length

/* 함수 선언들 */
void addChar();
void getChar();
void getNonBlank();
int lex();

/* 문자 유형들 */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* 토큰 코드들 */
#define INT_LIT 10
#define IDENT 11
#define COLON 19
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define SEMI_COLON 27
#define LEFT_C_PAREN 28
#define RIGHT_C_PAREN 29
#define FOR_CODE 30
#define IF_CODE 31
#define ELSE_CODE 32
#define WHILE_CODE 33
#define DO_CODE 34
#define INT_CODE 35
#define FLOAT_CODE 36
#define SWITCH_CODE 37
#define CASE_CODE 38
#define BREAK_CODE 39
#define DOT_OP 40
#define LESS_THEN 41
#define MORE_THEN 42
#define SQBRCK_OPEN 43
#define SQBRCK_CLOSE 44

int main(void)
{
    if ((in_fp = fopen("front.in", "r")) == NULL)
        printf("Error - cannot open front.in \n");
    else {
        getChar();
        do {
            lex();
        } while (nextToken != EOF);
    }
}

/* addChar - nextChar을 lexeme에 추가하는 함수 */
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0; // null character
    }
    else
        printf("Error - lexeme is too long \n");
}

/* 키워드는 별도의 키워드 버퍼에 저장 */
void addKey() {
    if (keyLen <= 18) {
        keyword_buffer[keyLen++] = nextChar;
        keyword_buffer[keyLen] = 0; // null character
    }
    else
        printf("Error - keyword is too long \n");
}

/* getChar - 입력으로부터 다음 번째 문자를 가져와서 그 문자 유형을 결정하는 함수 */
void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else charClass = UNKNOWN;
    }
    else
        charClass = EOF;
}

void getNonBlank() {
    while (isspace(nextChar)) getChar();
}

void lookupKeyword() {
    // keyword_buffer 검사를 통해 키워드인지 아닌지 추출
    if (strcmp(keyword_buffer, "for") == 0) {
        nextToken = FOR_CODE;
        return;
    }
    if (strcmp(keyword_buffer, "if") == 0) {
        nextToken = IF_CODE;
        return;
    }
    if (strcmp(keyword_buffer, "else") == 0) {
        nextToken = ELSE_CODE;
        return;
    }
    if (strcmp(keyword_buffer, "while") == 0) {
        nextToken = WHILE_CODE;
        return;
    }
    if (strcmp(keyword_buffer, "do") == 0) {
        nextToken = DO_CODE;
        return;
    }
    if (strcmp(keyword_buffer, "int") == 0) {
        nextToken = INT_CODE;
        return;
    }
    if (strcmp(keyword_buffer, "float") == 0) {
        nextToken = FLOAT_CODE;
        return;
    }
    if (strcmp(keyword_buffer, "switch") == 0) {
        nextToken = SWITCH_CODE;
        return;
    }
    if (strcmp(keyword_buffer, "case") == 0) {
        nextToken = CASE_CODE;
        return;
    }
    if (strcmp(keyword_buffer, "break") == 0) {
        nextToken = BREAK_CODE;
        return;
    }
    
}

/* lookup - 연산자와 괄호를 조사하여 그 토큰을 반환하는 함수 */
int lookup (char ch) {
    switch (ch) {
        case '(':
        addChar();
        nextToken = LEFT_PAREN;
        break;

        case ')':
        addChar();
        nextToken = RIGHT_PAREN;
        break;

        case '{':
        addChar();
        nextToken = LEFT_C_PAREN;
        break;

        case '}':
        addChar();
        nextToken = RIGHT_C_PAREN;
        break;
        
        case '+':
        addChar();
        nextToken = ADD_OP;
        break;
        
        case '-':
        addChar();
        nextToken = SUB_OP;
        break;
        
        case '*':
        addChar();
        nextToken = MULT_OP;
        break;
        
        case '/':
        addChar();
        nextToken = DIV_OP;
        break;

        case '=':
        addChar();
        nextToken = ASSIGN_OP;
        break;

        case ';':
        addChar();
        nextToken = SEMI_COLON;
        break;

        case '.':
        addChar();
        nextToken = DOT_OP;
        break;

        case '<':
        addChar();
        nextToken = LESS_THEN;
        break;

        case '>':
        addChar();
        nextToken = MORE_THEN;
        break;
        
        case ':':
        addChar();
        nextToken = COLON;
        break;
        
        case '[':
        addChar();
        nextToken = SQBRCK_OPEN;
        break;
        
        case ']':
        addChar();
        nextToken = SQBRCK_CLOSE;
        break; 
        
        default:
        addChar();
        nextToken = EOF;
        break;
    }
    return nextToken;
}

/* lex - 산술식을 위한 단순 어휘 분석기 */
int lex() {
    keyLen = 0; // 키워드
    lexLen = 0; // 렉스 (키워드포함)
    getNonBlank();
    switch (charClass) {
        case LETTER:
            addKey();
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {

                // 키워드는 LETTER로만 되어 있음.
                if (charClass == LETTER) {
                    addKey();
                } else {
                    keyword_buffer[0] = '$'; // 키워드가 될 수 없는 특수문자로 지정
                    keyword_buffer[1] = '$'; // 키워드가 될 수 없는 특수문자로 지정
                    keyLen = 2; // 길이 초기화
                }
                addChar();
                getChar();
            }
            nextToken = IDENT;
        break;
        case DIGIT:
            addChar();
            getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }
            nextToken = INT_LIT;
        break;
        /* 괄호와 연산자 */
        case UNKNOWN:
            lookup(nextChar);
            getChar();
        break;
        /* EOF */
        case EOF:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = '\0';
        break;
    }
    // printf("키워드? : [%s]\n", keyword_buffer);
    lookupKeyword();
    keyword_buffer[0] = 0; // 문자열을 비워주는 효과
    keyLen = 0; // 길이 초기화
    printf("Next token is : %d,\tNext lexeme is %s\n", nextToken, lexeme);
    return nextToken;
}
