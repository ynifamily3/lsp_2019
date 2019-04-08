#include <stdio.h>
// (생략) ==> pattern : 

// 생략 : 파일 생성 => q1.c
int main(int argc, char *args){
    //생략

    printf("Enter the number : "); //==> printf 사용했으므로 #include <stdio.h> 삽입

    int num;
    scanf("%d", &num);
    int even=0, odd=0;

    for(int i=1; i<=num; i++){
        if(i % 2 == 0){
            even+=i;
        }
        else{
            odd+=i;
        }
    }

    printf("Sum of Even number : %d\n", even);
    printf("Sum of Odd number : %d\n", odd);

    return 0;
}
// 생략 }