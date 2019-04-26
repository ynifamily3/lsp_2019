#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
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

// 입력 누적 (-r 옵션을 위해)
char java_buf[MAX_RESULT_CODE_LENGTH] = "";

int produces_c_source_file_numbers = 0;
char c_source_file_names[5][767]; // C언어 파일 이름들 (full path)
char c_source_file_names_print[5][256]; // C언어 파일 이름들 (순수 path)

extern char dir_path[512]; // main에 정의된 것.
extern char filename[256]; // main에 정의된 것. 순수 파일명 xx.java
extern char pathname[767]; // main에 정의된 것. dir_path + filename

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

void convert_java_to_c(char *output, char *input)
{
    char cfilename[256]; // 순수 c 파일 이름
    char fname[767]; // path 포함한 c 파일 이름
    char include_sentence[50];
    FILE *fp;
    _lexPattern **pa = (_lexPattern **)calloc(MAX_LINE_PER_FILE, sizeof(_lexPattern *));
    for (int i = 0; i < MAX_LINE_PER_FILE; i++) {
        pa[i] = (_lexPattern *)calloc(1, sizeof(_lexPattern));
    }
    // {~~}else {공백}\n 를 위한 정규화
    char *else_ptr = input;
    while ((else_ptr = strstr(else_ptr, "else")) != NULL) {
        char *origin;
        // 공백이면 다음부분 검사, 다른 문자가 들어오면 불일치, \n들어오면 일치
        else_ptr += 4;
        origin = else_ptr;
        while (isspace(*else_ptr) && *else_ptr != '\n') {
            else_ptr++;
        }
        if (*else_ptr == '\n') {
            *origin = '\n';
        }
    }

    lex(pa, input);
    for (int i = 0; i < lexPattern_number_of_line; i++) {
        int c = PATT_pattern_compile(pa[i], output); // 이곳에서 여닫는 걸 조사해서 파일을 열고 닫는다.
        if (c == 1 || c == 3) {
            // pa[i]->buffer[pa[i]->pattern_length-2]
            if (c == 1) {
                size_t olen = strlen(pa[i]->buffer[pa[i]->pattern_length-2]);
                if (pa[i]->buffer[pa[i]->pattern_length-2][olen-1] == '\n') {
                    // class ~~\n { 대응
                    pa[i]->buffer[pa[i]->pattern_length-2][olen-1] = '\0';
                }
                sprintf(fname, "%s%s.c", dir_path, pa[i]->buffer[pa[i]->pattern_length-2]);
                sprintf(cfilename, "%s.c", pa[i]->buffer[pa[i]->pattern_length-2]);
                strcpy(c_source_file_names_print[produces_c_source_file_numbers], cfilename);
                strcpy(c_source_file_names[produces_c_source_file_numbers++], fname);
                // fprintf(stderr, "make %s :\n", cfilename);

                c_source_file[0] = '\0'; // 초기화
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
                    sprintf(include_sentence, "#include \"%s\"\n", cfilename); // main에 추가해 달라고 예약해야한다.
            }
            is_main_func = 0; // 파일이 끝나면 메인도 끝난다.
            fprintf(fp, "%s\n", reqHeaders);
            fprintf(fp, "%s", c_source_file);
            if(!arg_option_r)
                printf("%s converting is finished!\n", cfilename);
            fclose(fp);
            // c_source_file[0] = '\0'; // 초기화
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

                strcat(output, "\n");
                strncat(c_source_file, output, MAX_RESULT_CODE_LENGTH);
            }
        }

        if (arg_option_r) {
            static int java_line_count = 0;
            static int java_brace_stack = 0;
            char lc[5];
            int m = 1; // 들여쓰기 관련
            if(pa[i]->pattern[pa[i]->pattern_length-1] == BRACE_RIGHT_OP) m = 2;
            for (int j = 0; j < pa[i]->pattern_length; j++)  {
                if (j == 0) {
                    ++java_line_count;
                    sprintf(lc, "%d ", java_line_count);
                    strcat(java_buf, lc);
                    // 들여쓰기 삽입
                    for (int cnt = java_brace_stack-m; cnt >= 0; cnt--) {
                        m = 1;
                        strcat(java_buf, "\t");
                    }
                }
                // 괄호 스택을 검사
                if(pa[i]->pattern[j] == BRACE_LEFT_OP) {
                    ++java_brace_stack;
                } else if (pa[i]->pattern[j] == BRACE_RIGHT_OP) {
                    --java_brace_stack;
                }
                strcat(java_buf, pa[i]->buffer[j]);
                strcat(java_buf, " ");
            }
            
            strcat(java_buf, "\n");

            pid_t pid = fork();
            if (pid < 0) fprintf(stderr, "fork error\n");
            else if (pid == 0) {
                // child process
                system("clear");
                printf("%s Converting....\n", filename);
                printf("--------\n%s\n--------\n%s\n", filename, java_buf);
                // tmpnam으로 헤더랑 c를 조합해 놓은 것을 만든다.
                FILE *tmp_c_file = tmpfile();
                if (strlen(reqHeaders))
                    fprintf(tmp_c_file, "%s\n%s", reqHeaders, c_source_file);
                else
                    fprintf(tmp_c_file, "%s", c_source_file);
                if (produces_c_source_file_numbers != 0) {
                    printf("--------\n%s\n--------\n", c_source_file_names_print[produces_c_source_file_numbers-1]);
                    rewind(tmp_c_file);
                    int c_line_count = 0;
                    char c_line_buf[512];
                    while (fgets(c_line_buf, 512, tmp_c_file) != NULL) {
                        printf("%d ", ++c_line_count);
                        printf("%s", c_line_buf);
                    }
                }
                fclose(tmp_c_file);
                // usleep(10);
                sleep(1);
                // system("clear");
                if (c == 2) {
                    printf("%s converting is finished!\n", cfilename);
                }
                exit(0);
            }
            wait(NULL);
        }
    }

    if (arg_option_j) {
        FILE *fp;
        char buf[512]; // 한 라인을 위한 버퍼
        fp = fopen(pathname, "r");
        int lineCnt = 1;
        while ( fgets(buf, 512, fp) ) {
            printf("%d %s", lineCnt++, buf);
        }
    }
    if (arg_option_c) {
        FILE *fp;
        char buf[512];
        int lineCnt;
        // 생성된 n 파일을 모두 보여줘야함.
        for (int i = 0; i < produces_c_source_file_numbers; i++) {
            lineCnt = 1;
            fp = fopen(c_source_file_names[i], "r");
            printf("----%s----\n", c_source_file_names_print[i]);
            while ( fgets(buf, 512, fp) ) {
                printf("%d %s", lineCnt++, buf);
            }
            printf("------------\n");
        }
    }
    if (arg_option_p) {
        int j = 0;
        for (int i = 0; i < 7; i++) {
            if (cngCnt[i] == 1) {
                printf("%d %s\n", ++j, cngPatt[i]);
            }
        }
    }
    if (arg_option_f) {
        FILE *fp;
        long fsize;
        // get java file size
        fp = fopen(java_file_name, "r");
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        printf("%s file size is %ld bytes\n", filename, fsize);
        // 생성된 n 파일을 모두 보여줘야함.
        for (int i = 0; i < produces_c_source_file_numbers; i++) {
            fp = fopen(c_source_file_names[i], "r");
            fseek(fp, 0, SEEK_END);
            fsize = ftell(fp);
            printf("%s file size is %ld bytes\n", c_source_file_names_print[i], fsize);
        }
    }
    if (arg_option_l) {
        FILE *fp;
        long line = 0;
        char buf[512];
        // get java file size
        fp = fopen(java_file_name, "r");
        while ( fgets(buf, 512, fp) )
            ++line;
        printf("%s file line number is %ld lines\n", filename, line);
        // 생성된 n 파일을 모두 보여줘야함.
        for (int i = 0; i < produces_c_source_file_numbers; i++) {
            line = 0;
            fp = fopen(c_source_file_names[i], "r");
            while ( fgets(buf, 512, fp) )
                ++line;
            printf("%s file line number is %ld lines\n", c_source_file_names_print[i], line);
        }
    }
}
