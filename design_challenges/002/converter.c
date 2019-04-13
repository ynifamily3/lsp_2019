#include "lex.h"
#include "pattern.h"

int lexPattern_number_of_line = 0;
// _lexPattern pattern[MAX_LINE_PER_FILE];
// 최대 라인 수 : 400
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
        if(strlen(output))
            printf("%s\n", output);
        if (c == 2) {
            fprintf(stderr, "end of %s\n", fname);
        }
    }
}