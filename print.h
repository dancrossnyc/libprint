// From rc 1.4 - code by Byron and Paul.

#ifndef	PRINT_H_
#define	PRINT_H_

#include <stddef.h>
#include <stdarg.h>

enum {
	// return values of conversion functions
	FMT_flag = 0,			// uhl#-.[0-9]
	FMT_verb = 1,			// scdoxr
	FMT_error = 2,			// error during formatting

	// sizes of various things
	FMT_MAXCONV = 256,		// number of conversion characters
	FMT_PRINT_ALLOCSIZE = 64,	// grow by this many bytes when necessary
	FMT_SPRINT_BUFSIZ = 1024	// bytes
};

typedef char *(*Alloc)(char *, size_t);

typedef struct Format Format;
typedef int (*Fmtconv)(Format *, int);

struct Format {
	// For the formatting routines.
	va_list args;
	long flags, f1, f2;
	Fmtconv *fmttab;
	// For the buffer maintainence routines.
	char *buf, *bufbegin, *bufend;
	int flushed;
	int (*grow)(Format *, size_t);
	int error;
	int reqlen;
	union {
		int n;
		void *p;
	} u;
	// For the sake of reentrancy.
	void *client_data;
};

// Format->flags values
enum {
	FMT_long = (1 << 0),		// %l
	FMT_short = (1 << 1),		// %h
	FMT_unsigned = (1 << 2),	// %u
	FMT_zeropad = (1 << 3),		// %0
	FMT_leftside = (1 << 4),	// %-
	FMT_altform = (1 << 5),		// %#
	FMT_f1set = (1 << 6),		// %<n>
	FMT_f2set = (1 << 7)		// %.<n>
};

extern int fmtputc(Format *f, const char c);
extern int fmtappend(Format *format, const char *s, size_t len);
extern int fmtcat(Format *format, const char *s);
extern Fmtconv fmtinstall(int c, Fmtconv f);
extern int fmtengine(Format *format, const char *fmt);
extern int fmtprint(Format *format, const char *fmt, ...);
extern int fmt_fprint_flush(Format *format, size_t more);
extern int vfprint(int fd, const char *fmt, va_list ap);
extern int fprint(int fd, const char *fmt, ...);
extern int print(const char *fmt, ...);
extern int eprint(const char *fmt, ...);
extern char *fmt_memprint(Format *format, const char *fmt, size_t *len);
extern int fmt_memprint_grow(Format *format, size_t more);
extern char *vsmprint(Alloc a, size_t *len, const char *fmt, va_list ap);
extern char *smprint(Alloc alloc, size_t *len, const char *fmt, ...);
extern int fmt_snprint_grow(Format *format, size_t more);
extern char *palloc(char *p, size_t size);
extern char *mprint(const char *fmt, ...);
extern int vsnprint(char *buf, int buflen, const char *fmt, va_list ap);
extern int snprint(char *buf, int buflen, const char *fmt, ...);
extern int sprint(char *buf, const char *fmt, ...);

extern int print_inaddr_conv(Format *, int);
extern int print_cquote_conv(Format *, int);
extern int print_rcquote_conv(Format *, int);

extern void fmt_install_runeconv();

#endif	// PRINT_H_
