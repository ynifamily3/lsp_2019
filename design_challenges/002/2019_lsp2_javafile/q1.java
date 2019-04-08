import java.util.Scanner; // pattern : 20으로 시작하고 109로 끝나는 매칭 => 생략

public class q1{ // pattern : 21 22 11 로 시작함 그리고 괄호 내부가 종속적임
	public static void main(String[] args){ // pattern : 21 23 24 11 103 25 118 11 104 패턴, 괄호 내부가 종속적임
		Scanner scn = new Scanner(System.in); // pattern : 26으로 시작함 (생략)

		System.out.printf("Enter the number : "); // pattern : 11 100 11 100 11 103 12 104 109 
		// 등등 패턴 테이블을 만들어 해당 라인으로 치환함. 패턴 불일치 시 소스코드 그냥 출력
		int num; // pattern : 30 11 109 그러나 이 문장은 패턴 테이블에 넣지 않고 그대로 사용해도 무방
		num = scn.nextInt(); // pattern : 11 117 11 100 31 103 104 109 이며,  scanf으로 치환
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
