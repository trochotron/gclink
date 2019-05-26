/*
 * gclink.c -- functions for manipulating slots
 *
 * Copyright 1999 by Joshua Neal.  This file may be redistributed under the
 * GNU Public License.
 */
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#ifdef linux
#define O_BINARY 0        
#endif


unsigned char slotbuf[100000];

char slotname[] = "slota.dat";

void printslot(unsigned char *buffer)
{
	int index, spc, slen;
	if (buffer[2] == 1) {
		printf("(Continued)        .");
		return;
	}
	slen = strlen((char *) &buffer[3]);
	if (slen == 0) {
		printf("(Empty)            .");
		return;
	}
	for (index = 0; index < slen; index++) {
		if (buffer[index+3] == 1) {
			spc = 21 - slen;
			for (; spc > 0; spc--) putchar(' ');
		} else putchar(buffer[index+3]);
	}


}

void listslots(unsigned char *buffer) 
{
	int slot;
	printf("\nSlot Game           Score\n"
		 "---- --------------------\n");
	for (slot = 0; slot < 10; slot++) {
		printf("%c    ",'A'+slot); 
		printslot(&buffer[slot*64]);
		putchar('\n');
	}
}

void saveslots(unsigned char *buffer)
{
	int slot,f;
	fprintf(stderr, "Writing slots... ");
	for (slot = 0; slot < 10; slot++) {
		slotname[4] = 'a' + slot;
		putc(slotname[4], stderr);
		f = open(slotname, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, 
			0644);
		if (f < 0) {
			perror("Error opening file.\ngclink");
			return;
		}
		if (write(f,&buffer[slot * 64],64) < 0) {
			perror("Error writing file.\ngclink");
			return;
		}
		close(f);
	}
	fprintf(stderr, " Done.\n");
}

int loadslot(char *filename, unsigned char *buffer)
{
	int f, status;
	struct stat statbuf;
	fprintf(stderr, "Loading slot... ");
	f = open(filename, O_RDONLY | O_BINARY);
	if (f < 0) {
		perror("Error opening file.\ngclink");
		return -1;
	}
	if (fstat(f, &statbuf) < 0) {
		perror("Error getting file information.\ngclink");
		return -1;
	}
	if (statbuf.st_size != 64) {
		fprintf(stderr, "Incorrect file size.\n");
		return -1;
	}
	status = read(f, buffer, 64);
	if (status < 0)  {
		perror("Error reading file.\ngclink");
		return -1;
	}
	if (status < 64) {
		fprintf(stderr, "Error reading file.\n");
		return -1;
	}	
	close(f);
	fprintf(stderr, "Loaded.\n");
	return 0;
}
