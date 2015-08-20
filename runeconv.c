// Contributed by Erik Quanstrom <quanstro@sartre.minerva.bah.com>

#include "print.h"
#include "rune.h"

static void
pad(Format *format, size_t len, int c)
{
	while (len-- != 0 && fmtputc(format, c));
}

static size_t
Runelen(const Rune *r)
{
	const Rune *s = r;
	while (*r)
		r++;
	return r - s;
}

// Delete this when you get a decent compiler.
static Rune runelit[] = { 923, 0 };

static int
Sconv(Format *format, int c)
{
	size_t maxrunes, minwidth, extra;
	Rune *R = va_arg(format->args, Rune *);
	char s[UTFmax];
	int i;

	// if (!R) R = L"Λ";  // some compilers choke on this
	if (!R)
		R = runelit;

	if (format->flags & FMT_altform) {
		// maxrunes may overrun R,  but this way we don't have
		// to assume that R is null terminated when f2 is set.
		maxrunes = (format->flags & FMT_f2set) ? format->f2 : Runelen(R);
	} else {
		// do f2 like printf
		maxrunes = Runelen(R);
		if ((format->flags & FMT_f2set) && (format->f2 < maxrunes))
			maxrunes = format->f2;
	}

	minwidth = (format->flags & FMT_f1set) ? format->f1 : maxrunes;
	extra = (minwidth > maxrunes) ? minwidth - maxrunes : 0;

	if (format->flags & FMT_leftside) {
		while (maxrunes--) {
			i = runetochar(s, R++);
			fmtappend(format, s, i);
		}
		pad(format, extra, ' ');
	} else {
		pad(format, extra, ' ');
		while (maxrunes--) {
			i = runetochar(s, R++);
			fmtappend(format, s, i);
		}
	}

	return FMT_verb;
}

static int
Cconv(Format *format, int c)
{
	Rune r = (unsigned short)va_arg(format->args, int);
	char s[UTFmax];
	int i;

	i = runetochar(s, &r);
	fmtappend(format, s, i);
	return FMT_verb;
}

void
fmt_install_runeconv()
{
	fmtinstall('C', Cconv);
	fmtinstall('S', Sconv);
}
