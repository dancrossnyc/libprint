/* print.c -- formatted printing routines */
/* this by Scott Schwartz, derived from Paul Haahr's version of 12/91 */

static const char rcsid[] = 
"@(#)$Id: print.c,v 1.35 1995/07/04 18:24:52 schwartz Exp $";

#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "print.h"

#ifdef __GNUC__
extern void * memcpy (void *, const void *, long unsigned int);
#endif

/*
 * functions for inserting strings in the format buffer
 */

#if !defined __GNUC__ || defined __STRICT_ANSI__
#define inline /* */
#endif

extern int fmtappend (Format *format, const char *s, SIZE_T len) {
    format->reqlen += len;
    if (format->error)
	return 0;
    while (len > format->bufend - format->buf) {
	const SIZE_T split = format->bufend - format->buf;
	memcpy (format->buf, s, split);
	format->buf += split;
	s += split;
	len -= split;
	if ((*format->grow)(format, len))
	    return 0;
    }
    memcpy (format->buf, s, len);
    format->buf += len;
    return 1;
}

extern int fmtputc (Format *f, const char c) {
    return fmtappend (f, &c, 1);
}

extern int fmtcat (Format *format, const char *s) {
    return fmtappend (format, s, strlen (s));
}

/*
 * conversion functions
 *	true return -> flag changes only, not a conversion
 */

#define Flag(name, flag) \
static int name (Format *format, int c) { \
	format->flags |= flag; \
	return FMT_flag; \
}

Flag (uconv,	FMT_unsigned)
Flag (hconv,	FMT_short)
Flag (lconv,	FMT_long)
Flag (altconv,	FMT_altform)
Flag (leftconv,	FMT_leftside)
Flag (dotconv,	FMT_f2set)

static int digitconv (Format *format, int c) {
    if (format->flags & FMT_f2set)
	format->f2 = 10 * format->f2 + c - '0';
    else {
	format->flags |= FMT_f1set;
	format->f1 = 10 * format->f1 + c - '0';
    }
    return FMT_flag;
}

static int zeroconv (Format *format, int c) {
    if (format->flags & (FMT_f1set | FMT_f2set))
	return digitconv (format, '0');
    format->flags |= FMT_zeropad;
    return FMT_flag;
}

static int starconv (Format *format, int c) {
    int n = va_arg (format->args, int);
    if (format->flags & FMT_f2set)
	format->f2 = n;
    else {
	format->flags |= FMT_f1set;
	format->f1 = n;
    }
    return FMT_flag;
}

static void pad (Format *format, SIZE_T len, int c) {
    do; while (len-- != 0 && fmtputc (format, c));
}

static int sconv (Format *format, int c) {
    SIZE_T maxbytes, minwidth, extra;
    char *s = va_arg (format->args, char *);

    if (!s) s = "(null)";

    if (format->flags & FMT_altform) {
	maxbytes = (format->flags & FMT_f2set) ? format->f2 : strlen (s);
	/* maxbytes may overrun s,  but this way we don't have 
	   to assume that s is null terminated when f2 is set. 
	   Maybe use a different format character for this? */
    } else {
	/* do f2 like printf */
	maxbytes = strlen (s);
	if ((format->flags & FMT_f2set) && (format->f2 < maxbytes))
	    maxbytes = format->f2;
    }

    minwidth = (format->flags & FMT_f1set) ? format->f1 : maxbytes;
    extra = (minwidth > maxbytes) ? minwidth - maxbytes : 0;

    if (format->flags & FMT_leftside) {
	fmtappend (format, s, maxbytes);
	pad (format, extra, ' ');
    } else {
	pad (format, extra, ' ');
	fmtappend (format, s, maxbytes);
    }
	
    return FMT_verb;
}

static char *utoa (unsigned long u, 
		  char *t, unsigned int radix, const char *digit) {
    if (u >= radix) {
	t = utoa (u / radix, t, radix, digit);
	u %= radix;
    }
    *t++ = digit[u];
    return t;
}

static void intconv (Format *format, 
		    unsigned int radix, int upper, const char *altform) {
    static const char * const table[] = {
	"0123456789abcdefghijklmnopqrstuvwxyz",
	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    };
    char padchar;
    SIZE_T len, pre, zeroes, padding, width;
    long n, flags;
    unsigned long u;
    char number[64], prefix[20];

    if (radix > 36)
	return;

    flags = format->flags;
    if (flags & FMT_unsigned) {
	if (flags & FMT_long)
	    n = (unsigned long) va_arg (format->args, long);
	else if (flags & FMT_short)
	    n = (unsigned long)(unsigned short) va_arg (format->args, int);
	else
	    n = (unsigned long)(unsigned int) va_arg (format->args, int);
    } else {
	if (flags & FMT_long)
	    n = (long) va_arg (format->args, long);
	else if (flags & FMT_short)
	    n = (long)(short) va_arg (format->args, int);
	else
	    n = (long) va_arg (format->args, int);
    }

    pre = 0;
    if ((flags & FMT_unsigned) || n >= 0)
	u = n;
    else {
	prefix[pre++] = '-';
	u = -n;
    }

    if (flags & FMT_altform)
	while (*altform != '\0')
	    prefix[pre++] = *altform++;

    len = utoa (u, number, radix, table[upper]) - number;
    if ((flags & FMT_f2set) && (SIZE_T) format->f2 > len)
	zeroes = format->f2 - len;
    else
	zeroes = 0;

    width = pre + zeroes + len;
    if ((flags & FMT_f1set) && (SIZE_T) format->f1 > width) {
	padding = format->f1 - width;
    } else
	padding = 0;

    padchar = ' ';
    if (padding > 0 && flags & FMT_zeropad) {
	padchar = '0';
	if ((flags & FMT_leftside) == 0) {
	    zeroes += padding;
	    padding = 0;
	}
    }

    if ((flags & FMT_leftside) == 0)
	pad (format, padding, padchar);
    fmtappend (format, prefix, pre);
    pad (format, zeroes, '0');
    fmtappend (format, number, len);
    if (flags & FMT_leftside)
	pad (format, padding, padchar);
}

