// Miscellaneous conversion functions for the print library.

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ctype.h>
#include <netdb.h>
#include <stddef.h>

#include "print.h"

// %I prints a IPv4 address in dotted quad form.
int
print_inaddr_conv(Format *format, int c)
{
	struct in_addr *in;

	in = va_arg(format->args, struct in_addr *);
	fmtcat(format, (in != NULL) ? inet_ntoa(*in) : "0.0.0.0");

	return FMT_verb;
}

// %R writes a string quoted for reading by /bin/rc.
int
print_rcquote_conv(Format *format, int c)
{
	unsigned char *s = va_arg(format->args, unsigned char *);
	unsigned char ch;

	while ((ch = *s++) != '\0') {
		if (ch == '\'')
			fmtputc(format, ch);
		fmtputc(format, ch);
	}

	return FMT_verb;
}

// %Q writes a string quoted as a literal C string.
int
print_cquote_conv(Format *format, int c)
{
	unsigned char *s = va_arg(format->args, unsigned char *);
	unsigned char ch;

	// XXX - ignores f1 and f2
	while ((ch = *s++) != '\0') {
		if (!isascii(ch) || !isalnum(ch)) {
			fmtprint(format, "\\%03uo", ch);
			continue;
		}
		if (ch == '\\')
			fmtputc(format, ch);
		fmtputc(format, ch);
	}

	return FMT_verb;
}
