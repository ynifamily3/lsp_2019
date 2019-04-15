#include "lex.h"
#include "pattern.h"

int lexPattern_number_of_line = 0;
// _lexPattern pattern[MAX_LINE_PER_FILE];
// 최대 라인 수 : 400

int number_of_anti_pattern = 0;
char anti_pattern[10][64]; //객체 등은 제거되어야 한다. 예를 들어 st.

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

void convert_java_to_c(char *output, const char *input)
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
                for (int j = 0; j < number_of_anti_pattern; j++) {
                    remove_str_in_str(output, anti_pattern[j]);
                }
                printf("%s\n", output);
            }
        }
    }
}
