#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
  echo "Do not run script directly."
  exit 1
fi

echo "[자바 테스트 디렉토리 생성]"
mkdir test_java_dir

echo "[자바 코드 컴파일]"
javac java/q0.java -d test_java_dir
javac java/q1.java -d test_java_dir
javac java/q2.java -d test_java_dir
javac java/q3.java -d test_java_dir
javac java/q4.java -d test_java_dir

echo "[디렉토리 이동]"
cd test_java_dir

echo "[실행]"
java q0 > q0_result
echo "10" | java q1 > q1_result
java q2 > q2_result
java q3 > q3_result
java q4 > q4_result

echo "[디렉토리 이동]"
cd ../

echo "[C 테스트 디렉토리 생성]"
mkdir test_c_dir

echo "[자바 파일 복사]"
cp java/q0.java test_c_dir/q0.java
cp java/q1.java test_c_dir/q1.java
cp java/q2.java test_c_dir/q2.java
cp java/q3.java test_c_dir/q3.java
cp java/q4.java test_c_dir/q4.java

echo "[자바 파일 변환]"
./ssu_convert test_c_dir/q0.java -c
sleep 3
./ssu_convert test_c_dir/q1.java -j
sleep 3
./ssu_convert test_c_dir/q2.java -r -l -f
sleep 3
./ssu_convert /usr/src/ssu_convert/test_c_dir/q3.java -p
sleep 3
./ssu_convert test_c_dir/q4.java -c
sleep 3

echo "[디렉토리 이동]"
cd test_c_dir

echo "[C 코드 컴파일]"
make -f q0_Makefile
make -f q1_Makefile
make -f q2_Makefile
make -f q3_Makefile
make -f q4_Makefile

echo "[실행]"
./q0 > q0_result
echo "10" | ./q1 > q1_result
./q2 > q2_result
./q3 > q3_result
./q4 > q4_result

echo "[디렉토리 이동]"
cd ../

echo "[결과 비교]"
diff "test_java_dir/q0_result" "test_c_dir/q0_result"
diff "test_java_dir/q1_result" "test_c_dir/q1_result"
diff "test_java_dir/q2_result" "test_c_dir/q2_result"
diff "test_java_dir/q3_result" "test_c_dir/q3_result"
diff "test_java_dir/q3java.txt" "test_c_dir/q3java.txt"
diff "test_java_dir/q4_result" "test_c_dir/q4_result"

sleep 0.3

echo "[테스트 파일 제거]"
rm -rf test_java_dir test_c_dir
