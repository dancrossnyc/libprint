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

#include <stddef.h>
#include <stdint.h>

/* Plan 9 C library interface */
typedef uint32_t Rune;

enum {
	UTFmax = 4,		/* maximum bytes per rune */
	Runesync = 0x80,	/* cannot represent part of a utf sequence (<) */
	Runeself = 0x80,	/* rune and utf sequences are the same (<) */
	Runeerror = 0x80,	/* decoding error in utf */
	Runemax = 0x10FFFF	/* maximum rune value */
};

/*
 * new rune routines
 */
extern int runetochar(char *, const Rune *);
extern int chartorune(Rune *, const char *);
extern int runelen(long);
extern size_t runenlen(const Rune *r, int nrune);
extern int fullrune(const char *, const int);

/*
 * rune routines from converted str routines
 */
extern size_t utflen(const char *);	/* was countrune */
extern char *utfrune(const char *, const long);
extern char *utfrrune(const char *, const long);
extern char *utfutf(const char *, const char *);

#endif  // RUNE_H_
