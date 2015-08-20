#ifndef RUNE_H_
#define RUNE_H_

/*
 * The authors of this software are Rob Pike and Howard Trickey.
 * 		Copyright (c) 1992 by AT&T.
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 */

/* Copyright (c) 1992 AT&T - All rights reserved. */

/* Plan 9 C library interface */
typedef unsigned short Rune;

enum {
	UTFmax = 3,		/* maximum bytes per rune */
	Runesync = 0x80,	/* cannot represent part of a utf sequence (<) */
	Runeself = 0x80,	/* rune and utf sequences are the same (<) */
	Runeerror = 0x80	/* decoding error in utf */
};

/*
 * new rune routines
 */
extern int runetochar(char *, const Rune *);
extern int chartorune(Rune *, const char *);
extern int runelen(long);
extern int fullrune(const char *, const int);

/*
 * rune routines from converted str routines
 */
extern long utflen(const char *);	/* was countrune */
extern char *utfrune(char *, const long);
extern char *utfrrune(char *, const long);
extern char *utfutf(const char *, const char *);

#endif  // RUNE_H_
