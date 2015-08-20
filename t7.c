
#include "print.h"

int main()
{
    char junk[] = "abcdefghijklmnopqrstuvwzyz";
    int jsize = sizeof junk;
    int width = jsize + 5;
    print("%*.*s\n", width, jsize, junk);
    print("%#*.*s\n", width, jsize, junk);
    exit (0);
}
