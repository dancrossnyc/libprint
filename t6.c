#include "print.h"
#include "rune.h"

Rune stupid[] = {945,946,947,948,0};

int 
main(int c, char **v){
/*	Rune r = L'α';*/
	Rune r = 945;

	fmt_install_runeconv ();
	for (r = 945; r<945+26;r++)
		fprint(2, "%C%C\n", r,r);
	fprint(2, "'%S'\n", stupid);
	fprint(2, "'%.2S'\n", stupid);
	fprint(2, "'%10.10S'\n", stupid);
	fprint(2, "'%10.10s'\n", "abcd");
	fprint(2, "'%-10.10S'\n",stupid);
	fprint(2, "%S\n", (Rune)0);
	return 0;
}
