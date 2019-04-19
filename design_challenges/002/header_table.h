#ifndef HEADER_TABLE
#define HEADER_TABLE
typedef struct {
    char func_name[50]; // 함수 이름
    int num_includes; // 인클루드 되는 헤더 파일 갯수 (최대 5)
    int headers[5]; // 해당하는 헤더의 인덱스 주소들
} _header_table;



void read_header_table(const char *header_table_fname);
void putheader(char *header_bank_tail_ptr, const char *c_func_name);
void reset_ref_count(void);
#endif