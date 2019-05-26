/*
 * protocol.c -- handles mid-level communications with game.com unit
 *
 * Copyright 1999 by Joshua Neal.  This file may be redistributed under the
 * GNU Public License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <pc.h>
#include "serial.h"
#include "message.h"
#include "commands.h"

#if DEBUG
    #include <stdio.h>
#endif

static unsigned char replybuf[50];

static int gcinit(char *device)
{
	int port, count;
	statusmessage("Opening port... ");
	port = openport(device);
	if (port < 0) {
		statusmessage("Unable to open.\n");
		errormessage(porterror(ser_errno));
		return -1;
	}
	statusmessage("Opened.\n");
	statusmessage("Sending request for connection... ");
        count = writeport(port, cmdinit, 2, 5);
	if (count < 2) {
		statusmessage("Error sending request.\n");
		if (count < 0) {
			errormessage(porterror(ser_errno));
		} else {
			errormessage("Command only partially sent");
		}
		closeport(port);
		return -1;
	}
	statusmessage("Sent.\n");
	statusmessage("Waiting for reply... ");
	count = readport(port, replybuf, 2, 1);
	if (count < 2) {
		if (count < 0) {
			statusmessage("Error receiving reply.\n");
			errormessage(porterror(ser_errno));
		} else {
			statusmessage("Timed out.\n");
		}
		closeport(port);
		return -1;
	}
#if DEBUG
	printf("Got reply %x %x\n", (unsigned int) replybuf[0], 
		(unsigned int) replybuf[1]);
#endif

	if (memcmp(replybuf, replyinit, 2)) {
		statusmessage("Incorrect reply.\n");
		closeport(port);
		return -1;
	}
	statusmessage("Connection established.\n");
	return port;
}

static int gcgetall(int port, unsigned char *buffer)
{
	int count;
	statusmessage("Sending request for all slots... ");
	count = writeport(port, cmdgetall, 2, 1);
	if (count < 2) {
		if (count < 0) {
			statusmessage("Error sending request.\n");
			errormessage(porterror(ser_errno));
		} else {
			errormessage("Command only partially sent");
		}
		closeport(port);
		return -1;
	}
	statusmessage("Sent.\n");
	statusmessage("Waiting for reply... ");
	count = readport(port, replybuf, 2, 5);
	if (count < 2) {
		if (count < 0) {
			statusmessage("Error receiving reply.\n");
			errormessage(porterror(ser_errno));
		} else {
			statusmessage("Timed out.\n");
		}
		closeport(port);
		return -1;
	}
#if DEBUG
	printf("Got reply %x %x\n", (unsigned int) replybuf[0], 
		(unsigned int) replybuf[1]);
#endif
	if (memcmp(replybuf, replygetall, 2)) {
		statusmessage("Incorrect reply.\n");
		closeport(port);
		return -1;
	}
	statusmessage("Request Acknowledged.\n");
	
	/* Now receive the data */
	statusmessage("Receiving all slots... ");
	count = readport(port, buffer, 641, 2);
#if DEBUG
	printf("Got %i bytes\n", count);
#endif
	if (count < 641) {
		if (count < 0) {
			statusmessage("Error receiving reply.\n");
			errormessage(porterror(ser_errno));
		} else {
			statusmessage("Timed out.\n");
		}
		closeport(port);
		return -1;
	}
	statusmessage("Received.\n");

	return 0;

}

static void gcdone (int port)
{
	int count;
	statusmessage("Closing connection... ");
	count = writeport(port, cmddone, 2, 1);
	if (count < 2) {
		statusmessage("Error sending request.\n");
		if (count < 0) {
			errormessage(porterror(ser_errno));
		} else {
			errormessage("Command only partially sent");
		}
		closeport(port);
		return;
	}
	statusmessage("Sent.\n");
	statusmessage("Waiting for reply... ");
	count = readport(port, replybuf, 40, 1);
	if (count < 2) {
		if (count < 0) {
			statusmessage("Error receiving reply.\n");
			errormessage(porterror(ser_errno));
		} else {
			statusmessage("Timed out.\n");
		}
		closeport(port);
		return;
	}
#if DEBUG
	printf("Got %i bytes\n", count);
	printf("Got reply %x %x\n", (unsigned int) replybuf[0], 
		(unsigned int) replybuf[1]);
#endif
	statusmessage("Closed.\n");

	closeport(port);
	

}

static unsigned char checksum (unsigned char *buffer, int length)
{
	unsigned char sum = 0;
	int count;
	for (count = 0; count < length; count++) sum += buffer[count];
	return sum;
}

int getslots (char *device, unsigned char *buffer)
{
	int port, status;
	unsigned char sum;
	
	port = gcinit(device);
	if (port < 0) return -1;

	status = gcgetall(port, buffer);
	if (status < 0) return -1;
	
	statusmessage("Verifying checksum... ");
	sum = checksum (buffer, 641);
#if DEBUG
	printf("Actual sum: %x\nComputed sum: %x\n",
		(unsigned int) buffer[640], (unsigned int) sum);
#endif
	if (sum != 0) {
		statusmessage("Incorrect checksum.\n");
		return -1;
	}
	statusmessage("Verified.\n");
	
	
	gcdone(port);
	return 0;
}


