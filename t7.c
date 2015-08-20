#include <stddef.h>

#include "print.h"

int
main()
{
	char junk[] = "abcdefghijklmnopqrstuvwzyz";
	size_t jsize = sizeof junk;
	size_t width = jsize + 5;
	print("%*.*s\n", width, jsize, junk);
	print("%#*.*s\n", width, jsize, junk);

	return 0;
}
