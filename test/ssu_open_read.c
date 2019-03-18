#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

int main(void)
{
    char *fname = "ssu_test.txt";
    int fd1, fd2;
    int count;
    char buf[BUFFER_SIZE];

    if ((fd1 = open(fname, O_RDONLY, 0644)) < 0) {
        fprintf(stderr, "open error for %s\n", fname);
        exit(1);
    }

    if ((count = lseek(fd1, 0, SEEK_END)) < 0) {
        fprintf(stderr, "lseek error\n");
        exit(1);
    }

    printf("%d\n", count); // file capacity
    lseek(fd1, 0, SEEK_SET); // rewind
    count = read(fd1, buf, BUFFER_SIZE); // max file size 1024 bytes
    fd2 = dup2(fd2, 1);
    close(fd1);
    write(fd2, buf, count);
    exit(0);
}