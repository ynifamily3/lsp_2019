#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "arg_read.h"

void parse_args(int argc, char *argv[])
{
	student_dir = argv[1];
	answer_dir = argv[2];
	arg_option_c = false;
 	arg_option_e = false;
 	arg_option_t = false;
 	arg_option_p = false;
 	arg_option_h = false;
	
	arg_option_c_argc = 0;
	arg_option_e_argc = 0;
	arg_option_t_argc = 0;

	arg_option_c_argv = (char **)malloc(sizeof(char *) * 100);
	arg_option_e_argv = (char **)malloc(sizeof(char *) * 100);
	arg_option_t_argv = (char **)malloc(sizeof(char *) * 100);
	
	// arguments judge
	extern int optind;
	extern char *optarg;
	int c;

	while( (c = getopt(argc, argv, "c:e:t:ph")) != -1) {

		switch (c) {
				case 'c':
				arg_option_c = true;
				break;
				case 'e':
				arg_option_e = true;
				break;
				case 'p':
				arg_option_p = true;
				break;
				case 't':
				arg_option_t = true;
				break;
				case 'h':
				arg_option_h = true;
				break;
				case '?':
				break;
				default:
				fprintf(stderr, "???\n");
				break;
		}

		if (c == 'c' || c == 'e' || c == 't') { 
			int i = optind - 1;
			for (; i < argc && argv[i][0] != '-'; i++) {
				switch(c) {
					case 'c':
                    if(arg_option_c_argc >= 5) {
                        printf("Maximum Number of Argument Exceeded.\t:: %s\n", argv[i]);
                        break;
                    }
					arg_option_c_argv[arg_option_c_argc++] = argv[i];
					break;
					case 'e':
                    if(arg_option_e_argc >= 5) {
                        printf("Maximum Number of Argument Exceeded.\t:: %s\n", argv[i]);
                        break;
                    }
					arg_option_e_argv[arg_option_e_argc++] = argv[i];
					break;
					case 't':
                    if(arg_option_t_argc >= 5) {
                        printf("Maximum Number of Argument Exceeded.\t:: %s\n", argv[i]);
                        break;
                    }
					arg_option_t_argv[arg_option_t_argc++] = argv[i];
					break;
				}
			}

		}
	}
}
