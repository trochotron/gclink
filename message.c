/*
 * messages.c -- reports status and error messages to user
 *
 * Copyright 1999 by Joshua Neal.  This file may be redistributed under the
 * GNU Public License.
 */
#include <stdio.h>

void statusmessage(char *message)
{
	fputs(message, stderr);

}

void errormessage(char *message)
{
	fprintf(stderr, "gclink: %s\n", message);
}