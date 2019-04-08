#include <stdio.h>
#include "pattern.h"

void pattern_matcher_init() {
    // 11 117 11 100 31 103 104 109
    // num=scn.nextInt(); ==> scanf("%d", &num);
    _patternChanger patt;
    patt.java_pattern_length = 8;
    patt.c_pattern_length = 8;
    patt.java_pattern[0] = 11;
    patt.java_pattern[1] = 117;
    patt.java_pattern[2] = 11;
    patt.java_pattern[3] = 100;
    patt.java_pattern[4] = 31;
    patt.java_pattern[5] = 103;
    patt.java_pattern[6] = 104;
    patt.java_pattern[7] = 109;

    patt.c_pattern[0] = "scanf code";
    patt.c_pattern[1] = "(";
    patt.c_pattern[2] = "큰따옴표 포함한 %d";
    patt.c_pattern[3] = ",";
    patt.c_pattern[4] = "&";
    patt.c_pattern[5] = "num";
    patt.c_pattern[6] = ")";
    patt.c_pattern[7] = ";";
}