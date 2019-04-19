#include "lex.h"
#include "arg_read.h"
#include "pattern.h"
#include "debug.h"

int lexPattern_number_of_line = 0;
// _lexPattern pattern[MAX_LINE_PER_FILE];
// 최대 라인 수 : 400

extern int patt_brace_stack;

int number_of_anti_pattern = 0;
char anti_pattern[10][64]; //객체 등은 제거되어야 한다. 예를 들어 st.

const int number_of_change_pattern = 1; // 상수임
char change_pattern_before[4][10] = {"null"}; // 두개가 서로 같은 문자열 길이여야 함. 혹은 after작거나
char change_pattern_after[4][10] = {"NULL"};

extern int (cngCnt)[7];
extern char (cngPatt)[7][50];

int is_main_func = 0;
extern char *argv1;

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
    char temp[MAX_RESULT_CODE_LENGTH];
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

void print_repeat(char p, int cnt) {
    while(cnt--) {
        putchar(p);
    }
}

// 절대 경로로부터 파일 이름을 구해오는 함수
void getFilename(char *result, const char *argv1)
{
    int endPos = strlen(argv1) - 1;
    int i;
    for (i = endPos; i > 0; --i) {
        if (argv1[i] == '/') { ++i; break;}
    }
    strcpy(result, argv1 + i);
}

void convert_java_to_c(char *output, const char *input, char *argv1)
{
    _lexPattern **pa = (_lexPattern **)calloc(MAX_LINE_PER_FILE, sizeof(_lexPattern *));
    for (int i = 0; i < MAX_LINE_PER_FILE; i++) {
        pa[i] = (_lexPattern *)calloc(1, sizeof(_lexPattern));
    }

    lex(pa, input);
    char fname[50];
    for (int i = 0; i < lexPattern_number_of_line; i++) {
        int c = PATT_pattern_compile(pa[i], output); // 이곳에서 여닫는 걸 조사해서 파일을 열고 닫는다.
        if (c == 1) {
            // pa[i]->buffer[pa[i]->pattern_length-2]
            sprintf(fname, "%s.c", pa[i]->buffer[pa[i]->pattern_length-2]);
            fprintf(stderr, "make %s :\n", fname);
            // main에 해당하면 플래그를 켠다.
            char onlyFname[20]; // 입력으로 들어온 java File
            getFilename(onlyFname, argv1);
            DBGMSG("%s", onlyFname);
            // 이러지 말고 그냥 main패턴인지 검사하는게 좋지 않을까?
        }
        // PATT_pattern_compile 의 리턴값
        // 0 : 아무것도 안함
        // 1 : (public) class ~~~ { 패턴이 왔으므로 (brace stack이 1이 됨) 해당 파일명을 w모드로 생성하고 쓸 준비를 함
        // 2 : } ==> 하고 brace stack 0 이 되었으므로 해당 파일을 닫아라.
        if (c == 2) {
            fprintf(stderr, "end of %s\n", fname);
        } else {
            if(strlen(output)) {
                // 안티패턴 제거하기 (객체 제거)
                // 특정 패턴 치환 (null -> NULL)
                for (int j = 0; j < number_of_anti_pattern; j++) {
                    remove_str_in_str(output, anti_pattern[j]);
                }
                for (int j = 0; j < number_of_change_pattern; j++) {
                    change_str_in_str(output, change_pattern_before[j], change_pattern_after[j]);
                }
                
                // brace stack가 1이고 (어디서 구해오지) } 이면 end of 일 것이다. 그리고 메인이면.
                if(patt_brace_stack == 1 && output[0] == '}') {
                    // printf("끝날 거 같다.\n");
                    printf("exit(0);\n");
                }
                printf("%s\n", output);
            }
        }
    }
    // 컨버터 석세스!!
    if(arg_option_p) {
        int j = 0;
        for (int i = 0; i < 7; i++) {
            // DBGMSG("%d : %s", i, cngPatt[i]);
            if (cngCnt[i] == 1) {
                printf("%d %s\n", ++j, cngPatt[i]);
            }
        }
    }
}
