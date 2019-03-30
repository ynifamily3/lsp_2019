#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

#include "arg_read.h"
#include "print_helps.h"
#include "ssu_runtime.h"
#include "parse_tree.h"

#define DIRECTORY_SIZE MAXNAMLEN

#define WAIT_TIME 5
#define WARNING_DEDUCTION_POINT 0.1
#define ERROR_POINT 0.0


char **answer_directory;
int *problem_type; // 0 for text , 1 for c
char **answers;
bool no_mark;
int number_of_questions = 0;
int number_of_students = 0;
char **students; 
bool is_time_limited = false;
double **scores; // 학생들이 받은 점수들
double *s_scores; // 각 학생이 받은 점수 (개별 학생의 총합)
double *indiv_score; // 문제를 풀면 받는 점수

int compare(const void *a, const void *b);
double compile_and_return_result(int studnt_index, int question_index, char *dirname);
double codeCMP(int question_index, char *dirname);
void *mark_thread(void *arg);
void *wait_thread(void *arg);
int strcmp2(char *a, char *b, int tol);
void check_score_csv();
void make_student_score_table();
void set_students_info();
void normalize(char *text);
void extract_answer(int index, char *ansdir);
void open_answer_set();
int mark_student(int student_index);


struct thread_args {
	char *gcc_command;
	pthread_t *target_tid;
	bool isError;
};

struct thread_args t_args;

void *mark_thread(void *arg)
{
	int isE;
	struct thread_args *args = (struct thread_args *) arg;
	isE = system(args->gcc_command);
	if (isE != 0)
		args->isError = true;
	pthread_cancel(*(args->target_tid));
	return NULL;
}

void *wait_thread(void *arg)
{
	sleep(WAIT_TIME);
	is_time_limited = true;
	// fprintf(stderr, "%ld", *(pthread_t *)arg);
	pthread_cancel(*(pthread_t *)arg);
	return NULL;
}

void check_score_csv()
{
	// 정답 폴더에 csv 파일이 있는지 체크한다. 현재 폴더는 ... 폴더 내에 있다.
	chdir("..");
	chdir(answer_dir);
	char pathname[512];
	// struct stat statbuf;
	sprintf(pathname, "score_table.csv");
	// score_table.csv 미 존재시 메시지를 띄우고 생성한다.
	if ( access(pathname, F_OK) < 0 ) {
		int select_type;
		printf("score_table.csv file doesn't exist in %s!\n", answer_dir);
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		do {
			printf("select type >> ");
			scanf("%1d", &select_type);
		} while(select_type != 1 && select_type != 2);
		if (select_type == 1) {
			double program_score = 0.0;
			double blank_score = 0.0;
			printf("Input value of blank question : ");
			scanf("%lf", &blank_score);
			printf("Input value of program question : ");
			scanf("%lf", &program_score);
			// make my csv file
			int csv_fd;
			if ( (csv_fd = creat(pathname, 0666)) < 0) {
				fprintf(stderr, "Error for generating %s\n", pathname); exit(1);
			}
			char fn[256];
			for (int i = 0; i < number_of_questions; i++) {
				if (problem_type[i] == 0) {
					indiv_score[i] = blank_score; // 변수에 낭낭하게 저장하여 나중에 활용
					// printf("text\t");
					sprintf(fn, "%s.txt,", answer_directory[i]);
					// printf("%s\n", fn);
					write(csv_fd, fn, strlen(fn));
					sprintf(fn, "%.2lf\n", blank_score);
					// printf("%s\n", fn);
					write(csv_fd, fn, strlen(fn));
				} else {
					indiv_score[i] = program_score; // 변수에 낭낭하게 저장하여 나중에 활용
					sprintf(fn, "%s.c,", answer_directory[i]);
					// printf("%s\n", fn);
					write(csv_fd, fn, strlen(fn));
					sprintf(fn, "%.2lf\n", program_score);
					// printf("%s\n", fn);
					write(csv_fd, fn, strlen(fn));
				}
			}
			close(csv_fd);
		} else if (select_type == 2) {
			// make my csv file
			int csv_fd;
			char fn[256];
			if ( (csv_fd = creat(pathname, 0666)) < 0) {
				fprintf(stderr, "Error for generating %s\n", pathname); exit(1);
			}
			for (int i = 0; i < number_of_questions; i++) {
				if (problem_type[i] == 0) {
					printf("Input of %s.txt: ", answer_directory[i]);
					scanf("%lf", &indiv_score[i]);					
					sprintf(fn, "%s.txt,", answer_directory[i]);
					write(csv_fd, fn, strlen(fn));
					sprintf(fn, "%.2lf\n", indiv_score[i]);
					write(csv_fd, fn, strlen(fn));

				} else {
					printf("Input of %s.c: ", answer_directory[i]);
					scanf("%lf", &indiv_score[i]);
					sprintf(fn, "%s.c,", answer_directory[i]);
					write(csv_fd, fn, strlen(fn));
					sprintf(fn, "%.2lf\n", indiv_score[i]);
					write(csv_fd, fn, strlen(fn));
				}
			}
			close(csv_fd);
		} else {
			fprintf(stderr, "select error\n"); exit(1);
		}
	} else {
		// 이미 csv 파일이 있을 경우 데이터만 읽어오면 된다.
		FILE *fp;
		if ( !(fp = fopen(pathname, "r")) ) {
			fprintf(stderr, "error for open score_table csv : %s\n", pathname);
			exit(1);
		}
		char line_buf[100];
		int score_ptr = 0;
		while (fgets(line_buf, sizeof(line_buf), fp) != NULL) {
			if(score_ptr >= number_of_questions) break;
			sscanf(line_buf, "%*[^,],%lf", &indiv_score[score_ptr++]);
		}
		fclose(fp);
	}
	chdir("..");
	chdir(student_dir);
}