static int cconv (Format *format, int c) {
    fmtputc (format, va_arg (format->args, int));
    return FMT_verb;
}

static int dconv (Format *format, int c) {
    intconv (format, 10, 0, "");
    return FMT_verb;
}

static int oconv (Format *format, int c) {
    intconv (format, 8, 0, "0");
    return FMT_verb;
}

static int xconv (Format *format, int c) {
    intconv (format, 16, 0, "0x");
    return FMT_verb;
}

static int litconv (Format *format, int c) {
    fmtputc (format, c);
    return FMT_verb;
}

extern int fmt_badconv (Format *format, int c) {
    eprint ("libprint: bad conversion char '%c'\n", c);
    format->error = 1; /* XXX - stop or not? consume an argv? */
    return FMT_verb;
}

static int nconv (Format* format, int c)
{
    /* write the current write length into an int* argument */
    int *p = va_arg (format->args, int*);
    if (p) *p = format->buf - format->bufbegin + format->flushed;
    return FMT_verb;
}

static int rconv (Format *format, int c) {
    int e = errno;
    /* do we have to store errno in the format structure because
       the grow proc could make a syscall that sets errno before
       we get here? */

    if (format->flags & FMT_altform) {
	fmtprint (format, "%d", e);
    } else {
	/* strerror() might be better, but this way we can range check */
        extern int sys_nerr;
        extern char* sys_errlist[];
	if ((0 <= e) && (e < sys_nerr)) 
	    fmtcat (format, sys_errlist[e]);
	else
	    fmtprint (format, "unknown error (%d)", e);
    }
    return FMT_verb;	
}

/*
 * conversion table management
 */

static Fmtconv dflt_fmttab[FMT_MAXCONV];

static Fmtconv* ensure_inittab (Fmtconv fmttab[])
{
    int i;

    if (fmttab[0] != 0)
	return fmttab;

    for (i = 0; i < FMT_MAXCONV; i++)
	fmttab[i] = fmt_badconv;

    fmttab['%'] = litconv;

    fmttab['s'] = sconv;
    fmttab['c'] = cconv;
    fmttab['d'] = dconv;
    fmttab['i'] = dconv;
    fmttab['o'] = oconv;
    fmttab['x'] = xconv;
    fmttab['r'] = rconv;
    fmttab['m'] = rconv;
    fmttab['n'] = nconv;

    fmttab['u'] = uconv;
    fmttab['h'] = hconv;
    fmttab['l'] = lconv;
    fmttab['#'] = altconv;
    fmttab['-'] = leftconv;
    fmttab['.'] = dotconv;
    fmttab['*'] = starconv;

    fmttab['0'] = zeroconv;
    for (i = '1'; i <= '9'; i++)
	fmttab[i] = digitconv;

    return fmttab;
}

extern Fmtconv fmtinstall (int c, Fmtconv f)
{
    Fmtconv oldf;

    ensure_inittab (dflt_fmttab);
    c &= FMT_MAXCONV - 1;
    oldf = dflt_fmttab[c];
    if (f != 0)
	dflt_fmttab[c] = f;
    return oldf;
}

/* --- */

static void fmtinitbuf (Format *format, 
			char *buf, SIZE_T size, int (*grow)(Format *, SIZE_T))
{
    format->buf      = buf;
    format->bufbegin = buf;
    format->bufend   = buf + size;
    format->grow     = grow;
    format->flushed  = 0;
    format->error    = 0;
    format->reqlen   = 0;
}

static void fmtinitarg (Format *format, 
			va_list args, Fmtconv* fmttab, void* client_data)
{
    format->args = args;
    format->fmttab = 0;
    format->client_data = 0;
}
		     
/*
 * the formatting engine
 */

extern int fmtengine (Format *format, const char *fmt)
{
    const unsigned char *s = (const unsigned char *) fmt;
    Fmtconv* fmttab;
 
    if ((fmttab = format->fmttab) == 0)
        fmttab = ensure_inittab (dflt_fmttab);

    for (;;) {
	int c = *s++;

	switch (c) {
	case '%':
	    format->flags = format->f1 = format->f2 = 0;
	    do c = *s++; while ((*fmttab[c])(format, c) == FMT_flag);
	    break;
	case '\0':
	    return format->buf - format->bufbegin + format->flushed;
	default:
	    fmtputc (format, c);
	    break;
	}
    }
}


