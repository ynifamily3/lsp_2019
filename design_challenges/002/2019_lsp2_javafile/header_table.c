#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header_table.h"

/*
header = {"#define XX", "#include XX", "int pop(void);", "int push(int value);"}
등등

대응되는 함수가 왔을 때 include 로 연결을 해 준다.
set_header 
*/

static int number_of_heder_tables = 0;
static _header_table *header_table; // 헤더 필요함 함수 종류가 몇 개인가.

static int number_of_headers = 0;
static char **header;

void addHeader(const char *header_value)
{
    for (int i = 0; i < number_of_headers; ++i) {
        if (strcmp(header[i], header_value) == 0) {
            return; // 중복되는 헤더 발견시 추가하지 않음.
        }
    }

    size_t allocate = strlen(header_value) + 1;
    header[number_of_headers] = (char *)calloc(allocate, sizeof(char));
    strncpy(header[number_of_headers++], header_value, allocate);

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
    fp = fopen(header_table_fname, "r");
    // 파일을 읽어 무조건 addHeader 호출
    // 한 라인을 읽고
    fclose(fp);
}