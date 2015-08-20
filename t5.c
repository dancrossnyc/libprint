#include "print.h"

int
main()
{

	for (int i = 0; i < 256; ++i) {
		char ch = i;
		print("%o %o %03uo\n", i, ch, ch);
	}

	return 0;
}
