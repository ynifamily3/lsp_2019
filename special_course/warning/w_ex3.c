#include <stdio.h>
int foo();

int main(void)
{
    foo();
    return 0;
}

int foo()
{
    printf("foo\n");
}