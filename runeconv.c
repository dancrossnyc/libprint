static const char rcsid[] = 
"@(#)$Id: runeconv.c,v 1.4 1995/07/04 18:24:52 schwartz Exp $";

/* Contributed by Erik Quanstrom <quanstro@sartre.minerva.bah.com> */

#include "print.h"
#include "rune.h"

static void pad (Format *format, SIZE_T len, int c) {
    do; while (len-- != 0 && fmtputc (format, c));
}

static SIZE_T Runelen (const Rune *r) {
	const Rune *s = r;
	while (*r) r++;
	return r-s;
}

/* delete this when you get a decent compiler */
static Rune stupid[] = { 923, 0};

static int Sconv (Format *format, int c) {
    SIZE_T maxrunes, minwidth, extra;
    Rune *R = va_arg (format->args, Rune*);
    char s[UTFmax]; 
    int i;

/*    if (!R) R = L"Λ";  some compilers choke on this*/
    if (!R) R = stupid;

    if (format->flags & FMT_altform) {
	maxrunes = (format->flags & FMT_f2set) ? format->f2 : Runelen (R);
	/* maxrunes may overrun R,  but this way we don't have 
	   to assume that R is null terminated when f2 is set. */
    } else {
	/* do f2 like printf */
	maxrunes = Runelen (R);
	if ((format->flags & FMT_f2set) && (format->f2 < maxrunes))
	    maxrunes = format->f2;
    }

    minwidth = (format->flags & FMT_f1set) ? format->f1 : maxrunes;
    extra = (minwidth > maxrunes) ? minwidth - maxrunes : 0;

    if (format->flags & FMT_leftside) {
	while (maxrunes--) {
    		i = runetochar (s, R++);
    		fmtappend (format, s, i);
	}
	pad (format, extra, ' ');
    } else {
	pad (format, extra, ' ');
	while (maxrunes--) {
    		i = runetochar (s, R++);
    		fmtappend (format, s, i);
	}
    }
	
    return FMT_verb;
}

static int Cconv (Format *format, int c) {
    Rune r = (unsigned short) va_arg (format->args, int);
    char s[UTFmax];
    int i;

    i = runetochar (s, &r);
    fmtappend (format, s, i);
    return FMT_verb;
}

void fmt_install_runeconv ()
{
    fmtinstall ('C', Cconv);
    fmtinstall ('S', Sconv);
}