void set_students_info()
{
	// sets number_of_students, students, scores
	struct dirent *dentry;
	struct stat statbuf;
	DIR *dirp;
	int i;
	chdir(".."); // rewind
	students = (char **)calloc(100, sizeof(char *));
	for (i = 0; i < 100; i++) {
		students[i] = (char *)calloc(100, sizeof(char));
	}
	if ((dirp = opendir(student_dir)) == NULL || chdir(student_dir) == -1) {
		// -c 옵션이 존재 할 경우 다른 곳으로 분기한다.
		if (arg_option_c) {
			no_mark = true;
			return;
		}
		fprintf(stderr, "Student's directory open failed : %s\n", strerror(errno));
		exit(1);
	}

	while ( (dentry = readdir(dirp)) != NULL ) {
		
		if (dentry->d_ino == 0 || dentry->d_name[0] == '.') continue;
		// printf("%s\n", dentry->d_name);
		if (stat(dentry->d_name, &statbuf) == -1) {
			fprintf(stderr, "stat error for %s : %s\n", dentry->d_name, strerror(errno));
			break;
		}
		memcpy(students[number_of_students++], dentry->d_name, 20);
	}
	closedir(dirp);
	scores = (double **)calloc(number_of_students, sizeof(double *));
	s_scores = (double *)calloc(number_of_students, sizeof(double));
	for (i = 0; i < number_of_students; i++) {
		scores[i] = (double *)calloc(number_of_questions, sizeof(double));
	}
	indiv_score = (double *)calloc(number_of_questions, sizeof(double));
	qsort((void *)students, number_of_students, sizeof(answer_directory[0]), compare);	
}

void normalize(char *text)
{
	int i, j;
	for (i = 0, j = 0; text[i] != 0; i++,j++) {
		// 텍스트 정답비교 문제로 개행문자까지 무시해 보았다.
		//if (text[i] == '\n' || !isspace(text[i])) {
		if (!isspace(text[i])) {
			// lower normalize
			char t = text[i];
			if (t >= 'A' && t <= 'Z') t += 32;
			text[j] = t;
		}
		else
			j--;
	}
	text[j] = '\0';
}


