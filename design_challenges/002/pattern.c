#include <stdio.h>
#include "pattern.h"
#include "header_table.h"
#include "debug.h"

//extern int brace_stack;
int patt_brace_stack = 0;

// extern int number_of_anti_pattern = 0;
// extern char anti_pattern[10][64]; //객체 등은 제거되어야 한다. 예를 들어 st.

// 안티패턴의 정의 예를들어 st. 등 모두 제거되어야 할 것들

int PATT_is_initalized = 0;

const char *C_codes[] = {
    "scanf", "\"%d\"", ",", "&", "*",
    "calloc", "(", ")", "sizeof", " ",
    "int", "void", "const", "{", "char *",
    "FILE *", "fopen", "\"", "w", "a",
    "fprintf", "fflush", "fclose", ";", "if",
    "!", "exit", "1", "\n", "// "
};

int cngCnt[] = {0,0,0,0,0,0,0};
char cngPatt[7][50];

int number_of_anti_pattern = 0;
char anti_pattern[10][64];

int tailreqHeaders = 0;
char reqHeaders[512]; // #include <xxx>\n#include <yyy> 등 (define 도)

char define_name[20]; // #define X Y 에서 X
char define_value[30]; // #define X Y 에서 Y

_patternChanger patternIndex[NUMBER_OF_PATTERNS];

