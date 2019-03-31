#ifndef PARSE_TREE
#define PARSE_TREE

#define NUMBER_OF_OPERATORS 31

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

// make parse tree
char *mpt(char *string);

#endif