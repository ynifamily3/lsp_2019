#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
    struct timeval begin, end;

    gettimeofday(&begin, NULL);
    if (argc != 3) {
        fprintf(stderr, "usage : %s <oldname> <newname>\n", argv[0]);
        exit(1);
    }

    if (link(argv[1], argv[2]) < 0) {
        fprintf(stderr, "link error\n");
        exit(1);
    }
    else
        printf("step1 passed.\n");
    
    if (remove(argv[1]) < 0) {
        fprintf(stderr, "remove error\n");
        remove(argv[2]);
        exit(1);
    }
    else
        printf("step2 passed.\n");
    printf("Success!\n");
    gettimeofday(&end, NULL);
    ssu_runtime(&begin, &end);
    exit(0);
}
