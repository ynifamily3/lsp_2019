#ifndef ARG_READ
#define ARG_READ
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>

// options
const char *student_dir;
const char *answer_dir;
bool arg_option_c;
bool arg_option_e;
bool arg_option_t;
bool arg_option_p;
bool arg_option_h;

// for c, e, t args store
// e option must gets one arg
int arg_option_c_argc;
char **arg_option_c_argv;

int arg_option_e_argc;
char **arg_option_e_argv;

int arg_option_t_argc;
char **arg_option_t_argv;

void parse_args(int argc, char *argv[]);




#endif
