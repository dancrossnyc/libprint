
#include "print.h"

int main()
{
    int i;
    char ch;

    for (i=0; i<256; ++i) {
	ch = i;
        print ("%o %o %03uo\n", i, ch, ch);
    }
    exit (0);
}
