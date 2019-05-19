#ifndef BACKUP
#define BACKUP
void twae(const char *absolute_dir, int period, int option_m, int option_n, int maxn, int option_t, int store_time);
void add_command_action(int argc, char **argv);
void list_command_action(void);
void remove_command_action(int argc, char **argv);
void backup_list_init(void);
#endif