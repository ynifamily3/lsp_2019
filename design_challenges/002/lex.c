#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "debug.h"
#include "lex.h"

int LEX_is_in_comment = 0; // multi line comment

/*
LEX 분석 참고문헌 : Concepts of Programming Languages Tenth Edition. Robert W. Sebesta
193 page 소스코드를 고쳐서 썼습니다.
*/
const char *LEX_keywords[NUMBER_OF_KEYWORDS] = {
    "import", "public", "class", "static", "void",
    "String", "Scanner", "new", "System", "out",
    "int", "nextInt", "for", "if", "else", "return", "final", "File",
     "throws", "IOException", "FileWriter", "printf"
};
const char *LEX_operators[NUMBER_OF_OPERATORS] = {
    ".", "[", "]", "(" ,")", "{", "}", "%", "\"", ";",
    "==", "<=", ">=", "+=", "++", "--", "!=", "=", "[]",
    ",", "//", "/*", "*/"
};

void remove_blank(_lexV *lV) {
    while (isspace(lV->LEX_nextChar)) getChar(lV);
}

int process_string_literals(_lexV *lV) {
    if (lV->LEX_nextChar == '"') {
        addChar(lV); // 시작하는 따옴표
        getChar(lV);
        while (lV->LEX_nextChar != '"') {
            addChar(lV); // 내용물
            getChar(lV);
        }
        addChar(lV); // 끝나는 따옴표
        getChar(lV);
        lV->LEX_nextToken = STR_LIT;
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
        case '(' : case ')' : case ',' : case '/': case '*':
        return 1;
        break;
    }
    return 0;
}

/* 한 바이트를 읽어서  */
void getChar(_lexV *lV)
{
    lV->LEX_nextChar = lV->inText[lV->LEX_inText_pointer++];
    if (lV->LEX_nextChar != '\0' && lV->LEX_nextChar != '\n') {
        if (isalpha(lV->LEX_nextChar) || lV->LEX_nextChar == '_')
            lV->LEX_charClass = LETTER;
        else if (isdigit(lV->LEX_nextChar))
            lV->LEX_charClass = DIGIT;
        else if (isoperator(lV->LEX_nextChar))
            lV->LEX_charClass = OPERATOR;
        else lV->LEX_charClass = UNKNOWN; // LETTER로 바꿨는데 import java 를 하나로 인식(버그) 그냥 언노운 할래..
    }
    else
        lV->LEX_charClass = EOF;
}

void addChar(_lexV *lV)
{
    if (lV->LEX_lex_length <= LEX_SIZE - 1) {
        lV->LEX_lexeme[lV->LEX_lex_length++] = lV->LEX_nextChar;
        lV->LEX_lexeme[lV->LEX_lex_length] = '\0';
    }
    else {
        fprintf(stderr, "Error - lexeme is too long \n");
        exit(1);
    }
}

void lex(_lexPattern *pattern, const char *_inText)
{
    _lexV lexMemory; // 전역 변수 대신 다음 구조체 사용
    _lexV *lV = &lexMemory;
    lexMemory.is_oneline_comment = 0; // 일단 추가해 줌
    lV->inText = _inText;
    pattern->pattern_length = 0;
    lV->LEX_inText_pointer = 0;
    getChar(lV);
    do {
        lex_analysis(pattern, lV);
    } while(lV->LEX_nextToken != EOF);
}

void lex_analysis(_lexPattern *pattern, _lexV *lV)
{
    lV->LEX_lex_length = 0;
    if (lV->is_oneline_comment == 1) {
        // 뒤에 있는 모든 코멘트 끝까지 복사 후 EOF종결
        fprintf(stderr, "//%s\n", lV->inText + lV->LEX_inText_pointer - 1);
        pattern->pattern[pattern->pattern_length] = lV->LEX_nextToken;
        strncpy(pattern->buffer[pattern->pattern_length++], lV->inText + lV->LEX_inText_pointer -1, LEX_SIZE);
        lV->LEX_nextToken = EOF;
        return;
    }
    // 멀티라인 주석은 생략해본다.
    remove_blank(lV);
    int is_str_lit = process_string_literals(lV); // 문자열은 따로 분리해줌
    if(!is_str_lit) // str_lit가 없었을 경우 switch문 실행
    switch (lV->LEX_charClass) {
        case LETTER:
        addChar(lV);
        getChar(lV);
        while (lV->LEX_charClass == LETTER || lV->LEX_charClass == DIGIT) {
            //DBGMSG("... %c", lV->LEX_nextChar); // import java 가 한 번 에 읽 혀 !
            //getchar();
            addChar(lV);
            getChar(lV);
        }
        lV->LEX_nextToken = IDENTFIER;
        break;

        case DIGIT:
        addChar(lV);
        getChar(lV);
        while (lV->LEX_charClass == DIGIT) {
            addChar(lV);
            getChar(lV);
        }
        lV->LEX_nextToken = INT_LIT;
        break;

        case OPERATOR:
        addChar(lV);
        getChar(lV);
        int keepGoing = 0;
        switch (lV->LEX_nextChar) {
            // 마지막 '/'은 주석의 시작을 알릴 수도 있으므로 살펴본다.
            // case */
            case '=': case '+' : case '-' : case ']' : case '/': case '*':
            keepGoing = 1;
        }
        // 아닌 이상 while 문을 수행하지 않아도 된다.
        while (lV->LEX_charClass == OPERATOR && keepGoing == 1) {
            addChar(lV);
            getChar(lV);
            keepGoing = 0;
            switch (lV->LEX_nextChar) {
                case '=': case '+' : case '-' : case ']' : case '/': case '*':
                keepGoing = 1;
            }
        }
        lV->LEX_nextToken = OPERATOR;
        break;

        case UNKNOWN:
        // 설 마 ㅠㅠ 
        getChar(lV);
        break;

        case EOF:
        lV->LEX_nextToken = EOF;
        break;
    }
    // 키워드인지 아닌지, 연산자인지 아닌지 조사
    if (lV->LEX_nextToken == OPERATOR) {
        lookup_operator(lV);
    } else if (lV->LEX_nextToken == IDENTFIER) {
        lookup_keyword(lV);
    }
    if (lV->LEX_nextToken != EOF) {
        pattern->pattern[pattern->pattern_length] = lV->LEX_nextToken;
        strncpy(pattern->buffer[pattern->pattern_length++], lV->LEX_lexeme, LEX_SIZE);
    }
}

void lookup_operator(_lexV *lV)
{
    for (int i = 0; i < NUMBER_OF_OPERATORS; i++) {
        if (strcmp(lV->LEX_lexeme, LEX_operators[i]) == 0) {
            lV->LEX_nextToken = i + 100;
            // 주석인지 아닌지 추가...
            if (lV->LEX_nextToken == ONE_LINE_COMMENT || lV->LEX_nextToken == MULTI_LINE_COMMENT_OPEN) {
                // '//' 주석이다.
                fprintf(stderr, "한 줄 주석 출연합니다..\n");
                lV->is_oneline_comment = 1;
            }
            return;
        }
    }
}

void lookup_keyword(_lexV *lV)
{
    for (int i = 0; i < NUMBER_OF_KEYWORDS; i++) {
        if (strcmp(lV->LEX_lexeme, LEX_keywords[i]) == 0) {
            // DBGMSG("찾는키워드 : %s 입력된 : %s", LEX_keywords[i], lV->LEX_lexeme);
            lV->LEX_nextToken = i + 20;
            return;
        }
    }
}