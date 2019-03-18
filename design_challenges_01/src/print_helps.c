#include <stdio.h>
#include "print_helps.h"

void print_helps()
{
	printf("%s", "Usage : ssu_score <STUDENTDIR> <TRUEDIR> [OPTION]\n");
	printf("%s", "Option : \n");
	printf("%s", " -e <DIRNAME>\t   print error on 'DIRNAME/ID/qname_error.txt' file\n");
	printf("%s", " -t <QNAMES>\t   compile QNAME.C with -lpthread option\n");
	printf("%s", " -h \t\t   print usage\n");
	printf("%s", " -p \t\t   print student's score and total average\n");
	printf("%s", " -c <IDS>\t   print ID's score\n");
}
