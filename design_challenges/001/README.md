# 설계과제 1
- ssu_score은 정답 파일을 기준으로 학생들이 제출한 답안 파일을 채점하는 프로그램이다.
## 주요 기능
- 학생들이 제출한 답안 디렉토리의 txt 파일들과 정답 파일의 txt 파일을 비교하면서 채점하고 맞으면 주어진 정답 파일에서 주어진 점수를 부여하고 틀리면 0점 부여
- 포인터 배열, 캐스팅 연산처럼 다수의 답이 정답이 될 경우 정답파일에 ‘콜론(:)’으로 구분하며, 학생들이 작성한 답안 파일을 비교할 때 다수의 답을 모두 정답으로 채점 -> 정답 파일에 새로운 답을 추가 및 삭제를 통해 학생들의 답안을 채점
- 연산자 사용으로 인해 다수의 답이 정답이 되는 경우 자동으로 동일한 연산식으로 자동으로 채점
- 단, 피연산자들의 순서가 바뀌었을 때 결과 값이 달라지는 문제는 지원하지 않음
- 연산자 우선순위을 기반으로 한 괄호도 정답으로 처리
- 문자열 간 공백은 구분하지 않음 -> 컴파일 시 에러가 나지 않는 경우 공백은 정답 처리
- ![image](https://user-images.githubusercontent.com/13795765/114253688-1f349680-99e6-11eb-84ca-998c21cf5f79.png)
- ![image](https://user-images.githubusercontent.com/13795765/114253693-2360b400-99e6-11eb-9a6c-194327495018.png)
## 과제 결과 보고서
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0001](https://user-images.githubusercontent.com/13795765/114253581-be0cc300-99e5-11eb-881d-bc0059b855d1.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0002](https://user-images.githubusercontent.com/13795765/114253583-c06f1d00-99e5-11eb-8c74-dc0682f2b46e.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0003](https://user-images.githubusercontent.com/13795765/114253585-c1a04a00-99e5-11eb-8e8a-c32845323657.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0004](https://user-images.githubusercontent.com/13795765/114253587-c238e080-99e5-11eb-94ad-77e30c5c08fa.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0005](https://user-images.githubusercontent.com/13795765/114253588-c2d17700-99e5-11eb-8f95-bd2810e32241.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0006](https://user-images.githubusercontent.com/13795765/114253590-c36a0d80-99e5-11eb-98ea-3058b641b900.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0007](https://user-images.githubusercontent.com/13795765/114253592-c402a400-99e5-11eb-9419-31780f439ac4.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0008](https://user-images.githubusercontent.com/13795765/114253593-c402a400-99e5-11eb-8d5d-e3e86f3a4a60.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0009](https://user-images.githubusercontent.com/13795765/114253595-c49b3a80-99e5-11eb-94d7-99fcd0449cd1.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0010](https://user-images.githubusercontent.com/13795765/114253596-c533d100-99e5-11eb-9bd5-12b8a512d895.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0011](https://user-images.githubusercontent.com/13795765/114253597-c533d100-99e5-11eb-84cc-73e75640004b.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0012](https://user-images.githubusercontent.com/13795765/114253598-c5cc6780-99e5-11eb-9d0d-b9374b990c47.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0013](https://user-images.githubusercontent.com/13795765/114253599-c5cc6780-99e5-11eb-8ab8-bbb5e1904a46.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0014](https://user-images.githubusercontent.com/13795765/114253600-c664fe00-99e5-11eb-8b84-0ea925aaab8c.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0015](https://user-images.githubusercontent.com/13795765/114253602-c664fe00-99e5-11eb-9c4e-cbdcef68c1e7.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0016](https://user-images.githubusercontent.com/13795765/114253603-c6fd9480-99e5-11eb-845a-f31c33feea9f.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0017](https://user-images.githubusercontent.com/13795765/114253604-c6fd9480-99e5-11eb-9eb8-f7e693eee4a2.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0018](https://user-images.githubusercontent.com/13795765/114253605-c7962b00-99e5-11eb-8148-4c156dddbfcb.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0019](https://user-images.githubusercontent.com/13795765/114253607-c82ec180-99e5-11eb-99bb-8b475fa37336.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0020](https://user-images.githubusercontent.com/13795765/114253609-c82ec180-99e5-11eb-8923-15d864a463ed.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0021](https://user-images.githubusercontent.com/13795765/114253610-c8c75800-99e5-11eb-8ad6-44cff5ff8abc.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0022](https://user-images.githubusercontent.com/13795765/114253611-c8c75800-99e5-11eb-9e35-f26a604beba1.png)
![ca0c6087-4b6f-4682-a5c1-8920661d9e40 hwp-0023](https://user-images.githubusercontent.com/13795765/114253612-c95fee80-99e5-11eb-8a3c-b994f491d152.png)
