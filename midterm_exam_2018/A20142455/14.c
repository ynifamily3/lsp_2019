#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int ssu_write(int fd, char *buf);

int main()
{
	char buf[128];
	char pattern[4] = "bcd";
	char *pos1 = buf, *pos2 = buf;
	char *fname_in = "in.txt";
	char *fname_out = "out.txt";
	int size;
	int fd1, fd2; // fd1 is input, fd2 is output
	int i = 0;

	fd1 = open(fname_in, O_RDONLY);
	size = lseek(fd1, 0, SEEK_END);
	lseek(fd1, 0, SEEK_SET);
	read(fd1, buf, size);
	close(fd1);

	fd2 = open(fname_out, O_WRONLY|O_CREAT|O_TRUNC, 0644); // S_IRUSR...
	// hole 메우기
	while ( (pos2 = strstr(pos1, pattern)) != NULL ) {
		// hole 전까지 쓰기
		*pos2 = '\0';
		ssu_write(fd2, pos1); // buf + 0
		// lseek 를 빼먹었구나 ^ㅁ^
		lseek(fd2, strlen(pattern), SEEK_CUR); // 실질적으로 hole만큼 파일 포인터 이동
		pos1 = pos2 + strlen(pattern);
		// hole 이후

	}
	ssu_write(fd2, pos1); // 나머지 내용


	close(fd2);
	
	return 0;
}

int ssu_write(int fd, char *buf)
{
	return write(fd, buf, strlen(buf));
}
