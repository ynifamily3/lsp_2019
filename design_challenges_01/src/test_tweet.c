#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>

int main(void)
{
    int fd, fd2;
    fd = creat("haha.txt", 0666);
    fd2 = creat("iserrhaha.txt", 0666);
    dup2(fd, 1);
    dup2(fd2, 2);
    printf("tweet....");
    sleep(3);
    exit(0);
}