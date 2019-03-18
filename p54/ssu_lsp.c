#include <stdio.h>

static void ssu_lsp2(void);

void ssu_lsp1(void) {
	printf("[%s] in %s:%d\n", __FILE__, __func__, __LINE__);
	ssu_lsp2();
}

static void ssu_lsp2(void) {
	printf("[%s] in %s:%d\n", __FILE__, __func__, __LINE__);
}

