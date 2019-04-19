#include "lex.h"
#include "arg_read.h"
#include "pattern.h"
#include "header_table.h"
#include "debug.h"

int lexPattern_number_of_line = 0;
// _lexPattern pattern[MAX_LINE_PER_FILE];
// 최대 라인 수 : 400

extern int patt_brace_stack;

extern int number_of_anti_pattern;
extern char (anti_pattern)[10][64];

const int number_of_change_pattern = 1; // 상수임
char change_pattern_before[4][10] = {"null"}; // 두개가 서로 같은 문자열 길이여야 함. 혹은 after작거나
char change_pattern_after[4][10] = {"NULL"};

extern int (cngCnt)[7];
extern char (cngPatt)[7][50];

int is_main_func = 0;
extern char *argv1;


extern int tailreqHeaders;
extern char (reqHeaders)[512]; // #include <xxx>\n#include <yyy> 등 (define 도)

int go_include = 0;
extern char (define_name)[20]; // #define X Y 에서 X
extern char (define_value)[30]; // #define X Y 에서 Y

// 출력 누적물 (헤더들 제외)
char c_source_file[MAX_RESULT_CODE_LENGTH];

void change_str_in_str(char *dest, const char *before, const char *after)
{
    size_t cplen = strlen(before);
    char *pos = strstr(dest, before);
    if (!pos) return;
    for (int i = 0; i < (int)cplen; i++) {
        pos[i] = after[i];
    }
}

void remove_str_in_str(char *dest, const char *anti_pattern)
{
    char *origin_dest = dest;
    char temp[256]; // 얘를 초기화해주지 않으면 계속 재사용 된다..
    // 분명히 이상한 상황이지만 일단 초기화
    memset(temp, 0x00, 256);
    int anti_pattern_len = strlen(anti_pattern);
    char *cp = strstr(dest, anti_pattern);
    if (cp != NULL) {
        while (cp != NULL) {
            *cp = '\0'; // 해당 패턴 시작부분을 자른다.
            strcat(temp, dest); // 그 전과 지금 - 1까지의 모든 것들을 넣는다.
            dest = cp + anti_pattern_len; // 그 다음 검사해야 할 부분
            cp = strstr(dest, anti_pattern);
            if (!cp) {
                strcat(temp, dest);
            }
        }
        strncpy(origin_dest, temp, MAX_RESULT_CODE_LENGTH);   
    }
}

void print_repeat(char *p, int cnt) {
    while(cnt--) {
        strncat(c_source_file, p, MAX_RESULT_CODE_LENGTH);
    }
}

void convert_java_to_c(char *output, const char *input)
{
    // char cfilename[256];
    char fname[256];
    char include_sentence[50];
    FILE *fp;
    _lexPattern **pa = (_lexPattern **)calloc(MAX_LINE_PER_FILE, sizeof(_lexPattern *));
    for (int i = 0; i < MAX_LINE_PER_FILE; i++) {
        pa[i] = (_lexPattern *)calloc(1, sizeof(_lexPattern));
    }

    lex(pa, input);
    for (int i = 0; i < lexPattern_number_of_line; i++) {
        int c = PATT_pattern_compile(pa[i], output); // 이곳에서 여닫는 걸 조사해서 파일을 열고 닫는다.
        if (c == 1 || c == 3) {
            // pa[i]->buffer[pa[i]->pattern_length-2]
            if (c == 1) {
                sprintf(fname, "%s.c", pa[i]->buffer[pa[i]->pattern_length-2]);
                // fprintf(stderr, "make %s :\n", fname);
                fp = fopen(fname, "w");
                tailreqHeaders = 0; // tail 초기화
                reqHeaders[0] = '\0'; // 헤더 목록 초기화
                reset_ref_count(); // include 레퍼런스 카운트 초기화
                if (go_include == 1) {
                    // include stack.c 한다..
                    go_include = 0;
                    strcat(reqHeaders, include_sentence); // cpy를 cat로 바꿈
                    tailreqHeaders = strlen(include_sentence);
                }
            }
            // main에 해당하면 플래그를 켠다.
            if(c == 3) is_main_func = 1;
        }
        if (c == 4) {
            char define_sentence[50];
            sprintf(define_sentence, "#define %s %s\n", define_name, define_value);
            strcat(reqHeaders + tailreqHeaders, define_sentence); // strcpy 를 cat로 바꿈
            tailreqHeaders += strlen(define_sentence);
        }
        if (c == 2) {
            if (is_main_func == 0) {
                    go_include = 1;
                    // 메인 함수가 아닌 파일이 끝남
                    // 그리고 헤더 사용 횟수 같은 것도 초기화
                    sprintf(include_sentence, "#include \"%s\"\n", fname); // main에 추가해 달라고 예약해야한다.
                    // strcat(reqHeaders, include_sentence); // cpy를 cat로 바꿈
                    // tailreqHeaders = strlen(include_sentence);
                    //tailreqHeaders = 0;
                    //reqHeaders[0] = '\0';
                    //reset_ref_count();
            }
            is_main_func = 0; // 파일이 끝나면 메인도 끝난다.
            // fprintf(stderr, "end of %s\n", fname);
            // 출력 시 헤더 리스트 같이 출력
            printf("------------------------------------\n");
            printf("%s\n", reqHeaders);
            printf("%s", c_source_file); // 소스코드 쭉 출력
            printf("------------------------------------\n");
            //fprintf(fp, "%s\n", reqHeaders);
            //fprintf(fp, "%s", c_source_file);
            printf("%s converting is finished!\n", fname);
            fclose(fp);
            c_source_file[0] = '\0'; // 초기화
        }
        else {
            if(strlen(output)) {
                // 안티패턴 제거하기 (객체 제거)
                // 특정 패턴 치환 (null -> NULL)
                for (int j = 0; j < number_of_anti_pattern; j++) {
                    remove_str_in_str(output, anti_pattern[j]);
                }
                for (int j = 0; j < number_of_change_pattern; j++) {
                    change_str_in_str(output, change_pattern_before[j], change_pattern_after[j]);
                }
                
                // brace stack가 1이고 } 이면 end of 일 것이다. 그리고 메인이면.
                if(patt_brace_stack == 1 && output[0] == '}' && is_main_func == 1) {
                    // printf("끝날 거 같다.\n");
                    print_repeat("\t", patt_brace_stack);
                    putheader(reqHeaders+tailreqHeaders, "exit");
                    tailreqHeaders = strlen(reqHeaders);
                    strncat(c_source_file, "exit(0);\n", MAX_RESULT_CODE_LENGTH);
                    // printf("exit(0);\n");
                }

                // output 에 { } 가 있으면 지연된 쓰기 
                if (output[strlen(output)-1] == '{')
                    print_repeat("\t", patt_brace_stack-2);
                else
                    print_repeat("\t", patt_brace_stack-1);
                // printf("%s\n", output); // 나중엔없애
                strcat(output, "\n");
                strncat(c_source_file, output, MAX_RESULT_CODE_LENGTH);
            }
        }
    }

    if(arg_option_p) {
        int j = 0;
        for (int i = 0; i < 7; i++) {
            if (cngCnt[i] == 1) {
                printf("%d %s\n", ++j, cngPatt[i]);
            }
        }
    }
}
