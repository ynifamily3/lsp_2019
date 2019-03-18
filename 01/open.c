#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *pathname, int oflag);
int open(const char *pathname, int oflag, mode_t mode);

