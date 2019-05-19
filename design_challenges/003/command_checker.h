#ifndef COMMAND_CHECKER
#define COMMAND_CHECKER

int check_exit(const char *command);
int check_ls(const char *command);
int check_vim(const char *command);
int check_add(const char *command);
int check_list(const char *command);
int check_remove(const char *command);


void parse_args(char *input_text, int *arg_count, char **arg_vector);

#endif