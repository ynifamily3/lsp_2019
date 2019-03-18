#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    int sys_ret;
    sys_ret = system("./a.out");
    printf("%d\n", sys_ret);
    return 0;
}