static int gcsetone(int port, unsigned char *buffer)
{
	int count;
	statusmessage("Sending new slot command... ");
        count = writeport(port, cmdgetone, 2, 1);
	if (count < 2) {
		if (count < 0) {
			statusmessage("Error sending request.\n");
			errormessage(porterror(ser_errno));
		} else {
			errormessage("Command only partially sent");
		}
		closeport(port);
		return -1;
	}
	statusmessage("Sent.\n");

	statusmessage("Sending new slot data... ");
        count = writeport(port, buffer, 16384, 150);
	if (count < 65) {
		if (count < 0) {
			statusmessage("Error sending data.\n");
			errormessage(porterror(ser_errno));
		} else {
			errormessage("Data only partially sent");
		}
		closeport(port);
		return -1;
	}
	statusmessage("Sent.\n");
	

	statusmessage("Waiting for reply... ");
	count = readport(port, replybuf, 2, 5);
	if (count < 2) {
		if (count < 0) {
			statusmessage("Error receiving reply.\n");
			errormessage(porterror(ser_errno));
		} else {
			statusmessage("Timed out.\n");
		}
		closeport(port);
		return -1;
	}
#if DEBUG
	printf("Got reply %x %x\n", (unsigned int) replybuf[0], 
		(unsigned int) replybuf[1]);
#endif
	if (memcmp(replybuf, nakbadsum, 2) == 0) {
		statusmessage("Incorrect checksum on sent data.\n");
		closeport(port);
		return -1;
	}
	if (memcmp(replybuf, naksetone, 2) == 0) {
		statusmessage("Slot not found.\n");
		closeport(port);
		return -1;
	}
	if (memcmp(replybuf, acksetone, 2) == 0) {
		statusmessage("Slot updated.\n");
		return 0;
	}
	statusmessage("Incorrect reply.\n");
	closeport(port);
	return -1;
}



int writeslot (char *device, unsigned char *buffer)
{
	int port, status;

	statusmessage("Generating checksum... ");
        buffer[64] = -checksum (buffer, 64);
	statusmessage("Generated.\n");
	port = gcinit(device);
	if (port < 0) return -1;

	status = gcsetone(port, buffer);
	if (status < 0) return -1;

	gcdone(port);

	return 0;
}

#if 0
int spamport (char *device, int seed)
{
        int port, count, value, status = 0, i, sum = 0;

        srandom(seed);

	port = gcinit(device);
	if (port < 0) return -1;

#if 1
        value = 0x0b; /* 0b */
        count = writeport(port, &value, 1, 1);
        value = 0xf4; /* f4 */
        count = writeport(port, &value, 1, 1);
#endif

#if 1
        value = 0xfe; /* DI */
        count = writeport(port, &value, 1, 1);
        value = 0xf1; /* HLT */
        count = writeport(port, &value, 1, 1);
#endif


#if 1
        for (;status < 2030; status++) {
                value = random();
        }                
#endif
        
/*        while (status < 2820) { */
        while (status < 2080) {
                status++;
                if ((status % 10 == 0) || (status >2930)) {
#if 0
                        count = readport(port, replybuf, 2, 1);
                        if (count >= 2) {
                            printf("Got reply %x %x\n", (unsigned int) replybuf[0], 
                               (unsigned int) replybuf[1]);

                        }
#endif
                        printf("num: %i\n", status);
                        // getkey();

                }
                value = 0xFE; /* random(); */
                count = writeport(port, &value, 1, 1);
                i = *((unsigned char *) &value);
                sum += i;
                if (i == 0xFF) printf("0xFF\n");
#if 1
                if (status > 2800) printf ("val: %x\n",i);
#endif
                if (count < 1) {
                        if (count < 0) {
                                statusmessage("Error sending request.\n");
                                errormessage(porterror(ser_errno));
                        } else {
                                errormessage("Command only partially sent");
                        }
                        closeport(port);
                        return -1;
                }




        }
#if 1
        printf("SUM: %x\n", sum);
#if 0
        value = 0x71; /* 71 */
        count = writeport(port, &value, 1, 1);
#endif
        sum += 0xff;
        value = 0xff; /* ff */
        count = writeport(port, &value, 1, 1);
        sum += 0xC1;
        value = 0xC1; /* 71 */
        printf("SUM: %x\n", sum);
        count = writeport(port, &value, 1, 1);
        value = 0x63; /* 63 */
        count = writeport(port, &value, 1, 1);
        value = 0x9c; /* 9C */
        count = writeport(port, &value, 1, 1);
#endif
                        count = readport(port, replybuf, 2, 5);
                        if (count >= 2) {
                            printf("Got reply %x %x\n", (unsigned int) replybuf[0], 
                               (unsigned int) replybuf[1]);

                        }

	return 0;



}
#endif
