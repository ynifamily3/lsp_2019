#ifndef SSU_RUNTIME
#define SSU_RUNTIME
#include <sys/time.h>
#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

#endif