/*
 * the public entry points
 */

extern int fmtprint (Format *format, const char *fmt,...) {
    int n = -format->flushed;
    va_list ap, saveargs;

    va_start (ap, fmt);
    saveargs = format->args;
    format->args = ap;
    n += fmtengine (format, fmt);
    va_end (format->args);
    format->args = saveargs;

    return n + format->flushed;
}

/* --- */

static int writeall (int fd, char *buf, SIZE_T n) {
    int i, remain;

    for (i = 0, remain = n; remain > 0; buf += i, remain -= i)
	if ((i = write (fd, buf, remain)) <= 0)
	    if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
		continue;
	    else
		break;

    return n - remain; 
}

/* --- */

extern int fmt_fprint_flush (Format *format, SIZE_T more) {
    SIZE_T n = format->buf - format->bufbegin;
    SIZE_T r;

    r = writeall (format->u.n, format->bufbegin, n);
    format->flushed += r;
    format->buf -= r;
    format->error |= (r != n);

    return format->error;
}

extern int vfprint (int fd, const char *fmt, va_list ap) {
    char buf[1024];
    Format format;

    fmtinitbuf (&format, buf, sizeof buf, fmt_fprint_flush);
    fmtinitarg (&format, ap, 0, 0);
    format.u.n = fd;

    fmtengine (&format, fmt);
    fmt_fprint_flush (&format, 0);
    return format.error ? -1 : format.flushed;
}

extern int fprint (int fd, const char *fmt,...) {
    int n;
    va_list ap;

    va_start (ap, fmt);
    n = vfprint (fd, fmt, ap);
    va_end (ap);
    return n;
}

extern int print (const char *fmt,...) {
    int n;
    va_list ap;

    va_start (ap, fmt);
    n = vfprint (1, fmt, ap);
    va_end (ap);
    return n;
}

extern int eprint (const char *fmt,...) {
    int n;
    va_list ap;

    va_start (ap, fmt);
    n = vfprint (2, fmt, ap);
    va_end (ap);
    return n;
}

/* --- */

extern int fmt_memprint_grow (Format *format, SIZE_T more) {
    char *buf = 0;
    SIZE_T len = format->bufend - format->bufbegin + 1;
    len = (len >= more)
	? len * 2 
        : ((len + more) + FMT_PRINT_ALLOCSIZE) &~ (FMT_PRINT_ALLOCSIZE - 1);

    buf = ((Alloc)format->u.p)(format->bufbegin, len);
    if (buf) {
	format->buf	 = buf + (format->buf - format->bufbegin);
	format->bufbegin = buf;
	format->bufend   = buf + len - 1;
	return 0;
    }
    return format->error = 1;
}

extern char *fmt_memprint (Format *format, const char *fmt, SIZE_T* len) {
    char* buf;
    int n;

    buf = (*(Alloc)format->u.p) (0, FMT_PRINT_ALLOCSIZE);
    if (!buf) {
	format->error = 1;
	return 0;
    }

    fmtinitbuf (format, buf, FMT_PRINT_ALLOCSIZE - 1, fmt_memprint_grow);
    n = fmtengine (format, fmt);
    *format->buf = '\0';
    if (len) *len = n;

    return format->error ? 0 : format->bufbegin;
}

extern char *vsmprint (Alloc a, SIZE_T* len, const char* fmt, va_list ap) {
    Format format;

    fmtinitarg (&format, ap, 0, 0);
    format.u.p = (void*) a;

    return fmt_memprint (&format, fmt, len);
}

extern char *smprint (Alloc alloc, SIZE_T* len, const char* fmt, ...) {
    char* result;
    va_list ap;

    va_start (ap, fmt);
    result = vsmprint (alloc, len, fmt, ap);
    va_end (ap);
    return result;
}

/* note -- moved palloc and mprint to mprint.c */

/* --- */

extern int fmt_snprint_grow (Format *format, SIZE_T more) {
    return format->error=1;
}

extern int vsnprint (char *buf, int buflen, const char *fmt, va_list ap) {
    Format format;

    fmtinitbuf (&format, buf, buflen - 1, fmt_snprint_grow);
    fmtinitarg (&format, ap, 0, 0);
    format.args = ap;

    (void) fmtengine (&format, fmt);
    *format.buf = '\0';
    return format.reqlen;
}

extern int snprint (char *buf, int buflen, const char *fmt,...) {
    int n;
    va_list ap;

    va_start (ap, fmt);
    n = vsnprint (buf, buflen, fmt, ap);
    va_end (ap);
    return n;
}

extern int sprint (char *buf, const char *fmt,...) {
    int n;
    va_list ap;

    va_start (ap, fmt);
    n = vsnprint (buf, FMT_SPRINT_BUFSIZ, fmt, ap);
    va_end (ap);
    return n;
}
