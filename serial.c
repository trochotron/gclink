/*
 * serial.c -- handles serial communications specific to each OS
 *
 * Copyright 1999 by Joshua Neal.  This file may be redistributed under the
 * GNU Public License.
 */

#include <string.h>

#ifdef linux
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#endif

#ifdef __DJGPP__
#include <stdio.h>
#include <ctype.h>
#include "com.h"         /* serial library header */
int ser_errno;           /* on dos, we have to set our own errno */
char comstring[] = "COM";

static int parsedevice(char *dev)
{
        unsigned int port = 0;
        int irq = 0;
	if (strlen(dev) == 4) {
		dev[0] = toupper(dev[0]);
		dev[1] = toupper(dev[1]);
		dev[2] = toupper(dev[2]);
		if (memcmp(&comstring, dev, 3) == 0) {
			if ((dev[3] < '1') || (dev[3] > '4')) {
				ser_errno = -1;
				return -1;
			}
			return dev[3] - '1';
		}
	}
	if(sscanf(dev, "%x,%i", &port, &irq) < 2) {
		ser_errno = -1;
		return -1;
	}
	if ((irq < 0) || (irq > 15)) {
		ser_errno = -3;
		return -1;
	}
	if (port > 0x0ffff) {
		ser_errno = -3;
		return -1;
	}
	COMSetHardwareParameters(COM5, irq, (int) port);
	return COM5;
}
#endif

int openport(char *dev)
{
#ifdef linux
        int fd;
	struct termios tio;
        fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1) return (fd);
        if (tcgetattr(fd, &tio)) return -1; 	/* setup port to 9600,N,8,1 */
	tio.c_cc[VMIN] = 2;
        cfmakeraw(&tio);			
        cfsetispeed(&tio, B9600);		
        cfsetospeed(&tio, B9600);
        if (tcflush(fd, TCIFLUSH)) return -1;
        if (tcsetattr(fd, TCSANOW, &tio)) return -1;
        if (fcntl(fd, F_SETFL, FNDELAY)) return -1;
        return (fd);    
#elif defined __DJGPP__
	int port, status;
	COMInit();
	if ((port = parsedevice(dev)) < 0) return -1;
	status = COMPortOpen(port, 9600, 8, 'N', 1, 0, NULL);
	if (status == COMERR_NOCHIP) {
		ser_errno = -2;
		return -1;
	}
	if (status == COMERR_NOMEMORY) {
		ser_errno = -4;
		return -1;
	}
	if (status != 0) {
		ser_errno = -5;
		return -1;
	}
	return port;
#endif
}

void closeport(int port)
{
#ifdef linux
        close(port);
#elif defined __DJGPP__
	COMPortClose(port);
#endif
}

int writeport(int port, unsigned char *buffer, int length, int timeout)
/* write buffer to port with timeout */
{
	int bytes_sent;
#ifdef linux
	bytes_sent = write(port, buffer, length);
#elif defined __DJGPP__
	int status;
	status = COMWriteBufferTimed(port, buffer, NULL, length, 
		&bytes_sent, timeout * 1000);
#endif    
        return bytes_sent;
}




int readport(int port, unsigned char *buffer, int length, int timeout)
{
	int bytes_in;
#ifdef linux
	int count = 0;
	fd_set rfds;
	struct timeval tv;
	FD_ZERO(&rfds);
	FD_SET(port, &rfds);
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	while (count < length) {
		if (!select(port+1, &rfds, NULL, NULL, &tv)) {
		return count;
		} 
		bytes_in = read(port, &buffer[count], length - count);
		if (bytes_in < 0) return -1;
		count += bytes_in;
	}
	bytes_in = count;
#elif defined __DJGPP__
	int status;
	status = COMReadBufferTimed(port, buffer, NULL, length, &bytes_in,
		timeout * 1000);
	if (status == COMERR_RXOVERFLOW) {
		ser_errno = -6;
		return -1;
	}
#endif
        return bytes_in;

}

#ifdef __DJGPP__
static char ser_baderr[] = "bad error value";
static char ser_nodevice[] = "invalid device, device must be com1-4 or port,irq";
static char ser_nouart[] = "no uart at specified com port io address";
static char ser_baddev[] = "port or irq out of range";
static char ser_nomemory[] = "no locked memory available for fifo buffers";
static char ser_general[] = "general error";
static char ser_rxoverflow[] = "uart receive buffer overflow";
#endif

char *porterror(int error)
{
#ifdef linux
	return strerror(error);
#elif defined __DJGPP__
	switch (error)
	{
		case -1:
			return ser_nodevice;
		case -2:
			return ser_nouart;
		case -3:
			return ser_baddev;
		case -4:
			return ser_nomemory;
		case -5:
			return ser_general;
		case -6:
			return ser_rxoverflow;
		default:
			return ser_baderr;
	}
#endif	
}
