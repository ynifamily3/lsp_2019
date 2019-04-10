#include <stdio.h>
#define NUM 5
int score[NUM];
int sum(int cnt){
int i;
int sum;
for(i = 0; i < cnt ; i++){
sum += score[i];
}
return sum;
}
int main()
{
int i = 0;
int cnt = 0;
printf("input scores. input -1 to finish.\n");
for(i = 0;i < NUM;i++) {
printf("score #%d : ", cnt+1);
scanf("%d", score[cnt]);
if(score[cnt] == -1)
break;
cnt++;
}
printf("%d scores read.\n", cnt);
printf("--- result ---\n");
printf("sum : %d avg : %d\n", sum(cnt), sum(cnt)/cnt);
return 0;
}
