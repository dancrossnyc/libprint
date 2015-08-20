#include <stdlib.h>
#include "print.h"

static const char rcsid[] = 
"@(#)$Id: mprint.c,v 1.4 1995/07/04 18:24:52 schwartz Exp $";

/* this is what ansi realloc does, but not e.g. sunos realloc */
extern char* palloc (char* p, SIZE_T size) {
    return p ? realloc(p, size) : malloc(size);
}

extern char *mprint(const char *fmt,...) {
    char* result;
    va_list ap;

    va_start(ap, fmt);
    result = vsmprint (palloc, 0, fmt, ap);
    va_end(ap);
    return result;
}
