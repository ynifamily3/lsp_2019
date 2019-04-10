#include <stdio.h>

void f(const char *str)
{
    char *s = (char *)str;
    s[0] = '\0';
}
int main(void)
{
    char *a = "hello World";
    f(a);
    return 0;
}