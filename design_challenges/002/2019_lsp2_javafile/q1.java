import java.util.Scanner;
/*** 이건 어떰 *************/
/*==**/
/*
* 이런 경우
*/
// 정상
//노멀
////// 이상함
//=하하 죽어라
public class q1{
	nini p;
	public static void main(String[] args){
		Scanner scn = new Scanner(System.in);

		System.out.printf("Enter the number : ");/*
		*/
		System.out.printf("Enter the number : ", 1, 2, 3, 4, 5, "6", "77", "888");
		/* multiline 1 */
		/*
		multilime 2
		*/
		else if{

		}
		else
		if (){}

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
		}

		System.out.printf("Sum of Even number : %d\n", even);
		System.out.printf("Sum of Odd number : %d\n", odd);

		return ;
	}
}