//////////////////////// 각 Java 패턴 ==> C 패턴 컨버전 정의 ////////////////////////
// 패턴 구조체 변환 필요 : 1) 스트릿하게 일치, 2) 자료형 등의 변환 가능 3) ~로 시작하는 문장 이렇게 세 개의 매칭 가능성을 시사해야 됨
void PATT_init()
{
    _patternChanger *pt0 = &patternIndex[0];
    pt0->pattern_type = EXACT;
    pt0->java_pattern_length = 8;
    pt0->c_pattern_length = 8;
    pt0->java_pattern[0] = IDENTFIER;
    pt0->java_pattern[1] = ASSIGN_OP;
    pt0->java_pattern[2] = IDENTFIER;
    pt0->java_pattern[3] = DOT_OP;
    pt0->java_pattern[4] = NEXTINT_CODE;
    pt0->java_pattern[5] = PARENTHESES_LEFT_OP;
    pt0->java_pattern[6] = PARENTHESES_RIGHT_OP;
    pt0->java_pattern[7] = SEMICOLON_OP;
    pt0->c_pattern[0] = 1000;
    pt0->c_pattern[1] = 5;
    pt0->c_pattern[2] = 1001;
    pt0->c_pattern[3] = 1002;
    pt0->c_pattern[4] = 1003;
    pt0->c_pattern[5] = 0;
    pt0->c_pattern[6] = 6;
    pt0->c_pattern[7] = 7;

    _patternChanger *pt1 = &patternIndex[1];
    pt1->pattern_type = EXACT;
    pt1->java_pattern_length = 8;
    pt1->c_pattern_length = 17;
    pt1->java_pattern[0] = IDENTFIER;
    pt1->java_pattern[1] = ASSIGN_OP;
    pt1->java_pattern[2] = NEW_CODE;
    pt1->java_pattern[3] = INT_CODE;
    pt1->java_pattern[4] = SQUARE_BRACKET_LEFT_OP;
    pt1->java_pattern[5] = IDENTFIER; // 혹은 수치 리터럴이 와도 좋다. 어떻게 처리할까.
    pt1->java_pattern[6] = SQUARE_BRACKET_RIGHT_OP;
    pt1->java_pattern[7] = SEMICOLON_OP;
    pt1->c_pattern[0] = 0; // stack
    pt1->c_pattern[1] = 1; // =
    pt1->c_pattern[2] = 1006; // (
    pt1->c_pattern[3] = 3; // int
    pt1->c_pattern[4] = 1009; // " "
    pt1->c_pattern[5] = 1004; // *
    pt1->c_pattern[6] = 1007; // )
    pt1->c_pattern[7] = 1005; // calloc
    pt1->c_pattern[8] = 1006; // (
    pt1->c_pattern[9] = 5; // STACK_SIZE
    pt1->c_pattern[10] = 1002; // ,
    pt1->c_pattern[11] = 1008; // sizeof
    pt1->c_pattern[12] = 1006; // (
    pt1->c_pattern[13] = 3; // int
    pt1->c_pattern[14] = 1007; // )
    pt1->c_pattern[15] = 1007; // )
    pt1->c_pattern[16] = 7; // ;

    _patternChanger *pt2 = &patternIndex[2];
    pt2->pattern_type = STARTSWITH; // ** 사실 가변인자가 들어가므로 starts with + ends with 패턴으로 조합되어야 한다.
    pt2->java_pattern_length = 7; 
    pt2->c_pattern_length = 3;
    // 결론은 c 패턴부터 원본 패턴 길이까지 전부 복사해봄
    pt2->java_pattern[0] = SYSTEM_CODE;
    pt2->java_pattern[1] = DOT_OP;
    pt2->java_pattern[2] = OUT_CODE;
    pt2->java_pattern[3] = DOT_OP;
    pt2->java_pattern[4] = PRINTF_CODE;
    pt2->java_pattern[5] = PARENTHESES_LEFT_OP; 
    pt2->java_pattern[6] = STR_LIT;
    //pt2->java_pattern[7] = PARENTHESES_RIGHT_OP;
    //pt2->java_pattern[8] = SEMICOLON_OP;

    pt2->c_pattern[0] = 4; // printf
    pt2->c_pattern[1] = 5; // (
    pt2->c_pattern[2] = 6; // "--"
    //  pt2->c_pattern[3] = 7; // )
    //  pt2->c_pattern[4] = 8; // ;

    _patternChanger *pt3 = &patternIndex[3];
    pt3->pattern_type = STARTSWITH_TRIM_ALL;
    pt3->java_pattern_length = 0;
    pt3->c_pattern_length = 0;
    pt3->java_pattern[pt3->java_pattern_length++] = IMPORT_CODE;
    // 즉 C에선 거르는 패턴임. (패턴 없음)

    _patternChanger *pt4 = &patternIndex[4];
    pt4->pattern_type = EXACT;
    pt4->java_pattern_length = 0;
    pt4->c_pattern_length = 0;
    pt4->java_pattern[pt4->java_pattern_length++] = PUBLIC_CODE;
    pt4->java_pattern[pt4->java_pattern_length++] = CLASS_CODE;
    pt4->java_pattern[pt4->java_pattern_length++] = IDENTFIER;
    pt4->java_pattern[pt4->java_pattern_length++] = BRACE_LEFT_OP;
    
    // pattern : public static void main(String[] args){
    // to : int main(void){
    _patternChanger *pt5 = &patternIndex[5];
    pt5->pattern_type = EXACT;
    pt5->java_pattern_length = 0;
    pt5->c_pattern_length = 0;
    pt5->java_pattern[pt5->java_pattern_length++] = PUBLIC_CODE;
    pt5->java_pattern[pt5->java_pattern_length++] = STATIC_CODE;
    pt5->java_pattern[pt5->java_pattern_length++] = VOID_CODE;
    pt5->java_pattern[pt5->java_pattern_length++] = IDENTFIER; // exactly main
    pt5->java_pattern[pt5->java_pattern_length++] = PARENTHESES_LEFT_OP;
    pt5->java_pattern[pt5->java_pattern_length++] = STRING_CODE;
    pt5->java_pattern[pt5->java_pattern_length++] = SQUARE_BRACKET_LEFT_OP;
    pt5->java_pattern[pt5->java_pattern_length++] = SQUARE_BRACKET_RIGHT_OP;
    pt5->java_pattern[pt5->java_pattern_length++] = IDENTFIER;
    pt5->java_pattern[pt5->java_pattern_length++] = PARENTHESES_RIGHT_OP;
    pt5->java_pattern[pt5->java_pattern_length++] = BRACE_LEFT_OP;
    pt5->c_pattern[pt5->c_pattern_length++] = 1010; // int
    pt5->c_pattern[pt5->c_pattern_length++] = 1009; // " "
    pt5->c_pattern[pt5->c_pattern_length++] = 3; // main
    pt5->c_pattern[pt5->c_pattern_length++] = 4; // (
    pt5->c_pattern[pt5->c_pattern_length++] = 1011; // void
    pt5->c_pattern[pt5->c_pattern_length++] = 9; // )
    pt5->c_pattern[pt5->c_pattern_length++] = 1009; // " "
    pt5->c_pattern[pt5->c_pattern_length++] = 10; // {
    
    _patternChanger *pt6 = &patternIndex[6];
    pt6->pattern_type = STARTSWITH_TRIM_ALL;
    pt6->java_pattern_length = 0;
    pt6->c_pattern_length = 0;
    pt6->java_pattern[pt6->java_pattern_length++] = SCANNER_CODE;
    // 즉 C에선 거르는 패턴임. (패턴 없음)

    // pattern : public static void main(String args[]){
    // to : int main(void){
    _patternChanger *pt7 = &patternIndex[7];
    pt7->pattern_type = EXACT;
    pt7->java_pattern_length = 0;
    pt7->c_pattern_length = 0;
    pt7->java_pattern[pt7->java_pattern_length++] = PUBLIC_CODE;
    pt7->java_pattern[pt7->java_pattern_length++] = STATIC_CODE;
    pt7->java_pattern[pt7->java_pattern_length++] = VOID_CODE;
    pt7->java_pattern[pt7->java_pattern_length++] = IDENTFIER; // exactly main
    pt7->java_pattern[pt7->java_pattern_length++] = PARENTHESES_LEFT_OP;
    pt7->java_pattern[pt7->java_pattern_length++] = STRING_CODE;
    pt7->java_pattern[pt7->java_pattern_length++] = IDENTFIER;
    pt7->java_pattern[pt7->java_pattern_length++] = SQUARE_BRACKET_LEFT_OP;
    pt7->java_pattern[pt7->java_pattern_length++] = SQUARE_BRACKET_RIGHT_OP;
    pt7->java_pattern[pt7->java_pattern_length++] = PARENTHESES_RIGHT_OP;
    pt7->java_pattern[pt7->java_pattern_length++] = BRACE_LEFT_OP;
    pt7->c_pattern[pt7->c_pattern_length++] = 1010; // int
    pt7->c_pattern[pt7->c_pattern_length++] = 1009; // " "
    pt7->c_pattern[pt7->c_pattern_length++] = 3; // main
    pt7->c_pattern[pt7->c_pattern_length++] = 4; // (
    pt7->c_pattern[pt7->c_pattern_length++] = 1011; // void
    pt7->c_pattern[pt7->c_pattern_length++] = 9; // )
    pt7->c_pattern[pt7->c_pattern_length++] = 1009; // " "
    pt7->c_pattern[pt7->c_pattern_length++] = 10; // {

    // pattern : class id{
    // to : 없음 (파일만 생성)
    _patternChanger *pt8 = &patternIndex[8];
    pt8->pattern_type = EXACT;
    pt8->java_pattern_length = 0;
    pt8->c_pattern_length = 0;
    pt8->java_pattern[pt8->java_pattern_length++] = CLASS_CODE;
    pt8->java_pattern[pt8->java_pattern_length++] = IDENTFIER;
    pt8->java_pattern[pt8->java_pattern_length++] = BRACE_LEFT_OP;

    // pattern : int[] id;
    // to : int *id;
    _patternChanger *pt9 = &patternIndex[9];
    pt9->pattern_type = EXACT;
    pt9->java_pattern_length = 0;
    pt9->c_pattern_length = 0;
    pt9->java_pattern[pt9->java_pattern_length++] = INT_CODE;
    pt9->java_pattern[pt9->java_pattern_length++] = SQUARE_BRACKET_LEFT_OP;
    pt9->java_pattern[pt9->java_pattern_length++] = SQUARE_BRACKET_RIGHT_OP;
    pt9->java_pattern[pt9->java_pattern_length++] = IDENTFIER;
    pt9->java_pattern[pt9->java_pattern_length++] = SEMICOLON_OP;
    pt9->c_pattern[pt9->c_pattern_length++] = 1010; // int
    pt9->c_pattern[pt9->c_pattern_length++] = 1009; // " "
    pt9->c_pattern[pt9->c_pattern_length++] = 1004; // *
    pt9->c_pattern[pt9->c_pattern_length++] = 3; // IDENT
    pt9->c_pattern[pt9->c_pattern_length++] = 4; // semicolon

    // pattern : Stack st = new Stack();
    // to : --, but Stack.c 를 인클루드 하고 st. 를 안티패턴으로 추가하도록 함
    _patternChanger *pt10 = &patternIndex[10];
    pt10->pattern_type = EXACT;
    pt10->java_pattern_length = 0;
    pt10->c_pattern_length = 0;
    pt10->java_pattern[pt10->java_pattern_length++] = IDENTFIER;
    pt10->java_pattern[pt10->java_pattern_length++] = IDENTFIER;
    pt10->java_pattern[pt10->java_pattern_length++] = ASSIGN_OP;
    pt10->java_pattern[pt10->java_pattern_length++] = NEW_CODE;
    pt10->java_pattern[pt10->java_pattern_length++] = IDENTFIER;
    pt10->java_pattern[pt10->java_pattern_length++] = PARENTHESES_LEFT_OP;
    pt10->java_pattern[pt10->java_pattern_length++] = PARENTHESES_RIGHT_OP;
    pt10->java_pattern[pt10->java_pattern_length++] = SEMICOLON_OP;
    //////////////////////////////////////////////////////////////////////

    // pattern : public static final ~ (startswith)
    // to : const ~ ==> 디파인으로 할 것 (패턴 컴파일 리턴값을 다르게 주어야함) ==> 해결중
    _patternChanger *pt11 = &patternIndex[11];
    pt11->pattern_type = STARTSWITH;
    pt11->java_pattern_length = 3; 
    pt11->c_pattern_length = 3;
    pt11->java_pattern[0] = PUBLIC_CODE;
    pt11->java_pattern[1] = STATIC_CODE;
    pt11->java_pattern[2] = FINAL_CODE;
    pt11->c_pattern[0] = 1029;
    pt11->c_pattern[1] = 1012;
    pt11->c_pattern[2] = 1009;

    // pattern : return ; 
    // to : -- (main이 아닌 경우 그래도 둬야할 수도 있다.)
    _patternChanger *pt12 = &patternIndex[12];
    pt12->pattern_type = EXACT;
    pt12->java_pattern_length = 2;
    pt12->c_pattern_length = 0;
    pt12->java_pattern[0] = RETURN_CODE;
    pt12->java_pattern[1] = SEMICOLON_OP;

    // pattern : public IDENT () {
    // to : -- , 
    _patternChanger *pt13 = &patternIndex[13];
    pt13->pattern_type = EXACT;
    pt13->java_pattern_length = 0;
    pt13->c_pattern_length = 0;
    pt13->java_pattern[pt13->java_pattern_length++] = PUBLIC_CODE;
    pt13->java_pattern[pt13->java_pattern_length++] = IDENTFIER;
    pt13->java_pattern[pt13->java_pattern_length++] = PARENTHESES_LEFT_OP;
    pt13->java_pattern[pt13->java_pattern_length++] = PARENTHESES_RIGHT_OP;
    pt13->java_pattern[pt13->java_pattern_length++] = BRACE_LEFT_OP;
    pt13->c_pattern[pt13->c_pattern_length++] = 1013;

    // public static void main(String[] args) throws IOException {
    // to : int main(void) {
    _patternChanger *pt14 = &patternIndex[14];
    pt14->pattern_type = EXACT;
    pt14->java_pattern_length = 0;
    pt14->c_pattern_length = 0;
    pt14->java_pattern[pt14->java_pattern_length++] = PUBLIC_CODE;
    pt14->java_pattern[pt14->java_pattern_length++] = STATIC_CODE;
    pt14->java_pattern[pt14->java_pattern_length++] = VOID_CODE;
    pt14->java_pattern[pt14->java_pattern_length++] = IDENTFIER; // main
    pt14->java_pattern[pt14->java_pattern_length++] = PARENTHESES_LEFT_OP;
    pt14->java_pattern[pt14->java_pattern_length++] = STRING_CODE;
    pt14->java_pattern[pt14->java_pattern_length++] = SQUARE_BRACKET_LEFT_OP;
    pt14->java_pattern[pt14->java_pattern_length++] = SQUARE_BRACKET_RIGHT_OP;
    pt14->java_pattern[pt14->java_pattern_length++] = IDENTFIER; // args
    pt14->java_pattern[pt14->java_pattern_length++] = PARENTHESES_RIGHT_OP;
    pt14->java_pattern[pt14->java_pattern_length++] = THROWS_CODE;
    pt14->java_pattern[pt14->java_pattern_length++] = IOEXCEPTION_CODE;
    pt14->java_pattern[pt14->java_pattern_length++] = BRACE_LEFT_OP;
    pt14->c_pattern[pt14->c_pattern_length++] = 1010; // int
    pt14->c_pattern[pt14->c_pattern_length++] = 1009; // " "
    pt14->c_pattern[pt14->c_pattern_length++] = 3; // main
    pt14->c_pattern[pt14->c_pattern_length++] = 4; // (
    pt14->c_pattern[pt14->c_pattern_length++] = 1011; // void
    pt14->c_pattern[pt14->c_pattern_length++] = 9; // )
    pt14->c_pattern[pt14->c_pattern_length++] = 1009; // " "
    pt14->c_pattern[pt14->c_pattern_length++] = 12; // {

    // pattern : public ~
    // to : --
    // 앞에서 public 다른것들은 처리 해줬으므로 뒤에 걸리는 것들은 일반함수이다.
    _patternChanger *pt15 = &patternIndex[15];
    pt15->pattern_type = STARTSWITH;
    pt15->java_pattern_length = 1;
    pt15->c_pattern_length = 0;
    pt15->java_pattern[0] = PUBLIC_CODE;

    // pattern : File file = new File("id");
    // to : char *file = "id";
    _patternChanger *pt16 = &patternIndex[16];
    pt16->pattern_type = EXACT;
    pt16->java_pattern_length = 0;
    pt16->c_pattern_length = 0;
    pt16->java_pattern[pt16->java_pattern_length++] = FILE_CODE;
    pt16->java_pattern[pt16->java_pattern_length++] = IDENTFIER;
    pt16->java_pattern[pt16->java_pattern_length++] = ASSIGN_OP;
    pt16->java_pattern[pt16->java_pattern_length++] = NEW_CODE;
    pt16->java_pattern[pt16->java_pattern_length++] = FILE_CODE;
    pt16->java_pattern[pt16->java_pattern_length++] = PARENTHESES_LEFT_OP;
    pt16->java_pattern[pt16->java_pattern_length++] = STR_LIT;
    pt16->java_pattern[pt16->java_pattern_length++] = PARENTHESES_RIGHT_OP;
    pt16->java_pattern[pt16->java_pattern_length++] = SEMICOLON_OP;
    pt16->c_pattern[pt16->c_pattern_length++] = 1014; // char *
    pt16->c_pattern[pt16->c_pattern_length++] = 1; // file
    pt16->c_pattern[pt16->c_pattern_length++] = 1009; // " "
    pt16->c_pattern[pt16->c_pattern_length++] = 2; // =
    pt16->c_pattern[pt16->c_pattern_length++] = 1009; // " "
    pt16->c_pattern[pt16->c_pattern_length++] = 6; // "id"
    pt16->c_pattern[pt16->c_pattern_length++] = 8; // ;

    // pattern : FileWriter writer = new FileWriter(file, false);
    // to : File *writer = fopen(file, "w");
    _patternChanger *pt17 = &patternIndex[17];
    pt17->pattern_type = EXACT;
    pt17->java_pattern_length = 0;
    pt17->c_pattern_length = 0;
    pt17->java_pattern[pt17->java_pattern_length++] = FILEWRITER_CODE;
    pt17->java_pattern[pt17->java_pattern_length++] = IDENTFIER;
    pt17->java_pattern[pt17->java_pattern_length++] = ASSIGN_OP;
    pt17->java_pattern[pt17->java_pattern_length++] = NEW_CODE;
    pt17->java_pattern[pt17->java_pattern_length++] = FILEWRITER_CODE;
    pt17->java_pattern[pt17->java_pattern_length++] = PARENTHESES_LEFT_OP;
    pt17->java_pattern[pt17->java_pattern_length++] = IDENTFIER;
    pt17->java_pattern[pt17->java_pattern_length++] = COMMA_OP;
    pt17->java_pattern[pt17->java_pattern_length++] = FALSE_CODE;
    pt17->java_pattern[pt17->java_pattern_length++] = PARENTHESES_RIGHT_OP;
    pt17->java_pattern[pt17->java_pattern_length++] = SEMICOLON_OP;
    pt17->c_pattern[pt17->c_pattern_length++] = 1015; // FILE *
    pt17->c_pattern[pt17->c_pattern_length++] = 1; // writer
    pt17->c_pattern[pt17->c_pattern_length++] = 1009; // 공백
    pt17->c_pattern[pt17->c_pattern_length++] = 2; // = 
    pt17->c_pattern[pt17->c_pattern_length++] = 1009; // 공백
    pt17->c_pattern[pt17->c_pattern_length++] = 1016; // fopen
    pt17->c_pattern[pt17->c_pattern_length++] = 5; // (
    pt17->c_pattern[pt17->c_pattern_length++] = 6; // file
    pt17->c_pattern[pt17->c_pattern_length++] = 7; // ,
    pt17->c_pattern[pt17->c_pattern_length++] = 1009; // 공백
    pt17->c_pattern[pt17->c_pattern_length++] = 1017; // "
    pt17->c_pattern[pt17->c_pattern_length++] = 1018; // w 
    pt17->c_pattern[pt17->c_pattern_length++] = 1017; // "
    pt17->c_pattern[pt17->c_pattern_length++] = 9; // )
    pt17->c_pattern[pt17->c_pattern_length++] = 10; // ;
    // if (!writer) exit(1);
    pt17->c_pattern[pt17->c_pattern_length++] = 1009; // 공백
    pt17->c_pattern[pt17->c_pattern_length++] = 1024; // if
    pt17->c_pattern[pt17->c_pattern_length++] = 5; // (
    pt17->c_pattern[pt17->c_pattern_length++] = 1025; // !
    pt17->c_pattern[pt17->c_pattern_length++] = 1; // writer
    pt17->c_pattern[pt17->c_pattern_length++] = 9; // )
    pt17->c_pattern[pt17->c_pattern_length++] = 1026; // exit
    pt17->c_pattern[pt17->c_pattern_length++] = 5; // (
    pt17->c_pattern[pt17->c_pattern_length++] = 1027; // 1
    pt17->c_pattern[pt17->c_pattern_length++] = 9; // )
    pt17->c_pattern[pt17->c_pattern_length++] = 10; // ;

    // writer.write("~");
    // fprintf(writer, "~");
    _patternChanger *pt18 = &patternIndex[18];
    pt18->pattern_type = STARTSWITH;
    pt18->java_pattern_length = 0;
    pt18->c_pattern_length = 0;
    pt18->java_pattern[pt18->java_pattern_length++] = IDENTFIER;
    pt18->java_pattern[pt18->java_pattern_length++] = DOT_OP;
    pt18->java_pattern[pt18->java_pattern_length++] = WRITE_CODE;
    pt18->java_pattern[pt18->java_pattern_length++] = PARENTHESES_LEFT_OP;
    pt18->java_pattern[pt18->java_pattern_length++] = STR_LIT;
    pt18->c_pattern[pt18->c_pattern_length++] = 1020; // fprintf
    pt18->c_pattern[pt18->c_pattern_length++] = 3; // (
    pt18->c_pattern[pt18->c_pattern_length++] = 0; // writer
    pt18->c_pattern[pt18->c_pattern_length++] = 1002; // ,
    pt18->c_pattern[pt18->c_pattern_length++] = 1009; // " "
    pt18->c_pattern[pt18->c_pattern_length++] = 4;

    // writer.flush();
    // fflush(writer);
    _patternChanger *pt19 = &patternIndex[19];
    pt19->pattern_type = EXACT;
    pt19->java_pattern_length = 0;
    pt19->c_pattern_length = 0;
    pt19->java_pattern[pt19->java_pattern_length++] = IDENTFIER;
    pt19->java_pattern[pt19->java_pattern_length++] = DOT_OP;
    pt19->java_pattern[pt19->java_pattern_length++] = FLUSH_CODE;
    pt19->java_pattern[pt19->java_pattern_length++] = PARENTHESES_LEFT_OP;
    pt19->java_pattern[pt19->java_pattern_length++] = PARENTHESES_RIGHT_OP;
    pt19->java_pattern[pt19->java_pattern_length++] = SEMICOLON_OP;
    pt19->c_pattern[pt19->c_pattern_length++] = 1021; // fflush
    pt19->c_pattern[pt19->c_pattern_length++] = 3; // (
    pt19->c_pattern[pt19->c_pattern_length++] = 0; // writer
    pt19->c_pattern[pt19->c_pattern_length++] = 4; // )
    pt19->c_pattern[pt19->c_pattern_length++] = 5; // ;

    // pattern : FileWriter writer = new FileWriter(file, true);
    // to : File *writer = fopen(file, "a");
    _patternChanger *pt20 = &patternIndex[20];
    pt20->pattern_type = EXACT;
    pt20->java_pattern_length = 0;
    pt20->c_pattern_length = 0;
    pt20->java_pattern[pt20->java_pattern_length++] = FILEWRITER_CODE;
    pt20->java_pattern[pt20->java_pattern_length++] = IDENTFIER;
    pt20->java_pattern[pt20->java_pattern_length++] = ASSIGN_OP;
    pt20->java_pattern[pt20->java_pattern_length++] = NEW_CODE;
    pt20->java_pattern[pt20->java_pattern_length++] = FILEWRITER_CODE;
    pt20->java_pattern[pt20->java_pattern_length++] = PARENTHESES_LEFT_OP;
    pt20->java_pattern[pt20->java_pattern_length++] = IDENTFIER;
    pt20->java_pattern[pt20->java_pattern_length++] = COMMA_OP;
    pt20->java_pattern[pt20->java_pattern_length++] = TRUE_CODE;
    pt20->java_pattern[pt20->java_pattern_length++] = PARENTHESES_RIGHT_OP;
    pt20->java_pattern[pt20->java_pattern_length++] = SEMICOLON_OP;
    pt20->c_pattern[pt20->c_pattern_length++] = 1015; // FILE *
    pt20->c_pattern[pt20->c_pattern_length++] = 1; // writer
    pt20->c_pattern[pt20->c_pattern_length++] = 1009; // 공백
    pt20->c_pattern[pt20->c_pattern_length++] = 2; // = 
    pt20->c_pattern[pt20->c_pattern_length++] = 1009; // 공백
    pt20->c_pattern[pt20->c_pattern_length++] = 1016; // fopen
    pt20->c_pattern[pt20->c_pattern_length++] = 5; // (
    pt20->c_pattern[pt20->c_pattern_length++] = 6; // file
    pt20->c_pattern[pt20->c_pattern_length++] = 7; // ,
    pt20->c_pattern[pt20->c_pattern_length++] = 1009; // 공백
    pt20->c_pattern[pt20->c_pattern_length++] = 1017; // "
    pt20->c_pattern[pt20->c_pattern_length++] = 1019; // a
    pt20->c_pattern[pt20->c_pattern_length++] = 1017; // "
    pt20->c_pattern[pt20->c_pattern_length++] = 9; // )
    pt20->c_pattern[pt20->c_pattern_length++] = 10; // ;
    // if (!writer) exit(1);
    pt20->c_pattern[pt20->c_pattern_length++] = 1009; // 공백
    pt20->c_pattern[pt20->c_pattern_length++] = 1024; // if
    pt20->c_pattern[pt20->c_pattern_length++] = 5; // (
    pt20->c_pattern[pt20->c_pattern_length++] = 1025; // !
    pt20->c_pattern[pt20->c_pattern_length++] = 1; // writer
    pt20->c_pattern[pt20->c_pattern_length++] = 9; // )
    pt20->c_pattern[pt20->c_pattern_length++] = 1026; // exit
    pt20->c_pattern[pt20->c_pattern_length++] = 5; // (
    pt20->c_pattern[pt20->c_pattern_length++] = 1027; // 1
    pt20->c_pattern[pt20->c_pattern_length++] = 9; // )
    pt20->c_pattern[pt20->c_pattern_length++] = 10; // ;

    // id.close();
    // fclose(id);
    _patternChanger *pt21 = &patternIndex[21];
    pt21->pattern_type = EXACT;
    pt21->java_pattern_length = 0;
    pt21->c_pattern_length = 0;
    pt21->java_pattern[pt21->java_pattern_length++] = IDENTFIER;
    pt21->java_pattern[pt21->java_pattern_length++] = DOT_OP;
    pt21->java_pattern[pt21->java_pattern_length++] = CLOSE_CODE;
    pt21->java_pattern[pt21->java_pattern_length++] = PARENTHESES_LEFT_OP;
    pt21->java_pattern[pt21->java_pattern_length++] = PARENTHESES_RIGHT_OP;
    pt21->java_pattern[pt21->java_pattern_length++] = SEMICOLON_OP;
    pt21->c_pattern[pt21->c_pattern_length++] = 1022; // fclose
    pt21->c_pattern[pt21->c_pattern_length++] = 3; // (
    pt21->c_pattern[pt21->c_pattern_length++] = 0; // writer
    pt21->c_pattern[pt21->c_pattern_length++] = 4; // )
    pt21->c_pattern[pt21->c_pattern_length++] = 5; // ;

}

