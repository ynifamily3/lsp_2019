#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header_table.h"

static int number_of_header_tables = 0; // 곧, 함수의 개수를 뜻함
static _header_table **header_table; // 헤더 필요한 함수 종류가 몇 개인가.

static int number_of_headers = 0;
static char **header; // #include <xx> #include <yy> 등이 저장됨
static int *header_ref_count; // 몇번 카운트 되었는가.

int addFunc(const char *func_value)
{
    size_t allocate = strlen(func_value) + 1;
    header_table[number_of_header_tables] = (_header_table *)calloc(allocate, sizeof(_header_table));
    strncpy(header_table[number_of_header_tables]->func_name, func_value, allocate);
    header_table[number_of_header_tables++]->num_includes = 0; // 0으로 초기화
    return number_of_header_tables - 1; // 해당 인덱스 반환
}

int addHeader(const char *header_value)
{
    for (int i = 0; i < number_of_headers; ++i) {
        if (strcmp(header[i], header_value) == 0) {
            return i; // 중복되는 헤더 발견시 추가하지 않고 그 주소를 리턴
        }
    }

    size_t allocate = strlen(header_value) + 1;
    header[number_of_headers] = (char *)calloc(allocate, sizeof(char));
    strncpy(header[number_of_headers++], header_value, allocate);
    return number_of_headers - 1; // 새로 헤더를 추가하고 그 주소 리턴
}

int findHeader(const char *header_value)
{
    // 해당 헤더가 어디 있는지 인덱스를 반환 없으면 -1 반환
    for (int i = 0; i < number_of_headers; ++i) {
        if (strcmp(header[i], header_value) == 0) {
            return i;
        }
    }
    return -1;
}

void read_header_table(const char *header_table_fname)
{
    FILE *fp;
    char line_buf[178];
    char func_name_buf[50];
    char headers_buf[128]; // #include <xx> 를 1단위로 봄

    // 함수가 50개쯤 있다고 가정한다.
    header_table = (_header_table **)calloc(50, sizeof(_header_table *));
    
    // 헤더파일은 70개쯤 있다고 가정한다.
    header = (char **)calloc(70, sizeof (char *));
    header_ref_count = (int *)calloc(70, sizeof(int));


    fp = fopen(header_table_fname, "r");
    if (!fp) {
        fprintf(stderr, "header_table.txt doesn't exitst\n");
    }
    // 메모리 잡는법
    while (fgets(line_buf, 178, fp) != NULL && line_buf[0] != '\n') {
        sscanf(line_buf, "%s", func_name_buf); // 띄어쓰기 전까지
        // printf("함수명 : %s\n", func_name_buf);
        int fn = addFunc(func_name_buf); // 함수를 테이블에 추가해 줌
        int nextPointer = 0;
        int hcnt;
        int limit = strlen(line_buf);
        int startInclude = strlen(func_name_buf) + 1;
        do {
            // printf("찾는 오프셋 : %d\n", startInclude + nextPointer);
            hcnt = sscanf(line_buf + startInclude + nextPointer, "%[^>]", headers_buf);
            unsigned int st = strlen(headers_buf);
            if(st <= 1) break;
            headers_buf[st] = '>';
            headers_buf[st + 1] = '\0';
            // printf("헤더 : %s\n", headers_buf);
            header_table[fn]->headers[header_table[fn]->num_includes++] = addHeader(headers_buf); // 헤더 테이블에 추가
            nextPointer += strlen(headers_buf) + 1;
        } while (hcnt && startInclude + nextPointer < limit);
    }
    // ref-count 초기화
    for (int i = 0; i < number_of_headers; i++) {
        header_ref_count[i] = 0;
    }
    fclose(fp);

    /*
    for (int i = 0; i < number_of_header_tables; ++i) {
        printf("%s\n", header_table[i]->func_name);
        for (int j = 0; j < header_table[i]->num_includes; ++j) {
            printf("- %s\n", header[header_table[i]->headers[j]]);
        }
    }
    */
    
}

void putheader(char *header_bank_tail_ptr, const char *c_func_name)
{
    // c함수이름이 오면 func_list에 있는지 보고 있으면 해당하는 헤더를 추가하는 명령을 내린다.
    for (int i = 0; i < number_of_header_tables; ++i) {
        char *fnname = header_table[i]->func_name;
        if (strcmp(fnname, c_func_name) == 0) {
            for (int j = 0; j < header_table[i]->num_includes; ++j) {
                if ( header_ref_count[header_table[i]->headers[j]] == 0 ) {
                    header_ref_count[header_table[i]->headers[j]] = 1; // ref-count 올림 (중복추가 방지)
                    // 헤더들을 추가 해 줌
                    strcpy(header_bank_tail_ptr, header[header_table[i]->headers[j]]);
                    strcpy(header_bank_tail_ptr+strlen(header[header_table[i]->headers[j]]), "\n");
                }
            }
        }
    }
}

void reset_ref_count(void) {
    for (int i = 0; i < number_of_headers; i++)
        header_ref_count[i] = 0;
}