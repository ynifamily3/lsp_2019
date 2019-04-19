#ifndef ARG_READ
#define ARG_READ
#include <stdbool.h>

// options
// 변환할 Java 언어 프로그램 코드 출력
bool arg_option_j;
// 변환된 C 언어 프로그램 출력
bool arg_option_c;
// Java 언어 프로그램에서 사용된 함수들을 C 언어 프로그램에서 대응되는 함수와 함께 출력
bool arg_option_p;
// Java 언어 프로그램 파일 미 C 언어 프로그램 파일의 파일 크기 출력
bool arg_option_f;
// Java 언어 프로그램 파일 및 C 언어 프로그램 파일의 라인 수 출력
bool arg_option_l;
/*  자식 프로세스를 생성하여 Java 언어 프로그램이 변환되는 과정을 한 줄 한 줄 출력.
 * 단, Java 언어 프로그램의 매 statement 가 C 언어 프로그램으로 변환되어 출력되고 터미널 화면이 clear 되고
 * 다시 다음 statement 변환 및 출력. 변환이 완료되면 파일명.c converting is finished! 출력
 * 가장 먼저 수행된다.
*/
bool arg_option_r;

char *java_file_name;

// argument option 파싱을 한다.
void parse_args(int argc, char *argv[]);




#endif
