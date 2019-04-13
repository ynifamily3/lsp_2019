typedef struct {
    int ref_count; // 이 함수가 쓰인 횟수
    char func_name[50]; // 대응되는 함수 이름
    int num_includes; // 인클루드 되는 헤더 파일 갯수
    char *includes[10]; // # include <xxx> 들의 포인터들
} _header_table;

void read_header_table(const char *header_table_fname);