int PATT_is_match(const _lexPattern *pattern)
{
    // 매칭시 해당 패턴 인덱스, 미 매칭시 -1
    for (int i = 0; i < NUMBER_OF_PATTERNS; ++i) {
        _patternChanger *patt = &patternIndex[i];
        int matched = 1;
        if (patt->pattern_type == STARTSWITH || patt->pattern_type == STARTSWITH_TRIM_ALL) {
            for (int j = 0; j < patt->java_pattern_length; ++j) { 
                 if (pattern->pattern[j] != patt->java_pattern[j]) {matched = 0; break;}
            }
            if (matched) return i;
        } else {
            for (int j = 0; j < patt->java_pattern_length; ++j) { 
                // DBGMSG("비교 패턴 : %d %d\n", pattern->pattern[j], patt->java_pattern[j]);
                if (pattern->pattern[j] != patt->java_pattern[j]) {matched = 0; break;}
            }
            if (matched) {
                // DBGMSG("이쪽에 제어가 걸림 %d\n", i);
                return i;
            }
        }
    }
    return -1;
}

int PATT_pattern_compile(const _lexPattern *pattern, char *resultbuf)
{
    int ret = 0;
    if (PATT_is_initalized == 0) {
        PATT_is_initalized = 1;
        PATT_init();
    }
    // 여는 중괄호와 닫는 중괄호 스택을 감시함
    for (int i = 0; i < pattern->pattern_length; i++) {
        if(pattern->pattern[i] == BRACE_LEFT_OP) {
            ++patt_brace_stack;
            if(patt_brace_stack == 1) ret = 1;
            break;
        } else if (pattern->pattern[i] == BRACE_RIGHT_OP) {
            --patt_brace_stack;
            if(patt_brace_stack == 0) ret = 2;
            break;
        }
    }

    memset(resultbuf, 0x00, MAX_RESULT_CODE_LENGTH); // 문자열 배열 비움

    int matchedIndex = PATT_is_match(pattern);
    if(matchedIndex >= 0) {
        _patternChanger *patt = &patternIndex[matchedIndex];


        // 패턴별 java->c 함수 발생시 기록
        switch (matchedIndex) {
            case 0:
            if(cngCnt[0] != 1)  {
                cngCnt[0] = 1;
                sprintf(cngPatt[0], "%s.nextInt() -> scanf()", pattern->buffer[2]);
            }
            break;
            case 1:
            if(cngCnt[1] != 1)  {
                cngCnt[1] = 1;
                sprintf(cngPatt[1], "new int -> calloc()");
            }
            break;
            case 2:
            if(cngCnt[2] != 1)  {
                cngCnt[2] = 1;
                sprintf(cngPatt[2], "System.out.printf() -> printf()");
            }
            break;
            case 17: case 20:
            if(cngCnt[3] != 1)  {
                cngCnt[3] = 1;
                sprintf(cngPatt[3], "new FileWriter() -> fopen()"); // 이게 왜 안돼
            }
            break;
            case 18:
            if(cngCnt[4] != 1)  {
                cngCnt[4] = 1;
                sprintf(cngPatt[4], "%s.write() -> fprintf()", pattern->buffer[0]);
            }
            break;
            case 19:
            if(cngCnt[5] != 1)  {
                cngCnt[5] = 1;
                sprintf(cngPatt[5], "%s.flush() -> fflush()", pattern->buffer[0]);
            }
            break;
            case 21:
            if(cngCnt[6] != 1)  {
                cngCnt[6] = 1;
                sprintf(cngPatt[6], "%s.close() -> fclose()", pattern->buffer[0]);
            }
            break;
        }

        // main 패턴 검사
        switch (matchedIndex) {
            case 5: case 7: case 14:
            ret = 3;
            break;
        }

        // 안티 패턴 파악하기. 해당 패턴 넘버 : 10
        if (matchedIndex == 10) {
            char tmp[20];
            sprintf(tmp, "%s.", pattern->buffer[1]);
            strcpy(anti_pattern[number_of_anti_pattern++], tmp); // st가 아닌 다른 것도 해당함.
        } else if (matchedIndex == 11) {
            // define 추가
            // public static final int STACK_SIZE = 10;
            strcpy(define_name, pattern->buffer[4]);
            strcpy(define_value, pattern->buffer[6]);
            ret = 4;
        }



        for (int i = 0; i < patt->c_pattern_length; ++i) {
            if (patt->c_pattern[i] >= 1000) {
                putheader(reqHeaders+tailreqHeaders, C_codes[patt->c_pattern[i]-1000]);
                tailreqHeaders = strlen(reqHeaders);
                strcat(resultbuf, C_codes[patt->c_pattern[i]-1000]);
            } else {
                putheader(reqHeaders+tailreqHeaders, pattern->buffer[patt->c_pattern[i]]);
                tailreqHeaders = strlen(reqHeaders);
                strcat(resultbuf, pattern->buffer[patt->c_pattern[i]]);
            }
        }
        if (patt->pattern_type == STARTSWITH) {
            // DBGMSG("스타트 윗 패턴, 나머지는 쭉 채움 %d", patt->c_pattern_length);
            // 결론은 c 패턴부터 원본 패턴 길이까지 전부 복사해봄
            for (int i = patt->java_pattern_length; i < pattern->pattern_length; ++i) {
                strcat(resultbuf, pattern->buffer[i]); // startsWith 를 고려한 나머지는 쭉 출력해준다.
                if (space_check(pattern, i))
                    strcat(resultbuf, " ");
            }
        }
    } else {
        // 그대로 붙여 출력
        for (int i = 0; i < pattern->pattern_length; ++i) {
            strcat(resultbuf, pattern->buffer[i]);
            if (space_check(pattern, i))
                strcat(resultbuf, " ");
        }
    }
    return ret;
}

