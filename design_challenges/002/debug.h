#ifndef __DEBUG__
#define __DEBUG__

#ifdef DEBUG
#define COLORS	"\x1b[36m"
#define COLORE	"\x1b[0m"
#define DBGMSG_PREFX	COLORS "<< message >> "

#define DBGMSG(msg,...) fprintf(stderr, \
						DBGMSG_PREFX"[%s:%s:%d] : " msg "\n" COLORE, __FILE__, __func__, \
		__LINE__, ##__VA_ARGS__)

#else
#define DBGMSG(...)
#endif
#endif

