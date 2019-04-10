#include <stdio.h>

int main(void)
{
    char arr[] = "Hello World\n";
    for(int i = 0; i < sizeof(arr); i++)
        printf("%c", arr[i]);
    return 0;
}