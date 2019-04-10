import java.util.Scanner;
// 아니 이게 왜 안 나와
public class q1{
	public static void main(String[] args){
		Scanner scn = new Scanner(System.in);

		System.out.printf("Enter the number : ");

		int num;
		num = scn.nextInt();
		int even=0, odd=0;

		for(int i=1; i<=num; i++){ // Checking...
			if(i % 2 == 0){
				even+=i;
			}
			else{
				odd+=i;
			}
		} /**/ //
		/*
		내주석에 불만 있나요?
		불만 있으면 california로 오십시오
		나는 옥수수 만들어 줄 수 있다.
		*/

		System.out.printf("Sum of Even number : %d\n", even);
		System.out.printf("Sum of Odd number : %d\n", odd);

		return ;
	}
}
