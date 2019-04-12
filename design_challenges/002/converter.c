#include "lex.h"
#include "pattern.h"

void convert_java_to_c(char *output, const char *input)
{
    _lexPattern pattern;
    lex(&pattern, input);
    // PATT_pattern_compile(&pattern, output); ==> 생략
}