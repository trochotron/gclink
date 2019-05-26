/*
 * gclink.c -- main function for command line interface
 *
 * Copyright 1999 by Joshua Neal.  This file may be redistributed under the
 * GNU Public License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "protocol.h"
#include "slot.h"

#define VERSION "0.2"

void usage(void)
{
        printf ("Usage: gclink -d device [options]\n"
		"	Communicates with a Tiger game.com unit via a Web Link cable.\n\n"		
		"Options:\n"
#if defined linux
		"-d device	serial device link cable is connected to (e.g. -d /dev/ttyS1)\n"
#elif defined __DJGPP__
		"-d COMn  	COM port number link cable is connected to (1-4)\n"
		"-d port,irq	serial port base in hex and irq (e.g. -d 3f8,4)\n"		
#endif
		"-h		display this help and exit\n"
		"-l		list slots\n"						
		"-r		receive slots from game.com unit and save\n"
		"-s filename	send slot file to game.com\n"
		"-v		display version information and exit\n\n"
	);
		
}

int main (int argc, char *argv[])
{
	enum actions { NONE, LIST, RECEIVE, SEND };
	int c, action = NONE;
	char *device = NULL, *filename = NULL;

	opterr = 0;

	while ((c = getopt (argc, argv, ":vhrls:d:")) >= 0)
	{
		switch (c)
		{
    			case 'v':
			/* display version information and exit */
#if defined linux
				printf("gclink Linux Version " VERSION "\n");
#elif defined __DJGPP__
				printf("gclink MS-DOS Version " VERSION "\n");
#endif
				printf( "Copyright 1999, Joshua D. Neal\n"
					"There is NO warranty.  You may redistribute this software under the terms\n"
					"of the GNU Public License.  For more information, see the files named COPYING.\n"				
				);
				exit(0);		
			case 'h':
			/* display help and exit */
				usage();
				exit(0);
			case 'r':  
			/* receive slots from game.com unit and save */
				action = RECEIVE;	
				break;
			case 'l':
			/* list slots */
				action = LIST;
				break;
			case 's':
			/* send slot file to game.com */
				action = SEND;
				filename = optarg;
				break;
			case 'd':
			/* get specified serial device */
				device = optarg;
				break;
			case ':':
			/* missing argument */
				fprintf(stderr,	"gclink: option %c requires an "
					"argument\n\n", optopt);
				usage();
				exit(-1);
			case '?':
			/* invalid option */
				if (optopt != '?') fprintf(stderr, "gclink: invalid "
					"option: %s\n\n", argv[optind-1]);
				usage();
				exit(-1);
			default:
				fprintf(stderr, "gclink: unimplemented argument: %c",
					optopt);					
				exit(-1);
        	}
	}
	if (device == NULL) {
		fprintf(stderr, 
			"gclink: no device specified\n"
			"Try 'gclink -h' for a list of options.\n"
		);
		exit(-1);
	}
	switch (action) {
		case NONE:
			fprintf(stderr, 
				"gclink: no action specified\n"
				"Try 'gclink -h' for a list of options.\n");
			exit(-1);
		case LIST:
			fprintf(stderr, "gclink: listing slots from device "
				"%s\n",	device);
                        // spamport(device, 2);
			listslots(slotbuf);
			break;
		case RECEIVE:
			fprintf(stderr, "gclink: receiving data from device "
				"%s\n", device);
			if (getslots(device, slotbuf) < 0) exit(-1);
			saveslots(slotbuf);
			break;
		case SEND:
			fprintf(stderr, "gclink: sending file %s to device "
				"%s\n", filename, device);
			if (loadslot(filename, slotbuf) < 0) exit(1);
			writeslot(device, slotbuf);
			
			break;
	}
	return(0);
}		








