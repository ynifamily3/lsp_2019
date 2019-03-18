#include <stdio.h>
#include <stdlib.h>

void ssu_os1(void);
void ssu_os2(void);
void ssu_esp1(void);
void ssu_esp2(void);
void ssu_lsp1(void);

extern int i;

int main(void)
{
	ssu_os1();
	ssu_os2();
	ssu_esp1();
	ssu_esp2();
	ssu_lsp1();
	printf("[%s] in %s:%d\n", __FILE__, __func__, __LINE__);
	printf("i = %d\n", i);
	exit(0);
}

