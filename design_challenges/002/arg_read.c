#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "debug.h"
#include "arg_read.h"

void parse_args(int argc, char *argv[])
{
	int c;
	arg_option_j = false;
 	arg_option_c = false;
 	arg_option_p = false;
 	arg_option_f = false;
 	arg_option_l = false;
	arg_option_r = false;

	while( (c = getopt(argc, argv, "jcpflr")) != -1) {
		switch (c) {
			case 'j':
				arg_option_j = true;
				DBGMSG("j option enbled");
			break;
			case 'c':
				arg_option_c = true;
				DBGMSG("c option enbled");
			break;
			case 'p':
				arg_option_p = true;
				DBGMSG("p option enbled");
			break;
			case 'f':
				arg_option_f = true;
				DBGMSG("f option enbled");
			break;
			case 'l':
				arg_option_l = true;
				DBGMSG("l option enbled");
			break;
			case 'r':
				arg_option_r = true;
				DBGMSG("r option enbled");
			break;
			default:
				// 보통 '?' 가 오게 됨.
				DBGMSG("지원하지 않는 옵션 사용");
				exit(1);
			break;
		}
	}
}

