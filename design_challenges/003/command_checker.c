#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "command_checker.h"

int check_exit(const char *command)
{
    if (strcmp("exit", command) == 0) {
        return 1;
    }
    return 0;
}

int check_ls(const char *command)
{
    size_t len = strlen(command);
    if ((len == 2 && command[0] == 'l' && command[1] == 's') || (len >= 3 && command[0] == 'l' && command[1] == 's' && command[2] == ' ')) {
        return 1;
    }
    return 0;
}

int check_vim(const char *command)
{
    size_t len = strlen(command);
    if ((len == 2 && command[0] == 'v' && command[1] == 'i') || (len == 3 && command[0] == 'v' && command[1] == 'i' && command[2] == 'm') ||
        (len >= 3 && command[0] == 'v' && command[1] == 'i' && command[2] == ' ')||(len >= 4 && command[0] == 'v' && command[1] == 'i' && command[2] == 'm' && command[3] == ' ')) {
            return 1;
    }
    return 0;
}

int check_add(const char *command)
{
    size_t len = strlen(command);
    if ((len >= 3 && command[0] == 'a' && command[1] == 'd' && command[2] == 'd' && command[3] == '\0') ||
        (len >= 4 && command[0] == 'a' && command[1] == 'd' && command[2] == 'd' && command[3] == ' ')) {
        return 1;
    } else {
        return 0;
    }
}

int check_list(const char *command)
{
    if (strcmp("list", command) == 0) {
        return 1;
    }
    return 0;
}

int check_remove(const char *command)
{
    // i) -d 옵션 없을 경우
    size_t len = strlen(command);
    if ((len >= 6 && command[0] == 'r' && command[1] == 'e' && command[2] == 'm' && command[3] == 'o' && command[4] == 'v' && command[5] == 'e' && command[6] == '\0') ||
        (len >= 7 && command[0] == 'r' && command[1] == 'e' && command[2] == 'm' && command[3] == 'o' && command[4] == 'v' && command[5] == 'e' && command[6] == ' ')) {
        return 1;
    } else {
        return 0;
    }
}

int check_compare(const char *command)
{
    size_t len = strlen(command);
    if ((len >= 7 && command[0] == 'c' && command[1] == 'o' && command[2] == 'm' && command[3] == 'p' && command[4] == 'a' && command[5] == 'r' && command[6] == 'e' && command[7] == '\0') ||
        (len >= 8 && command[0] == 'c' && command[1] == 'o' && command[2] == 'm' && command[3] == 'p' && command[4] == 'a' && command[5] == 'r' && command[6] == 'e' && command[7] == ' ')) {
        return 1;
    } else {
        return 0;
    }
}

int check_recover(const char *command)
{
    size_t len = strlen(command);
    if ((len >= 7 && command[0] == 'r' && command[1] == 'e' && command[2] == 'c' && command[3] == 'o' && command[4] == 'v' && command[5] == 'e' && command[6] == 'r' && command[7] == '\0') ||
        (len >= 8 && command[0] == 'r' && command[1] == 'e' && command[2] == 'c' && command[3] == 'o' && command[4] == 'v' && command[5] == 'e' && command[6] == 'r' && command[7] == ' ')) {
        return 1;
    } else {
        return 0;
    }
}

void parse_args(char *input_text, int *arg_count, char **arg_vector)
{
    // 동적 메모리 할당을 함.
    // add /file/path/and/name.txt ...
    // 띄어쓰기 단위로 구분함.
    *arg_count = 0;
    char *ptr = input_text;
    char *ptr2;
    int length, i;
    while (*ptr != '\0') {
        while (isspace(*ptr)) ++ptr; // 공백 제거
        // 문자열의 파싱 ( \0이 나오거나 띄어쓰기가 나올 때까지)
        ptr2 = ptr;
        while (!isspace(*ptr2) && *ptr2 != '\0') ++ptr2;
        length = ptr2 - ptr;
        arg_vector[*arg_count] = malloc((length+1) * sizeof(char));
        for (i = 0; i < length; i++) {
            arg_vector[*arg_count][i] = *(ptr + i);
        }
        arg_vector[*arg_count][length] = '\0';
        ++(*arg_count);
        ptr = ptr2;
        if (*arg_count >= 11) {
            break;
        }
    }
}