#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "debug.h"
#include "lex.h"

extern int brace_stack;
extern int lexPattern_number_of_line;
int is_comment; // 0 코멘트 아님 1 한줄 코멘트 중 2 여러줄 코멘트 중
int in_for_if_code; // for이나 if 내에선 ;가 있어도 어휘분석단위를 나누지 않는다.
int parentheses_stack; // ( ) 스택을 조사한다. (in_for_if_code) 있을 시

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
        else if (isalpha(lV->LEX_nextChar) || lV->LEX_nextChar == '_' || lV->LEX_nextChar == '\n') // else \n if 처리하려 넣은 \n 구문이.. ㅠ => else를 제외한 나머지는 토큰내 \n을 무시하도록 한다.
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

void lex(_lexPattern **pattern, const char *_inText)
{
    is_comment = 0;
    in_for_if_code = 0; // for 32이나 if 33 내에선 ;가 있어도 어휘분석단위를 나누지 않는다.
    parentheses_stack = 0; // (, ) 스택을 조사한다. (in_for_if_code) 있을 시
    _lexV lexMemory; // 전역 변수 대신 다음 구조체 사용
    _lexV *lV = &lexMemory;
    lV->inText = _inText;
    for (int i = 0; i < MAX_LINE_PER_FILE; i++) {
        pattern[i]->pattern_length = 0; // 이게 필요할까?
    }
    lV->LEX_inText_pointer = 0;
    getChar(lV);
    do {
        lex_analysis(pattern, lV);
    } while(lV->LEX_nextToken != EOF);
}

void lex_analysis(_lexPattern **pattern, _lexV *lV)
{
    lV->LEX_lex_length = 0;
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
        int check_unary = 0;
        if(lV->LEX_nextChar == '+' || lV->LEX_nextChar == '-')
            check_unary = 1;
        // 만약에 + -가 오면 단항일수도 있음. 다음에 숫자라면 오퍼레이터로써 처리를 안해 줘야하는 것이 있음
        /////

        addChar(lV);
        getChar(lV);

        if (check_unary == 1 && lV->LEX_charClass == DIGIT) {
            addChar(lV);
            getChar(lV);
            while (lV->LEX_charClass == DIGIT) {
                addChar(lV);
                getChar(lV);
            }
            lV->LEX_nextToken = INT_LIT;
            break;
        }

        int keepGoing = 0;
        switch (lV->LEX_nextChar) {
            // ; 다음에는 연산자 쓰지마.. ;/*
            // 마지막 '/'은 주석의 시작을 알릴 수도 있으므로 살펴본다.
            // case */
            case '=': case '+' : case '-' : case ']' :  case '*': case '/':
            //if (lV->LEX_nextChar == '/') specialCase = 1;
            keepGoing = 1;
            if (lV->LEX_inText_pointer-2 >= 0 && (lV->inText[lV->LEX_inText_pointer-2] == '[' || lV->inText[lV->LEX_inText_pointer-2] == ';')) {
                keepGoing = 0;
            }
            // =  뒤에 =가 아닌 다른 연산자 오면 끊음
            if (lV->inText[lV->LEX_inText_pointer-2] == '=' && lV->LEX_nextChar != '=') {
                keepGoing = 0;
            }
        }
        
        while (lV->LEX_charClass == OPERATOR && keepGoing == 1) {
            addChar(lV);
            getChar(lV);
            keepGoing = 0;
            switch (lV->LEX_nextChar) {
                case '=': case '+' : case '-' : case ']' : case '/': case '*':
                keepGoing = 1;
                if( lV->LEX_nextChar == '*' || lV->LEX_nextChar == '/') keepGoing = 0;
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
            lookup_keyword(lV);
        }
    } else if (lV->LEX_nextToken == IDENTFIER) {
        lookup_keyword(lV);
    }
    if (lV->LEX_nextToken != EOF) {
        pattern[lexPattern_number_of_line]->pattern[pattern[lexPattern_number_of_line]->pattern_length] = lV->LEX_nextToken;
        // fprintf(stderr, "[%s]", lV->LEX_lexeme);
        int newline_test = pattern[lexPattern_number_of_line]->pattern[pattern[lexPattern_number_of_line]->pattern_length];
        
        if (newline_test == 103) {
            ++parentheses_stack;
        } else if (newline_test == 104) {
            --parentheses_stack;
        } 

        if(in_for_if_code == 1 && parentheses_stack == 0) {
            // 스택이 끝났으므로 뉴라인을 해줘도 된다.
            in_for_if_code = 0; // 이건 없앤다.
            newline_test = 13; // 임의로 if가 참이 되는 값으로 했다.
        }
        // 세미콜론이 왔으나 for, if문 안인 경우
        if (newline_test == 109 && parentheses_stack != 0) {
            newline_test = -1; // 임의로 if가 거짓이 되는 값으로 했다.
        }

        if (newline_test == 32 || newline_test == 33) {
            in_for_if_code = 1;
        }
        
        // 나중에는 뉴라인 대신 line_count 증가시키고 나중에 line 별 패턴 분석하면 될 듯
        //  pattern->pattern[pattern->pattern_length]
        if(pattern[lexPattern_number_of_line]->pattern[pattern[lexPattern_number_of_line]->pattern_length] != COMMENT)
            strncpy(pattern[lexPattern_number_of_line]->buffer[pattern[lexPattern_number_of_line]->pattern_length++], lV->LEX_lexeme, LEX_SIZE);

        // for code 32 if code 33 이다. 이 안쪽으로 들어오면 ;(109)는 자르지 않는다.
        // 주석 (13) 세미콜론 (109) 중괄호 열기 (105) 중괄호 닫기 (106)
        if (newline_test == 13 || newline_test == 109 || newline_test == 105 || newline_test == 106) {
            // fprintf(stderr, "\n");
            ++lexPattern_number_of_line;
        }
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
        while (is_comment == 1) {
            if(lV->LEX_nextChar != '\n') {
                addChar(lV);
                getChar(lV);
            } else {
                // 비어있는 //주석이면 끝내줌
                is_comment = 0;
            }
        }
        while (is_comment == 2 && lV->LEX_nextChar != 0) { // 두번째 조건은 주석을 열고 닫지 않을 경우 처리
            char endComment1 = lV->LEX_nextChar;
            addChar(lV);
            getChar(lV);
            char endComment2 = lV->LEX_nextChar;
            if (endComment1 == '*' && endComment2 == '/') {
                is_comment = 0; // 이거 빼먹어서 /**/ 다음 문장에 버그가 걸림
                addChar(lV);
                getChar(lV);
                break;
            }
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