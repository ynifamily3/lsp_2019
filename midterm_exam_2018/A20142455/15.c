#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define DIRECTORY_SIZE MAXNAMLEN // 디렉토리 이름 길이 매크로
#define isdigit(x) (x>='0'&&x<='9')?1:0 // 숫자 판단 매크로
#define MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH // 권한 매크로

int create_dir(int depth, char *cur_dir); //디렉토리 생성 함수
void writefile(char *in_f, char *out_f); //파일을 복사하는 함수
void change_mod(const char *file_path); //모드를 변경하는 함수

char *fname = "ssu_test.txt"; // 생성하고 복사할 파일의 기본 이름
int o_flag = 0, e_flag = 0; // e옵션과 o옵션을 나타낼 플래그

int main(int argc, char *argv[]) {
	int opt; //옵션인자를 받을 변수
	int depth = 0; // 하위 디렉터리의 갯수를 받을 변수
	char cur_dir_name[DIRECTORY_SIZE] = {"\0",}; //현재 디렉토리 이름
	int fd;

	while ((opt = getopt(argc, argv, "e:o:")) != -1)
	{
		switch(opt)
		{
			case 'e':
				for (int i = 0; i < (int)strlen(optarg); i++)
				{
					if (!isdigit(optarg[i]))
					{
						fprintf(stderr, "put the depth N number.\n");
						exit(1);
					}
					depth = optarg[i] - '0';
				}
				e_flag = 1;
				break;

			case 'o':
				for (int i = 0; i < (int)strlen(optarg); i++)
				{
					if (!isdigit(optarg[i]))
					{
						fprintf(stderr, "put the depth N number.\n");
						exit(1);
					}
					depth = optarg[i] - '0';
				}
				o_flag = 1;
				break;
			case '?':
				break;
		}
	}

	if (argc < 3) // ./a.out 숫자 또는 ./a.out
	{
		printf("%s\n", argv[argc-1]);
		for (int i = 0; i < (int)strlen(argv[argc-1]); i++)
			if (isdigit(argv[argc-1][i]))
			{
				depth = argv[argc-1][i] - '0';
				break;
			}
	}
	else
		fprintf(stderr, "too many argv\n");

	if ((fd = creat(fname, MODE)) < 0) {
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}
	else
		close(fd);

	if (mkdir("0", 0755))
	{
		fprintf(stderr, "mkdir error\n");
		exit(1);
	}
	strcpy(cur_dir_name, "0"); //현재 디렉터리 이름 저장
	create_dir(depth, cur_dir_name);
	exit(0);
}

int create_dir(int depth, char *cur_dir)
{
	struct stat dir_st;
	int i = 0;
	char tmp_filename[MAXNAMLEN] = {'\0',};
	while (cur_dir[i] != '\0') i++; // 글자수 세기 
	cur_dir[i] = '/';
	i++;
	cur_dir[i] = depth + '0'; //깊이 저장 

	if (stat(cur_dir, &dir_st) < 0) {
		// 디렉토리가 존재하지 않으면 생성
		if (mkdir(cur_dir, 0777) < 0) {
			fprintf(stderr, " fail to make directory.\n");
			return -1;
		}
	}

	strcat(tmp_filename, cur_dir); // 왜 필요할까? (지역화 변수)

	if (o_flag && (depth % 2 == 0))
	{
		writefile(fname, strcat(tmp_filename, fname));
		change_mod(tmp_filename);
	}
	else if (e_flag && (depth % 2 == 1))
	{
		writefile(fname, strcat(tmp_filename, fname));
		change_mod(tmp_filename);
	}
	else if (!o_flag && !e_flag)
	{
		writefile(fname, strcat(tmp_filename, fname));
	}
	if (depth == 0)
		return 0;
	return create_dir(depth-1, cur_dir);
}

void writefile(char *in_f, char *out_f)
{
	int in_fd, out_fd;
	int read_count;
	char buf[MAXNAMLEN];

	in_fd = open(in_f, O_RDONLY);
	out_fd = open(out_f, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

	while ((read_count = read(in_fd, buf, sizeof(buf))) > 0)
		write(out_fd, buf, read_count);
}

void change_mod(const char *file_path)
{
	if (chmod(file_path, MODE) < 0)
		fprintf(stderr, "chmod error %s\n", file_path);
}

































