#ifndef COMMAND_CHECKER
#define COMMAND_CHECKER

int check_exit(char *command);
int check_ls(char *command);
int check_vim(char *command);
int check_add(char *command);
int check_list(char *command);
int check_remove(char *command);
int check_compare(char *command);
int check_recover(char *command);


void parse_args(char *input_text, int *arg_count, char **arg_vector);

#endif