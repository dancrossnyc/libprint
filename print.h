/* from rc 1.4 - code by Byron and Paul */
#ifndef __nicer_print_h
#define __nicer_print_h

#include <stdarg.h>

#ifndef SIZE_T
typedef unsigned long int SIZE_T;
#endif

enum {
  /* return values of conversion functions */
  FMT_flag = 0,			/* uhl#-.[0-9] */
  FMT_verb = 1,			/* scdoxr */
  FMT_error = 2,  		/* error during formatting */

  /* sizes of various things */
  FMT_MAXCONV = 256,	  	/* number of conversion characters */
  FMT_PRINT_ALLOCSIZE = 64,	/* grow by this many bytes when necessary */
  FMT_SPRINT_BUFSIZ = 1024	/* bytes */
};

typedef char* (*Alloc)(char *, SIZE_T);

typedef struct Format Format;
typedef int (*Fmtconv)(Format *, int);

struct Format {
	/* for the formatting routines */
	va_list args;
	long flags, f1, f2;
	Fmtconv* fmttab;
	/* for the buffer maintainence routines */
	char *buf, *bufbegin, *bufend;
	int flushed;
	int (*grow)(Format *, SIZE_T);
	int error;
	int reqlen;
	union { int n; void *p; } u;
	/* for the sake of reentrancy */
	void* client_data;
};

/* Format->flags values */
enum {
	FMT_long	= (1<<0),	/* %l */
	FMT_short	= (1<<1),	/* %h */
	FMT_unsigned	= (1<<2),	/* %u */
	FMT_zeropad	= (1<<3),	/* %0 */
	FMT_leftside	= (1<<4),	/* %- */
	FMT_altform	= (1<<5),	/* %# */
	FMT_f1set	= (1<<6),	/* %<n> */
	FMT_f2set	= (1<<7)	/* %.<n> */
};

extern int fmtputc(Format *f, const char c);
extern int fmtappend(Format *format, const char *s, SIZE_T len);
extern int fmtcat(Format *format, const char *s);
extern Fmtconv fmtinstall(int c, Fmtconv f);
extern int fmtengine(Format *format, const char *fmt);
extern int fmtprint(Format *format, const char *fmt,...);
extern int fmt_fprint_flush (Format *format, SIZE_T more);
extern int vfprint(int fd, const char *fmt, va_list ap);
extern int fprint(int fd, const char *fmt,...);
extern int print(const char *fmt,...);
extern int eprint(const char *fmt,...);
extern char *fmt_memprint (Format *format, const char *fmt, SIZE_T* len);
extern int fmt_memprint_grow (Format *format, SIZE_T more);
extern char *vsmprint (Alloc a, SIZE_T* len, const char* fmt, va_list ap);
extern char *smprint (Alloc alloc, SIZE_T* len, const char* fmt, ...);
extern int fmt_snprint_grow (Format *format, SIZE_T more);
extern char* palloc (char* p, SIZE_T size);
extern char *mprint(const char *fmt,...);
extern int vsnprint(char *buf, int buflen, const char *fmt, va_list ap);
extern int snprint(char *buf, int buflen, const char *fmt,...);
extern int sprint(char *buf, const char *fmt,...);

extern int print_inaddr_conv (Format*, int);
extern int print_cquote_conv (Format*, int);
extern int print_rcquote_conv (Format*, int);

extern void fmt_install_runeconv ();

#endif /* __nicer_print_h */