void extract_answer(int index, char *ansdir)
{
	chdir(ansdir);
	// open C or txt file
	int fd_txt, fd_c;
	char filename_txt[20];
	char filename_c[20];
	char gcc_command[100];
	char *p = "";
	sprintf(filename_txt, "%s.txt", ansdir);
	sprintf(filename_c, "%s.c", ansdir);
	fd_txt = open(filename_txt, O_RDONLY);
	fd_c = open(filename_c, O_RDONLY);
	if(fd_txt > 0) {
		// text mode
		// printf("text : %s\n", filename_txt);
		problem_type[index] = 0;
		ssize_t flength;
		ssize_t content_length;

		// stat file size
		/// will be filled

		flength = lseek(fd_txt, (off_t)0, SEEK_END);
		lseek(fd_txt, (off_t)0, SEEK_SET);
		answers[index] = (char *)calloc(flength + 1, sizeof(char)); // answer contents
		if( (content_length = read(fd_txt, answers[index], flength)) < 0) {
			fprintf(stderr, "reading file error... %s\n", gcc_command);
		}
		close(fd_txt); // no more
	}
	else if (fd_c > 0) {
		problem_type[index] = 1;
		// c mode
		// printf("c : %s\n", filename_c);
		close(fd_c); // no more....

		if(arg_option_t) {
			for (int i = 0;i < arg_option_t_argc; i++) {
				if (strcmp(ansdir, arg_option_t_argv[i]) == 0) {
						p = "-lpthread";
						break;
				}
			}
		}
		// printf("making answer .... %s \n", filename_c);
		sprintf(gcc_command, "gcc %s %s -o %s.exe", filename_c, p, ansdir);
		system(gcc_command);

		// file creat
		sprintf(gcc_command, "%s.stdout", ansdir);
		
		int stdout_fd, origin_fd;
		origin_fd = dup(1); // 0 : in 1 : out 2 : error
		if ((stdout_fd = creat(gcc_command, 0644)) < 0) {
			fprintf(stderr, "error create .... for %s\n", gcc_command);
			exit(1);
		}
		sprintf(gcc_command, "./%s.exe", ansdir);
		// write %s.stdout using dup
		dup2(stdout_fd, 1);
		system(gcc_command);
		// after write. close file.
		close(stdout_fd);
		dup2(origin_fd, 1);

		// open %s.stdout and put into answers[number_of_questions]
		ssize_t flength;
		ssize_t content_length;
		sprintf(gcc_command, "%s.stdout", ansdir);
		if ( (fd_c = open(gcc_command, O_RDONLY)) < 0) {
			fprintf(stderr, "open file error... %s\n", gcc_command);
			exit(1);
		}
		flength = lseek(fd_c, (off_t)0, SEEK_END);

		answers[index] = (char *)calloc(flength + 1, sizeof(char)); // answer contents

		lseek(fd_c, (off_t)0, SEEK_SET);
		if( (content_length = read(fd_c, answers[index], flength)) < 0) {
			fprintf(stderr, "reading file error... %s\n", gcc_command);
		}
		// normalize C program output answer
		normalize(answers[index]);
		close(fd_c);
	}
	else {
		fprintf(stderr, "No file in ans_dir : %s\n", ansdir);
		exit(1);
	}
	chdir("..");
}

int compare(const void *a, const void *b) 
{
	char *aString = *(char **)a;
	char *bString = *(char **)b;
	int diff;
	while ( *aString && *bString ) {
		if (isdigit(*aString) && isdigit(*bString))
			diff = strtol(aString, (char **)&aString, 10) - strtol(bString, (char**)&bString, 10);
		else
			diff = tolower( *aString ) - tolower( *bString);
		if (diff != 0) return diff;
		aString++;
		bString++;
	}
	return *aString - *bString;
}

