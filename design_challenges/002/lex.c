#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "debug.h"
#include "lex.h"

extern int brace_stack;
int is_comment; // 0 코멘트 아님 1 한줄 코멘트 중 2 여러줄 코멘트 중

/*
LEX 분석 참고문헌 : Concepts of Programming Languages Tenth Edition. Robert W. Sebesta
193 page 소스코드를 고쳐서 썼습니다.
*/
const char *LEX_keywords[NUMBER_OF_KEYWORDS] = {
    "import", "public", "class", "static", "void",
    "String", "Scanner", "new", "System", "out",
    "int", "nextInt", "for", "if", "else",
    "return", "final", "File", "throws", "IOException",
    "FileWriter", "printf","//", "/*", "*/",
    "else\n" // else\nif 같은 문장을 처리하기 위함 (else if != else\nif)
};
const char *LEX_operators[NUMBER_OF_OPERATORS] = {
    ".", "[", "]", "(" ,")",
    "{", "}", "%", "\"", ";",
    "==", "<=", ">=", "+=", "++",
    "--", "!=", "=", "[]", ","
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
    if (lV->LEX_nextChar != '\0') {
        if (lV->LEX_nextChar == '\n' && is_comment == 1) {
            //fprintf(stderr, "한 줄 주석 깨짐\n");
            is_comment = 0;
        } // comment 해제
        if (lV->LEX_nextChar == '*' && lV->inText[lV->LEX_inText_pointer] == '/' && is_comment == 2) {
            // fprintf(stderr, "멀티라인 주석 깨짐 의심\n");
            addChar(lV);
            getChar(lV);
            addChar(lV);
            getChar(lV); // */ breaks
            is_comment = 0;
        }


        if ( is_comment == 1 ) {
            //fprintf(stderr, "주석 유지..\n");
            lV->LEX_charClass = COMMENT;
        } 
        else if (isalpha(lV->LEX_nextChar) || lV->LEX_nextChar == '_' || lV->LEX_nextChar == '\n')
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
    lV->inText = _inText;
    pattern->pattern_length = 0;
    lV->LEX_inText_pointer = 0;
    getChar(lV);
    fprintf(stderr, "렉스 분석 시작\n");
    do {
        lex_analysis(pattern, lV);
    } while(lV->LEX_nextToken != EOF);
}

void lex_analysis(_lexPattern *pattern, _lexV *lV)
{
    lV->LEX_lex_length = 0;
    // 멀티라인 주석은 생략해본다.
    remove_blank(lV);
    int is_str_lit = process_string_literals(lV); // 문자열은 따로 분리해줌
    if(!is_str_lit) // str_lit가 없었을 경우 switch문 실행
    switch (lV->LEX_charClass) {
        case LETTER:
        addChar(lV);
        getChar(lV);
        while (lV->LEX_charClass == LETTER || lV->LEX_charClass == DIGIT) {
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
        // 여닫는 중괄호 계층 스택 조작
        if (lV->LEX_nextChar == '{') {
            ++brace_stack;
        } else if (lV->LEX_nextChar == '}') {
            --brace_stack;
        }
        addChar(lV);
        getChar(lV);
        int keepGoing = 0;
        int specialCase = 0; // //나 /* 같은 케이스
        switch (lV->LEX_nextChar) {
            // ; 다음에는 연산자 쓰지마.. ;/*
            // 마지막 '/'은 주석의 시작을 알릴 수도 있으므로 살펴본다.
            // case */
            case '=': case '+' : case '-' : case ']' :  case '*': case '/':
            //if (lV->LEX_nextChar == '/') specialCase = 1;
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
                if( lV->LEX_nextChar == '*' || lV->LEX_nextChar == '/') keepGoing = 0;
                //if ((lV->LEX_nextChar == '/' || lV->LEX_nextChar == '*') && specialCase == 1) {
                    //fprintf(stderr, "주석엶\n");
                    //keepGoing = 0;}
            }
        }
        lV->LEX_nextToken = OPERATOR;
        break;

        case UNKNOWN:
        getChar(lV);
        break;

        case EOF:
        lV->LEX_nextToken = EOF;
        break;
    }
    // 키워드인지 아닌지, 연산자인지 아닌지 조사
    if (lV->LEX_nextToken == OPERATOR) {
        lookup_operator(lV);
        if (lV->LEX_lexeme[0] == '/') {
            //DBGMSG("주석 의심 %s", lV->LEX_lexeme);
            lookup_keyword(lV);
        }
    } else if (lV->LEX_nextToken == IDENTFIER) {
        lookup_keyword(lV);
    }
    if (lV->LEX_nextToken != EOF) {
        pattern->pattern[pattern->pattern_length] = lV->LEX_nextToken;
        fprintf(stderr, "[%s] %d\n", lV->LEX_lexeme, pattern->pattern[pattern->pattern_length]);
        strncpy(pattern->buffer[pattern->pattern_length++], lV->LEX_lexeme, LEX_SIZE);
    }
}

void lookup_operator(_lexV *lV)
{
    for (int i = 0; i < NUMBER_OF_OPERATORS; i++) {
        if (strcmp(lV->LEX_lexeme, LEX_operators[i]) == 0) {
            lV->LEX_nextToken = i + 100;
            return;
        }
    }
}

void lookup_keyword(_lexV *lV)
{
    // 키워드가 //로 시작하면 한줄 주석 /*로 시작하면 여러줄 주석
    if (lV->LEX_lex_length >= 2 && lV->LEX_lexeme[0] == '/' && lV->LEX_lexeme[1] == '/') {
        is_comment = 1;
    } else if (lV->LEX_lex_length >= 2 && lV->LEX_lexeme[0] == '/' && lV->LEX_lexeme[1] == '*') {
        is_comment = 2;
    }
    if (is_comment == 1 || is_comment == 2) {
        // 계속 getchar?
        while ((is_comment == 1 && lV->LEX_nextChar != '\n')) {
            addChar(lV);
            getChar(lV);
        }
        while ((is_comment == 2)) {
            addChar(lV);
            getChar(lV);
        }
        lV->LEX_nextToken = COMMENT;
        return;
    }
    for (int i = 0; i < NUMBER_OF_KEYWORDS; i++) {
        if (strcmp(lV->LEX_lexeme, LEX_keywords[i]) == 0) {
            lV->LEX_nextToken = i + 20;
            return;
        }
    }
}