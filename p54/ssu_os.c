#include <stdio.h>

int i = 10;

void ssu_os1(void) {
	printf("[%s] in %s:%d\n", __FILE__, __func__, __LINE__);
}

void ssu_os2(void) {
	printf("[%s] in %s:%d\n", __FILE__, __func__, __LINE__);
}