void open_answer_set()
{
	// open answer directory, and cd into....
	struct dirent *dentry;
	struct stat statbuf;
	DIR *dirp;
	int i;
	answer_directory = (char **)calloc(100, sizeof(char *));
	answers = (char **)calloc(100, sizeof(char *));
	problem_type = (int *)calloc(100, sizeof(int));
	for (i = 0; i < 100; i++) {
		answer_directory[i] = (char *)calloc(20, sizeof(char));
		// answers[i] = (char *)calloc(1000, sizeof(char)); // answer contents
		// will be placed on extract_answer
		// will be filesize (answer file)
	}

	if ((dirp = opendir(answer_dir)) == NULL || chdir(answer_dir) == -1) {
		// -c 옵션이 존재 할 경우 다른 곳으로 분기한다.
		if (arg_option_c) {
			no_mark = true;
			return;
		}
		fprintf(stderr, "Answer Data open Error\n");
		exit(1);
	}

	while ( (dentry = readdir(dirp)) != NULL ) {
		if (dentry->d_ino == 0 || dentry->d_name[0] == '.' || dentry->d_type != 4) continue;

		if (stat(dentry->d_name, &statbuf) == -1) {
			fprintf(stderr, "stat error for %s\n", dentry->d_name);
			break;
		}
		// all of contents contain directory
		memcpy(answer_directory[number_of_questions++], dentry->d_name, 20);
	}
	closedir(dirp);
	// all of directorys must be sorted
	qsort((void *)answer_directory, number_of_questions, sizeof(answer_directory[0]), compare);
	for (i = 0; i < number_of_questions; i++) {
		extract_answer(i, answer_directory[i]); // momo
	}
}

// originally, this module is into mark_student.c
int mark_student(int student_index) {
    int i;
	
	if ( chdir(students[student_index]) < 0 ) {
		fprintf(stderr, "cannot find student directory %s\n", students[student_index]);
		exit(1);
	}
	
	// printf("Grading %s...\n", students[student_index]);
	struct stat statbuf;
	double sum = 0.0; // 개별 학생 총점
    for (i = 0; i < number_of_questions; i++) {
        if (problem_type[i] == 1) {
			char pathname[15];
			sprintf(pathname, "%s.c", answer_directory[i]);
			statbuf.st_size = -1;
			if (access(pathname, F_OK) == 0)
				stat(pathname, &statbuf);
			if (statbuf.st_size > 0) {
				// printf("%s : ", pathname);
				scores[student_index][i] = compile_and_return_result(student_index, i, answer_directory[i]);
				sum += scores[student_index][i];
				// fprintf(stderr, "test Score : %.2lf\n", scores[student_index][i]);
			} else {
				//printf("%s : X - Not Submitted\n", pathname);
				// Not submit or zero-byte
			}
        } else {
			char pathname[15];
			sprintf(pathname, "%s.txt", answer_directory[i]);
			statbuf.st_size = -1;
			if (access(pathname, F_OK) == 0)
				stat(pathname, &statbuf);
			if (statbuf.st_size > 0) {
				 //printf("%s : ", pathname);
				 scores[student_index][i] = codeCMP(i, answer_directory[i]);
				//scores[student_index][i] = compile_and_return_result(student_index, i, answer_directory[i]);
				if (scores[student_index][i] > 0.0) {
					//printf("O - Text Correct!!");
				} else {
					//printf("X - Text Incorrect!!");
				}
				sum += scores[student_index][i];
				// fprintf(stderr, "test Score : %.2lf\n", scores[student_index][i]);
				// printf("\n");
			} else {
				//printf("%s : X - Text Not Submitted\n", pathname);
				// Not submit or zero-byte
			}
		}
    }
	// db are
	s_scores[student_index] = sum;
	if (arg_option_p) {
		printf("%s is finished.. score : %.2lf\n", students[student_index], sum);
	}
	else
		printf("%s is finished..\n", students[student_index]);
	chdir("..");
    return 0;
}

