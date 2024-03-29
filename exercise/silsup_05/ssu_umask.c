#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "ssu_runtime.h"

#define RW_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
// RW_ RW_ RW_

int main(void)
{
    struct timeval begin, end;
    char *fname1 = "ssu_file1";
    char *fname2 = "ssu_file2";

    gettimeofday(&begin, NULL);

    umask(0);

    if (creat(fname1, RW_MODE) < 0) {
        fprintf(stderr, "creat error for %s\n", fname1);
        exit(1);
    }
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    // 그룹과 오더의 RW를 마스크

    if (creat(fname2, RW_MODE) < 0) {
        fprintf(stderr, "creat error for %s\n", fname2);
        exit(1);
    }
    
    gettimeofday(&end, NULL);
    ssu_runtime(&begin, &end);
    
    exit(0);
}
