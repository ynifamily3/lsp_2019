#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header_table.h"

static int number_of_heder_tables = 0;
static _header_table **header_table; // 헤더 필요함 함수 종류가 몇 개인가.

static int number_of_headers = 0;
static char **header; // #include <xx> #include <yy> 등이 저장됨
static int *header_ref_count; // 몇번 카운트 되었는가.

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
    // 메모리 잡는법
    while (fgets(line_buf, 178, fp) != NULL) {
        sscanf(line_buf, "%s", header[number_of_heder_tables++]); // 띄어쓰기 전까지
        int cnts;
        // while (cnts = sscanf(line_buf+strlen(header[number_of_heder_tables-1], "[^ ]")))
    }

    fclose(fp);
}