int space_check(const _lexPattern *pattern, int i) {
    // 다음 토큰이 수치리터럴, 아이덴티파이어, 연산자면 뛴다. 전위 후위, ; 연산자면 안뛴다. ) 가 뒤에 와도 안 뛴다. ','도 안 뛴다. '.'도 안 뛴다.
    // 특정 연산자 (.등) 뒤에는 안 뛴다.
if (
                pattern->pattern[i] != DOT_OP && // 지금 . 연산자가 아니고
                pattern->pattern[i] != SQUARE_BRACKET_LEFT_OP && // [가 아니고
                pattern->pattern[i] != PARENTHESES_LEFT_OP && // (가 아니고
                i+1 < pattern->pattern_length && // 다음이 있고
                (
                    pattern->pattern[i+1] == 10 || // 단순 리터럴?
                    pattern->pattern[i+1] == 11 || // 단순 리터럴?
                    pattern->pattern[i+1] == 2  || // 단순 연산자?
                    (
                        pattern->pattern[i+1] >= 100 &&
                        pattern->pattern[i+1] != 114 &&
                        pattern->pattern[i+1] != 115 &&
                        pattern->pattern[i+1] != 109 &&
                        pattern->pattern[i+1] != 103 &&
                        pattern->pattern[i+1] != 104 &&
                        pattern->pattern[i+1] != 119 &&
                        pattern->pattern[i+1] != SQUARE_BRACKET_LEFT_OP &&
                        pattern->pattern[i+1] != SQUARE_BRACKET_RIGHT_OP &&
                        pattern->pattern[i+1] != 100
                    ) ||
                    (
                        pattern->pattern[i+1] >= 20 &&
                        pattern->pattern[i+1] <= 40
                    )
                )
            ) {
                return 1;
            }
            return 0;
}