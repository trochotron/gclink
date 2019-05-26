int openport(char *device);
int writeport(int port, unsigned char *buffer, int length, int timeout);
int readport(int port, unsigned char *buffer, int length, int timeout);
void closeport(int port);
char *porterror(int error);

#ifdef linux
#include <errno.h>
#define ser_errno errno          /* linux uses value of errno */
#elif defined __DJGPP__
extern int ser_errno;		 /* need to fake errno in serial.c */
#endif
