#ifndef PARSE_TREE
#define PARSE_TREE
#define NUMBER_OF_OPERATORS 31

int priority[NUMBER_OF_OPERATORS] = {0, 0, 8, 8, 7, 7, 6, 6, 2, 1, 0, 0, 0, 0, 0, 0, 99, 10, 10, 10, 9, 9, 7, 7, 5, 4, 3, 0, 99, 99, -1};
char operator[NUMBER_OF_OPERATORS][4] = {"<<=", ">>=","<<", ">>","<=", ">=",  "==", "!=", "&&", "||", "+=", "*=", "/=", "%=", "&=", "|=", "->", "*", "/", "%", "+", "-",  "<",  ">", "&", "^", "|",  "=", "(", ")", ","};
bool operator_swapable[NUMBER_OF_OPERATORS] = {false, false, false, false, false, false, true, true, true, true, false, false, false, false, false, false, false, true, false, false, true, false, false, false, true, true, true, false, false, false, false};
int operator_convable[NUMBER_OF_OPERATORS] = {-1,-1,-1,-1,-1,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 22,-1,-1,-1,-1,-1,-1,-1};
char operator_onebyte[NUMBER_OF_OPERATORS] = {(char)1, (char)2, (char)3, (char)4, (char)5, (char)6, (char)7, (char)8, (char)9, (char)10, (char)11, (char)12, (char)13, (char)14, (char)15, (char)16, (char)17, '*', '/', '%', '+', '-',  '<',  '>', '&', '^', '|', '=', '(', ')', ','};

typedef struct { 
	char *stream;
	int number_of_tokens; // 토큰 갯수
	char *tokens[1024]; // 각 토큰들의 시작 위치 포인터
	int tokens_length[1024]; // 각 토큰들의 길이 (연산자는 무조건 1일 것이다..)
	bool is_operator[1024]; // 토큰이 연산자인 경우에는 true, 아니면 false
} _container;

typedef struct {
	char elem[1024];
	bool is_operator;
	bool is_pure_swapable;
} _token;

typedef struct {
	int top;
	_token *stack_data[301]; // 토큰들을 담는다...
} stackString;

typedef struct {
	char **tokens;
	bool is_operator[1024];
	int length;
} _tokens;
#endif