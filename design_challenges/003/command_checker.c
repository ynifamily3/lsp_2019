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