#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "ssu_runtime.h"

int main(void)
{
    struct stat statbuf;
    struct timeval begin, end;
    char *fname1 = "ssu_file1";
    char *fname2 = "ssu_file2";
    gettimeofday(&begin, NULL);
    if (stat(fname1, &statbuf) < 0)
        fprintf(stderr, "stat error %s\n", fname1);
    
    if (chmod(fname1, (statbuf.st_mode & ~S_IXGRP) | S_ISUID) < 0)
        fprintf(stderr, "chmod error %s\n", fname1);
    
    if (chmod(fname2, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IXOTH) < 0)
        fprintf(stderr, "chmod error %s\n", fname2);
    gettimeofday(&end, NULL);
    ssu_runtime(&begin, &end);
    exit(0);
}
