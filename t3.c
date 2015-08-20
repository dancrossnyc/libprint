#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "print.h"


Fmtconv date_tab[FMT_MAXCONV];

// XXX - expose these things from print.c?

static int
badconv(Format *format, int c)
{
	eprint("dateconv: bad conversion char '%c'\n", c);
	format->error = 1;
	return FMT_verb;
}

static int
litconv(Format *format, int c)
{
	fmtputc(format, c);
	return FMT_verb;
}

// XXX - expose intconv() instead?

static void
put_int02(Format *format, int i)
{
	Fmtconv *save = format->fmttab;
	format->fmttab = 0;
	fmtprint(format, "%02d", i);
	format->fmttab = save;
}

static int
dconv(Format *format, int c)
{
	struct tm *tm = (struct tm *)format->client_data;
	put_int02(format, tm->tm_mday);
	return FMT_verb;
}

static int
mconv(Format *format, int c)
{
	struct tm *tm = (struct tm *)format->client_data;
	put_int02(format, tm->tm_mon + 1);
	return FMT_verb;
}

static int
yconv(Format *format, int c)
{
	struct tm *tm = (struct tm *)format->client_data;
	put_int02(format, tm->tm_year);
	return FMT_verb;
}

static int
Dconv(Format *format, int c)
{
	fmtprint(format, "%m/%d/%y");
	return FMT_verb;
}

static int
Hconv(Format *format, int c)
{
	struct tm *tm = (struct tm *)format->client_data;
	put_int02(format, tm->tm_hour);
	return FMT_verb;
}

static int
Mconv(Format *format, int c)
{
	struct tm *tm = (struct tm *)format->client_data;
	put_int02(format, tm->tm_min);
	return FMT_verb;
}

static int
Sconv(Format *format, int c)
{
	struct tm *tm = (struct tm *)format->client_data;
	put_int02(format, tm->tm_sec);
	return FMT_verb;
}

static int
Tconv(Format *format, int c)
{
	fmtprint(format, "%H:%M:%S");
	return FMT_verb;
}

static void
init_date_tab()
{
	int i;
	Fmtconv *fmttab = date_tab;

	for (i = 0; i < FMT_MAXCONV; i++)
		fmttab[i] = badconv;

	fmttab['%'] = litconv;

	fmttab['d'] = dconv;	// day of month
	fmttab['m'] = mconv;	// month
	fmttab['y'] = yconv;	// year
	fmttab['D'] = Dconv;	// %m/%d/%y

	fmttab['H'] = Hconv;	// hour
	fmttab['M'] = Mconv;	// min
	fmttab['S'] = Sconv;	// sec
	fmttab['T'] = Tconv;	// %H:%M:%S
}

char *
format_date(time_t clock, char *fmt)
{
	struct tm tm = *gmtime(&clock);
	Format format;

	memset(&format, 0, sizeof(format));
	format.buf = 0;
	format.bufbegin = 0;
	format.bufend = 0;
	format.flushed = 0;
	format.error = 0;

	format.u.p = (void *)palloc;
	format.grow = fmt_memprint_grow;
	format.fmttab = date_tab;
	format.client_data = &tm;

	if (date_tab[0] == 0)
		init_date_tab();

	return fmt_memprint(&format, fmt, 0);
}

int
main()
{
	char *ds = format_date(time(0), "%% the date is %D at time %T");
	print("%s\n", ds);
	free(ds);
	return 0;
}
