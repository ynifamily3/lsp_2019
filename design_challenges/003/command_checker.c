#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "command_checker.h"

/*
void str_rstrip(str* s)
{
  unsigned len;
  for (len = s->len; len > 0 && isspace(s->s[len-1]); --len) ;
  s->len = len;
  s->s[len] = 0;
}
*/

// 리턴 값 : lstrip 한 만큼 이동된 포인터, 변경 값 : rstrip 한 \0
char *strip(char *command)
{
    int start_ptr = 0;
    // rstrip
    for (int i = (int)strlen(command) - 1; i != 0 ;i--) {
        if (!isspace(command[i])) {
            command[i + 1] = '\0';
            break;
        }
    }

    // lstrip
    while (command[start_ptr] != '\0') {
        if (isspace(command[start_ptr])) ++start_ptr;
        else break;
    }
    return &command[start_ptr];
}

int check_exit(char *command)
{
    char *striped = strip(command);
    if (strcmp("exit", striped) == 0) {
        return 1;
    }
    return 0;
}

int check_ls(char *command)
{
    char *striped = strip(command);
    size_t len = strlen(striped);
    if ((len == 2 && striped[0] == 'l' && striped[1] == 's') || (len >= 3 && striped[0] == 'l' && striped[1] == 's' && striped[2] == ' ')) {
        return 1;
    }
    return 0;
}

int check_vim(char *command)
{
    char *striped = strip(command);
    size_t len = strlen(striped);
    if ((len == 2 && striped[0] == 'v' && striped[1] == 'i') || (len == 3 && striped[0] == 'v' && striped[1] == 'i' && striped[2] == 'm') ||
        (len >= 3 && striped[0] == 'v' && striped[1] == 'i' && striped[2] == ' ')||(len >= 4 && striped[0] == 'v' && striped[1] == 'i' && striped[2] == 'm' && striped[3] == ' ')) {
            return 1;
    }
    return 0;
}

int check_add(char *command)
{
    char *striped = strip(command);
    size_t len = strlen(striped);
    if ((len >= 3 && striped[0] == 'a' && striped[1] == 'd' && striped[2] == 'd' && striped[3] == '\0') ||
        (len >= 4 && striped[0] == 'a' && striped[1] == 'd' && striped[2] == 'd' && striped[3] == ' ')) {
        return 1;
    } else {
        return 0;
    }
}

int check_list(char *command)
{
    char *striped = strip(command);
    if (strcmp("list", striped) == 0) {
        return 1;
    }
    return 0;
}

int check_remove(char *command)
{
    char *striped = strip(command);
    size_t len = strlen(striped);
    if ((len >= 6 && striped[0] == 'r' && striped[1] == 'e' && striped[2] == 'm' && striped[3] == 'o' && striped[4] == 'v' && striped[5] == 'e' && striped[6] == '\0') ||
        (len >= 7 && striped[0] == 'r' && striped[1] == 'e' && striped[2] == 'm' && striped[3] == 'o' && striped[4] == 'v' && striped[5] == 'e' && striped[6] == ' ')) {
        return 1;
    } else {
        return 0;
    }
}

int check_compare(char *command)
{
    char *striped = strip(command);
    size_t len = strlen(striped);
    if ((len >= 7 && striped[0] == 'c' && striped[1] == 'o' && striped[2] == 'm' && striped[3] == 'p' && striped[4] == 'a' && striped[5] == 'r' && striped[6] == 'e' && striped[7] == '\0') ||
        (len >= 8 && striped[0] == 'c' && striped[1] == 'o' && striped[2] == 'm' && striped[3] == 'p' && striped[4] == 'a' && striped[5] == 'r' && striped[6] == 'e' && striped[7] == ' ')) {
        return 1;
    } else {
        return 0;
    }
}

int check_recover(char *command)
{
    char *striped = strip(command);
    size_t len = strlen(striped);
    if ((len >= 7 && striped[0] == 'r' && striped[1] == 'e' && striped[2] == 'c' && striped[3] == 'o' && striped[4] == 'v' && striped[5] == 'e' && striped[6] == 'r' && striped[7] == '\0') ||
        (len >= 8 && striped[0] == 'r' && striped[1] == 'e' && striped[2] == 'c' && striped[3] == 'o' && striped[4] == 'v' && striped[5] == 'e' && striped[6] == 'r' && striped[7] == ' ')) {
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
    input_text = (char *)strip(input_text);
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