double codeCMP(int question_index, char *dirname)
{
	// pwd는 해당 학생의 디렉터리임으로, 학생정보는 필요치 않다.
	char stu_buf[1024];
	char pathname[15];
	int fd, length;
	int answer_candidate_number = 1;
	char *answer_start_pointer[100];
	double score = indiv_score[question_index]; // 일단 만점을 주고 틀리면 뺏는다.
	// 학생파일을 분석한다.
	sprintf(pathname, "%s.txt", dirname);
	if ((fd = open(pathname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n%s\n", pathname, strerror(errno));
		exit(1);
	}
	if ((length = read(fd, stu_buf, 1023)) < 0) {
		fprintf(stderr, "read error for %s-%s\n", dirname, pathname);
		exit(1);
	}
	close(fd);
	stu_buf[length] = 0;
	normalize(stu_buf);
	length = strlen(stu_buf);

	normalize(answers[question_index]);
	
	// 여러 정답을 추출해낸다.
	answer_start_pointer[0] = answers[question_index];
	int ansl = strlen(answers[question_index]);
	for (int i = 0; i < ansl; i++) {
		// if (answers[question_index][i] == '\n') answers[question_index][i] = '\0';
		if (answers[question_index][i] == ':') {
			if(answers[question_index][i+1] == '\0') {
				// 스트링 끝에 :이 붙은 경우
				break;
			}
			answer_start_pointer[answer_candidate_number] = &answers[question_index][i+1];
			++answer_candidate_number;
			answers[question_index][i] = '\0'; // make null character
		}
	}
	bool isCorrect = false;
	for (int i = 0; i < answer_candidate_number; i++) {
		if (strcmp(stu_buf, answer_start_pointer[i]) == 0) {
			isCorrect = true;
			break;
		} else {
			// take parse tree
			//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
			printf("--%s--\n--%s--\n", stu_buf, answer_start_pointer[i]);
			//getchar();
			char *treeL = mpt(stu_buf);
			char *treeR = mpt(answer_start_pointer[i]);
			printf("통과\n");
			//getchar();
			if(strcmp(treeL, treeR) == 0) {
				isCorrect = true;
				free(treeL);
				free(treeR);
				break;
			}
		}
	}
	if (!isCorrect) {
		score = 0.0;
	}
	return score;
}

double compile_and_return_result(int student_index, int question_index, char *dirname)
{
	// 기본적으론 해당 점수 리턴하되, 워닝시 감점해서 리턴, 틀리거나 오류 시 0점
	double score = indiv_score[question_index];
	char *p = "";
	char pathname[15];
	char gcc_command[100];
	struct stat statbuf;
	if(arg_option_t) {
		for (int i = 0;i < arg_option_t_argc; i++) {
			if (strcmp(dirname, arg_option_t_argv[i]) == 0) {
					p = "-lpthread";
					break;
			}
		}
	}
	sprintf(pathname, "%s.c", dirname);

	// error, warning redirecting (error file writing)
	char errr[256];
	char path[256];
	if(arg_option_e) {
		if (arg_option_e_argv[0][0] != '/')
			sprintf(path, "../../%s/", arg_option_e_argv[0]);
		else
			sprintf(path, "%s", arg_option_e_argv[0]);
		if (access(path, F_OK) != 0) {
			if (mkdir(path, 0777) != 0) {
				fprintf(stderr, "error for generating -errordir\n");
				exit(1);
			}
		}
		if (arg_option_e_argv[0][0] != '/')
			sprintf(path, "../../%s/%s/", arg_option_e_argv[0], students[student_index]);
		else
			sprintf(path, "%s/%s/", arg_option_e_argv[0], students[student_index]);
		if (access(path, F_OK) != 0) {
			if (mkdir(path, 0777) != 0) {
				fprintf(stderr, "error for generating -errordir\n");
				exit(1);
			}
		}
	}
	if(!arg_option_e)
		sprintf(errr, "%s_error.txt", dirname);
	else
		sprintf(errr, "%s%s_error.txt", path, dirname);
	int error_warning_fd = creat(errr, 0666);
	int original_stderr = dup(2);
	dup2(error_warning_fd, 2);
	
	sprintf(gcc_command, "gcc %s %s -o %s.stdexe", pathname, p, dirname);
	
	// try compile
	int retsys = system(gcc_command);
	
	close(error_warning_fd);
	
	dup2(original_stderr, 2);

	// if retsys is not 0, error detected
	if (retsys != 0) {
		// printf("X - Compile Error\n");
		if(!arg_option_e)
			remove(errr); // error file delete
		return ERROR_POINT;
	}
	// counting warnings
	stat(errr, &statbuf);
	if(statbuf.st_size > 0) {
		// printf("warning detected\n");
		FILE *fp;
		fp = fopen(errr, "r");
		char line_buf[500];
		while (fgets(line_buf, sizeof(line_buf), fp) != NULL) {
			char *find_warning;
			find_warning = strstr(line_buf, " warning: ");
			if(find_warning) {
				//fprintf(stderr, "워닝 찾음");
				find_warning = NULL;
				score -= WARNING_DEDUCTION_POINT;
			}
		}
		fclose(fp);
	if(!arg_option_e)
		remove(errr); // warn file delete
	} else {
		remove(errr); // warn file delete
	}
	// create stdout file (running students' exe)
	int stdout_fd, origin_fd;
	sprintf(gcc_command, "%s.stdout", dirname);
	stdout_fd = creat(gcc_command, 0666);
	sprintf(gcc_command, "./%s.stdexe", dirname);
	// open 2 thread and race time

	origin_fd = dup(1);
	dup2(stdout_fd, 1);
	pthread_t mark_tid, wait_tid;
	t_args.gcc_command = gcc_command;
	t_args.target_tid = &wait_tid;
	t_args.isError = false;
	// fprintf(stderr, "wait Tid(1) : %ld\n", *(t_args.target_tid));
	// execute students' program
	pthread_create(&mark_tid, NULL, mark_thread, (void *)&t_args);
	// sleep 5-7 seconds
	pthread_create(&wait_tid, NULL, wait_thread, (void *)&mark_tid);
	
	pthread_join(wait_tid, NULL);
	pthread_join(mark_tid, NULL);
	// restore std print
	dup2(origin_fd, 1);

	if (t_args.isError) {
		// printf("X - Run Time Error!\n");
		return ERROR_POINT;
	}
	if (is_time_limited) {
		is_time_limited = false;
		// printf("X - Time Limit Exceeded!!\n"); // 6 times
		// process kill please
		char kill_command[50];
		sprintf(kill_command, "pkill -9 -ef %s > /dev/null", gcc_command);
		system(kill_command);
		return ERROR_POINT; // 0 score
	}
	// dup2(origin_fd, 1);
	// read stdout and mark answer
	sprintf(gcc_command, "%s.stdout", dirname);
	char ansbuf[101];
	int student_fd = open(gcc_command, O_RDONLY);
	ssize_t len;
	size_t off = 0;
	while ((len = read(student_fd, ansbuf, 100)) > 0) {
		ansbuf[len] = '\0';
		normalize(ansbuf);
		len = strlen(ansbuf);
		ansbuf[len] = '\0';
		//fprintf(stderr, " 길이 ? : %ld\n", len);
		//fprintf(stderr, "버퍼에 저장된 내용2 : %s\n", ansbuf);
		//printf("<<%ld>>\n", len);
		if ( strcmp2( answers[question_index] + off, ansbuf, len ) != 0  ) {
			//fprintf(stderr, "[<%s> %s - X : %ld [%s] [%s]\n", students[student_index], dirname, off, answers[question_index] + off, ansbuf + off);
			// printf("X - incorrect!!!\n");
			close(student_fd);
			return 0.0;
		}
		off += len;
	}
	close(student_fd);
	// fprintf(stderr, "<%s> %s - O : [%s]\n", students[student_index], dirname, answers[question_index]);
	// printf("O - correct!!\n");
	return score;
}

int strcmp2(char *a, char *b, int tol) {
	int i;
	for (i = 0; i < tol; i++) {

		//// test .....
		 //fprintf(stderr, "test[%d] :[%c/%d] [%c/%d] \n", i, a[i], (int)a[i], b[i], (int)b[i]);
		// getchar(); getchar();
		if(a[i] != b[i]) return 1;
	}
	return 0;
}

void make_student_score_table()
{
	chdir("..");
	int csv_fd;
	char buf[50];
	if ((csv_fd = open("score.csv", O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
		fprintf(stderr, "Error Create for score.csv file !! \n");
		exit(1);
	}
	// fill with answer_directory
	write(csv_fd, ",", 1); // comma

	for (int i = 0; i < number_of_questions; i++) {
		if (problem_type[i] == 0) {
			// text
			sprintf(buf, "%s.txt", answer_directory[i]);
		} else {
			sprintf(buf, "%s.c", answer_directory[i]);
		}
		write(csv_fd, buf, strlen(buf));
		write(csv_fd, ",", 1); // comma
	}
	write(csv_fd, "sum\n", 4);
	for (int i = 0; i < number_of_students; i++) {
		// write 학번
		write(csv_fd, students[i], strlen(students[i]));
		write(csv_fd, ",", 1);
		for (int j = 0; j < number_of_questions; j++) {
			sprintf(buf, "%.2lf", scores[i][j]);
			write(csv_fd, buf, strlen(buf));
			write(csv_fd, ",", 1);
		}
		// write sum
		sprintf(buf, "%.2lf", s_scores[i]);
		write(csv_fd, buf, strlen(buf));
		write(csv_fd, "\n", 1);
	}
	close(csv_fd);
}


int main(int argc, char *argv[])
{
	struct timeval begin, end;
	gettimeofday(&begin, NULL);
	no_mark = false;
	// parse user's arguments
	// rmate로 수정하는 것 테스트
	parse_args(argc, argv);

	if (arg_option_h) {
		print_helps();
		gettimeofday(&end, NULL);
		ssu_runtime(&begin, &end);
		exit(0);
	}

	// 기존 error directory 삭제
	if (arg_option_e) {
		char rmrfcmd[50];
		sprintf(rmrfcmd, "rm -rf %s", arg_option_e_argv[0]);
		system(rmrfcmd);
	}

	// compose Answer Data into RAM (include compile C & run so long time)
	open_answer_set();
	if (!no_mark) {
		set_students_info();

		check_score_csv();

		printf("Grading Student's test papers..\n");
		for (int i = 0; i < number_of_students; i++) {
			// printf("Grading %s...\n", students[i]);
			mark_student(i);
			// printf("score : %.2lf\n", scores[i]);
		}

		// 성적 테이블 생성
		make_student_score_table();

		if (arg_option_p) {
			double sum = 0.0, avg;
			for (int i = 0; i < number_of_students; i++) {
				sum += s_scores[i];
			}
			avg = sum / number_of_students;
			printf("Total average : %.2lf\n", avg);
		}
	} else {
		// 분명히 C옵션을 주었을 것이다.
		FILE *fp;
		if ( !(fp = fopen("score.csv", "r")) ) {
			fprintf(stderr, "error for open score.csv\n");
			exit(1);
		}
		char line_buf[1024];
		char curr_student_number[101];
		while (fgets(line_buf, sizeof(line_buf), fp) != NULL) {
			//line_buf[strlen(line_buf)-1] = 0;
			// 학번 일치 여부 검사
			// 첫번째 항목은 학번인데 콤마 전까지 자른다.
			int tempPtr;
			//printf("원본<%s>\n", line_buf);
			for (int i = 0; i < 256; i++) {
				if (line_buf[i] == ',') {tempPtr = i; line_buf[i] = '\0'; break;}
			} // 학번만 보이도록 자름
			strncpy(curr_student_number, line_buf, 100);
			line_buf[tempPtr] = ','; // 복구
			// printf("%s\n", curr_student_number);
			for (int i = 0; i < arg_option_c_argc; i++) {
				if (strcmp(arg_option_c_argv[i], curr_student_number) == 0) {
					// 점수를 찾아서 출력
					int lastIndex = strlen(line_buf) - 1;
					for (int j = lastIndex; j >=0; j--) {
						if (line_buf[j] == ',') {
							lastIndex = j+1;
							break;
						}
					}
					// 마지막에 개행 문자 포함
					printf("%s's score : %s", curr_student_number, line_buf+lastIndex);
					break;
				}
			}
		}
		fclose(fp);
	}
	if (arg_option_c) {
		// 채점을 함.
		for (int i = 0; i < arg_option_c_argc; i++) {
			for (int j = 0; j < number_of_students; j++) {
				if (strcmp(arg_option_c_argv[i], students[j]) == 0) {
					printf("%s's score : %.2lf\n", students[j], s_scores[j]);
					break;
				}
			}
		}
	}
	gettimeofday(&end, NULL);
	ssu_runtime(&begin, &end);
	exit(0);
}
