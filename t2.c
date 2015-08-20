#include "print.h"

int main()
{
    char buf[32];
    int i;

    i = snprint (buf, sizeof buf, "%z123456789012345678901234567890");
    print ("snprint (%s) -> %d\n", buf, i);

    i = snprint (buf, sizeof buf, "123456789012345678901234567890");
    print ("snprint (%s) -> %d\n", buf, i);

    i = snprint (buf, sizeof buf, "1234567890123456789012345678901");
    print ("snprint (%s) -> %d\n", buf, i);

    i = snprint (buf, sizeof buf, "12345678901234567890123456789012");
    print ("snprint (%s) -> %d\n", buf, i);

    exit(0);
}
