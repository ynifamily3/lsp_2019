#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

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

int compare(const void *a, const void *b);

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
    // printf("mark %d\n", student_index);
    // test. Programming problem first
    int i;
	
	if (chdir(students[student_index]) < 0 ) {
		fprintf(stderr, "cannot find student directory %s\n", students[student_index]);
		exit(1);
	}
	
	printf("Grading %s\n", students[student_index]);
    for (i = 0; i < number_of_questions; i++) {
		// printf("%d %s\n", problem_type[i], answer_directory[i]);
        if (problem_type[i] == 1) {
            // find student's file
			char pathname[15];
			// sxxxxxxxxxxxxxxxxxxxx
			sprintf(pathname, "%s.c", answer_directory[i]);
			if ( access(pathname, F_OK) == 0) {
				printf("%s exists \n", pathname);
			} else {
				printf("cannot find %s\n", pathname);
			}
        }
    }
	chdir("..");
    return 0;
}


int main(int argc, char *argv[])
{
	int i;
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

	for (i = 0; i < number_of_students; i++)
		mark_student(i);
	
	exit(0);
}
