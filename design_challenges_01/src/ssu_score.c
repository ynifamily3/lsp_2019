#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

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

/*
	students 	-> "2016xxxx"
				-> "2017xxxx"
				-> "2018xxxx"
				...
*/


void extract_answer(char *ansdir)
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
		problem_type[number_of_questions] = 0;
		ssize_t flength;
		ssize_t content_length;
		flength = lseek(fd_txt, (off_t)0, SEEK_END);
		lseek(fd_txt, (off_t)0, SEEK_SET);
		if( (content_length = read(fd_txt, answers[number_of_questions], flength)) < 0) {
			fprintf(stderr, "reading file error... %s\n", gcc_command);
		}
		close(fd_txt); // no more
	}
	else if (fd_c > 0) {
		problem_type[number_of_questions] = 1;
		// c mode
		// printf("c : %s\n", filename_c);
		close(fd_c); // no more....
		if(!arg_option_t) {
			p = "";
		} else {
			for (int i = 0;i < arg_option_t_argc; i++) {
				if (strcmp(ansdir, arg_option_t_argv[i]) == 0) {
						p = "-lpthread";
						break;
				}
			}
		}
		printf("making answer .... %s \n", filename_c);
		sprintf(gcc_command, "gcc %s %s -o %s.exe", filename_c, p, ansdir);
		printf("%s\n", gcc_command);
		system(gcc_command);
		sprintf(gcc_command, "./%s.exe > %s.stdout", ansdir, ansdir);
		printf("%s\n", gcc_command);
		system(gcc_command);
		// open %s.stdout and put into answers[number_of_questions]
		ssize_t flength;
		ssize_t content_length;
		sprintf(gcc_command, "%s.stdout", ansdir);
		if ( (fd_c = open(gcc_command, O_RDONLY)) < 0) {
			fprintf(stderr, "open file error... %s\n", gcc_command);
			exit(1);
		}
		flength = lseek(fd_c, (off_t)0, SEEK_END);
		lseek(fd_c, (off_t)0, SEEK_SET);
		if( (content_length = read(fd_c, answers[number_of_questions], flength)) < 0) {
			fprintf(stderr, "reading file error... %s\n", gcc_command);
		}
	}
	else {
		printf("no file in ans_dir : %s\n", ansdir);
		exit(1);
	}
	chdir("..");
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
		answers[i] = (char *)calloc(1000, sizeof(char)); // answer contents
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
		memcpy(answer_directory[number_of_questions], dentry->d_name, 20);
		extract_answer(answer_directory[number_of_questions]);
		++number_of_questions;
	}
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

	for (int i = 0; i < number_of_questions; i++) {
		// if(problem_type[i] == 0) continue;
		printf("%s : %s", answer_directory[i], answers[i]);
		printf("--------------------------------\n");
	}

	mark_student(0);
	
	//mark_student(int student_index);
	// for example, 11 check
	//printf("%d", assert_answer(1, "parent", "parent : SIGUSR1 pending"));
	
	exit(0);
}

