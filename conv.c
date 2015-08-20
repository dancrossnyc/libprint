/* miscellaneous conversion functions for the print library */

static const char rcsid[] =
"@(#)$Id: conv.c,v 1.10 1995/07/04 18:24:52 schwartz Exp $";

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "print.h"

/* %I prints a struct in_addr* in dotted quad form */

int print_inaddr_conv(Format *format, int c) 
{
    struct in_addr* in;

    in = va_arg (format->args, struct in_addr*);
    if (in)
	fmtcat (format, inet_ntoa (*in));
    else
	fmtcat (format, "0.0.0.0");
    return FMT_verb;
}

/* %Q writes a string quoted for reading by /bin/rc */

int print_rcquote_conv (Format* format, int c)
{
    unsigned char* s = va_arg (format->args, unsigned char*);
    unsigned char ch;

    while ((ch = *s++)) {
	if (ch == '\'') 
	    fmtputc (format, ch);
	fmtputc (format, ch);
    }
    return FMT_verb;
}

int print_cquote_conv (Format* format, int c)
{
    unsigned char* s = va_arg (format->args, unsigned char*);
    unsigned char ch;

    /* XXX - ignores f1 and f2 */
    while ((ch = *s++)) {
	if (isascii (ch) && isalnum (ch)) {
	    if (ch == '\\')
		fmtputc (format, ch);
	    fmtputc (format, ch);
	} else {
	    fmtprint (format, "\\%03uo", ch);
	}
    }
    return FMT_verb;
}
