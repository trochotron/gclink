/*
 * commands.h -- commands and responses for game.com protocol
 *
 * Copyright 1999 by Joshua Neal.  This file may be redistributed under the
 * GNU Public License.
 */

unsigned char cmdinit[]   = { 0x01, 0xFE };
unsigned char cmddone[]   = { 0x03, 0xFC };
unsigned char cmdgetall[] = { 0x05, 0xFA };
unsigned char cmdgetone[] = { 0x07, 0xF8 };
unsigned char cmdsetone[] = { 0x09, 0xF6 };

unsigned char replyinit[]   = { 0x02, 0xFD };
unsigned char replygetall[] = { 0x06, 0xF9 };
unsigned char replygetone[] = { 0x08, 0xF7 };
unsigned char acksetone[] = { 0x0C, 0xF3 };
unsigned char naksetone[] = { 0x0A, 0xF5 };
unsigned char nakbadsum[] = { 0x0F, 0XF0 };

/* the following may be commands or responses */
unsigned char unknown1[] = { 0x0B, 0XF4 };
unsigned char unknown2[] = { 0x0D, 0XF2 };
unsigned char unknown3[] = { 0x0E, 0XF1 };
