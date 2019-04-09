#include <stdio.h>
#include "pattern.h"

#define NUMBER_OF_C_CODES // h 파일로 이주

// int exchangers[from_java] = to_java;

const char *C_codes[] = {
    "scanf", "\"%d\"", ",", "&", "*", "calloc", "(", ")", "sizeof", " "
};

int is_match(int numPattern, _patternChanger **__patt, _lexPattern *pattern)
{
    // 매칭시 해당 패턴 인덱스, 미 매칭시 -1
    for (int i = 0; i < numPattern; i++) {
        _patternChanger *patt = __patt[i];
        int matched = 1;
        for (int j = 0; j < pattern->pattern_length; j++) {
            if (pattern->pattern[j] != patt->java_pattern[j]) {matched = 0; break;}
        }
        if (matched) return i;
    }
    return -1;
}

void convertJavatoC(_patternChanger *patt, char *resultbuf)
{
    memset(resultbuf, 0x00, 100); // debug
    for (int i = 0; i < patt->c_pattern_length; i++) {
        if (patt->c_pattern[i] >= 1000) {
            strcat(resultbuf, C_codes[patt->c_pattern[i]-1000]);
        } else {
            strcat(resultbuf, patt->java_pattern_buffer[ patt->c_pattern[i] ]);
        }
    }
}