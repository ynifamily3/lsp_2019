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

#include "arg_read.h"
#include "print_helps.h"
#include "mark_student.h"
#include "assert_answer.h"

#define DIRECTORY_SIZE MAXNAMLEN

#define true 1
#define false 0

char **answer_directory;
int *problem_type; // 0 for text , 1 for c
char **answers;

char students_challenge[1000];

/*
answers -> contents of ans/1-1.txt (abc : def : ghi...)
		-> contents of ans/1-2.txt
		-> contents of ans/1-3.txt
		...
		-> contents of ans/11.stdout ( alarm 0 \n alarm 1 ..)
		-> contents of ans/12.stdout
*/

typedef int bool;

int number_of_questions = 0;
int number_of_program_questions = 0;
int number_of_text_questions = 0;

int number_of_students = 0;
char **students; 
int *scores; // siljae score = ( / 10 )(int hwa)

bool is_execute_completed = false; // check mark_thread ended
bool is_time_limited = false;

int compare(const void *a, const void *b);
int compile_and_return_result(int studnt_index, char *dirname);
void *mark_thread(void *arg);
void *wait_thread(void *arg);
int strcmp2(char *a, char *b, int tol);

struct thread_args {
		char *gcc_command;
		pthread_t *target_tid;
	};

struct thread_args t_args;

void *mark_thread(void *arg)
{
	struct thread_args *args = (struct thread_args *) arg;
	// fprintf(stderr, "wait Tid(2) : %ld\n", *(args->target_tid));
	// fprintf(stderr, "[[[[");
	system(args->gcc_command);
	// fprintf(stderr, "]]]]");
	is_execute_completed = true;
	pthread_cancel(*(args->target_tid));
	return NULL;
}

void *wait_thread(void *arg)
{
	sleep(7);
	is_time_limited = true;
	fprintf(stderr, "%ld", *(pthread_t *)arg);
	pthread_cancel(*(pthread_t *)arg);
	return NULL;
}

/*
	students 	-> "2016xxxx"
				-> "2017xxxx"
				-> "2018xxxx"
				...
*/
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
	scores = (int *)calloc(number_of_students, sizeof(int));
	qsort((void *)students, number_of_students, sizeof(answer_directory[0]), compare);	
}

void normalize(char *text)
{
	int i, j;
	for (i = 0, j = 0; text[i] != 0; i++,j++) {
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
		printf("making answer .... %s \n", filename_c);
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
	}
	else {
		printf("no file in ans_dir : %s\n", ansdir);
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
		fprintf(stderr, "Answer Data open Error\n");
		exit(1);
	}

	while ( (dentry = readdir(dirp)) != NULL ) {
		if (dentry->d_ino == 0 || dentry->d_name[0] == '.') continue;

		if (stat(dentry->d_name, &statbuf) == -1) {
			fprintf(stderr, "stat error for %s\n", dentry->d_name);
			break;
		}
		// all of contents contain directory
		memcpy(answer_directory[number_of_questions++], dentry->d_name, 20);
	}
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
	
	printf("Grading %s...\n", students[student_index]);
	struct stat statbuf;
    for (i = 0; i < number_of_questions; i++) {
        if (problem_type[i] == 1) {
			char pathname[15];
			sprintf(pathname, "%s.c", answer_directory[i]);
			statbuf.st_size = -1;
			if (access(pathname, F_OK) == 0)
				stat(pathname, &statbuf);
			if (statbuf.st_size > 0) {
				printf("%s : ", pathname);
				compile_and_return_result(i, answer_directory[i]);
			} else {
				printf("%s : Not Submitted\n", pathname);
			}
        }
    }
	chdir("..");
    return 0;
}

int compile_and_return_result(int student_index, char *dirname)
{
	int score = 100; // init score
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
	char errr[30];
	sprintf(errr, "%s-error.txt", dirname);
	int error_warning_fd = creat(errr, 0666);
	int original_stderr = dup(2);
	dup2(error_warning_fd, 2);
	
	sprintf(gcc_command, "gcc %s %s -o %s.exe", pathname, p, dirname);
	
	// try compile
	int retsys = system(gcc_command);
	
	close(error_warning_fd);
	
	dup2(original_stderr, 2);

	// if retsys is not 0, error detected
	if (retsys != 0) {
		printf("error occured\n");
		remove(errr); // error file delete
		return 0;
	}
	// counting warnings
	stat(errr, &statbuf);
	if(statbuf.st_size > 0) {
		// printf("warning detected\n");
		FILE *fp;
		fp = fopen(errr, "r");
		char line_buf[500];
		while (!feof(fp)) {
			char *find_warning;
			fgets(line_buf, sizeof(line_buf), fp);
			find_warning = strstr(line_buf, " warning: ");
			if(find_warning) {
				find_warning = NULL;
				score--; // decrease score
			}
		}
		fclose(fp);
	}

	remove(errr); // warn file delete
	// create stdout file (running students' exe)
	int stdout_fd, origin_fd;
	sprintf(gcc_command, "%s.stdout", dirname);
	stdout_fd = creat(gcc_command, 0666);
	sprintf(gcc_command, "./%s.exe", dirname);
	// open 2 thread and race time

	origin_fd = dup(1);
	dup2(stdout_fd, 1);
	pthread_t mark_tid, wait_tid;
	t_args.gcc_command = gcc_command;
	t_args.target_tid = &wait_tid;
	// fprintf(stderr, "wait Tid(1) : %ld\n", *(t_args.target_tid));
	// execute students' program
	pthread_create(&mark_tid, NULL, mark_thread, (void *)&t_args);
	// sleep 5-7 seconds
	pthread_create(&wait_tid, NULL, wait_thread, (void *)&mark_tid);
	
	pthread_join(wait_tid, NULL);
	pthread_join(mark_tid, NULL);
	// restore std print
	dup2(origin_fd, 1);


	if (is_time_limited) {
		is_time_limited = false;
		printf(" Time Limit Exceeded!!\n"); // 6 times
		// process kill please
		char kill_command[50];
		sprintf(kill_command, "pkill -9 -ef %s > /dev/null", gcc_command);
		system(kill_command);
		return 0; // 0 score
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
		// printf("%s\n", ansbuf);
		if ( strcmp2( answers[student_index] + off, ansbuf, len ) != 0  ) {
			printf(" Incorrect!!!\n");
			close(student_fd);
			return 0;
		}
		off += len;
	}
	close(student_fd);

	printf(" correct !!\n");
	return score;
}

int strcmp2(char *a, char *b, int tol) {
	int i;
	for (i = 0; i < tol; i++) {
		if(a[i] != b[i]) return 1;
	}
	return 0;
}


int main(int argc, char *argv[])
{
	// parse user's arguments
	parse_args(argc, argv);

	if (arg_option_h) {
		print_helps();
		exit(0);
	}
	// compose Answer Data into RAM (include compile C & run so long time)
	open_answer_set();

	set_students_info();

	printf("Grading Student's test papers..\n");
	mark_student(0);
	//for (int i = 0; i < number_of_students; i++)
		//mark_student(i);
	exit(0);
}
