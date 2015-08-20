#include <stdlib.h>
#include "print.h"

// this is what ansi realloc does, but not e.g. sunos realloc
extern char *
palloc(char *p, size_t size)
{
	return (p != NULL) ? realloc(p, size) : malloc(size);
}

extern char *
mprint(const char *fmt, ...)
{
	char *result;
	va_list ap;

	va_start(ap, fmt);
	result = vsmprint(palloc, 0, fmt, ap);
	va_end(ap);

	return result;
}
