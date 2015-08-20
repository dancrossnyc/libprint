#include "print.h"

int
main()
{
	fmtinstall('Q', print_cquote_conv);
	fmtinstall('R', print_rcquote_conv);

	print("\"%Q\"\n", "\001\002\200\127abcde\nend\n");
	print("'%R'\n", "this can't be and isn't either\n");

	return 0;
}
