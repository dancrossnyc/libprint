#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "print.h"

extern int print_inaddr_conv(Format*,int);

char junk[9000];

void main(int argc, char**argv)
{
  struct in_addr* in = 0;
  struct rlimit rl;
  long int i, j;
  unsigned long int u;
  short int h;
  unsigned short int uh;

  fmtinstall('I', print_inaddr_conv);

  if (argc > 1) {
      rl.rlim_cur = rl.rlim_max = 1024 * atoi(argv[1]);
      if (setrlimit (RLIMIT_DATA,&rl) < 0)
	  eprint("setrlimit: %r");
  }
  
  for (i = 0; i < sizeof(junk); ++i) 
    junk[i] = (i&63) + ' ';
  junk[sizeof(junk) - 1] = 0;

  /* print nuls */
  print ("%c%c\n", 0, 0);

  /* test badconv */
  print ("%Z", 0);

  /* integers */
  print("%d %#xd %0d %10.5d\n", 3,4,5,6);

  /* nconv */
  print ("%s\n%n", "foo", &i);
  print ("len=%d\n", i);

  /* print ip quads */
  { struct hostent* h;
    for (h = gethostbyname ("globin.cse.psu.edu"); 
	h && *h->h_addr_list; h->h_addr_list++) {
      in = (struct in_addr*) *h->h_addr_list;
      print("g=%#ux [%I]\n", in->s_addr, in);
    }
  }

  /* print errno */
  for (errno = 0; errno < 10; ++errno) {
    i = print("error %#r: %r");
    print(" (%d)\n", i);
  }
  errno = 345; i = print("error %#r: %r"); print(" (%d)\n", i);


  /* indirect field specs */
  for (i = 0; i < 10; ++i)
	for (j = 0; j < 5; ++j)
		print ("%*.*s\n", i, j, "12345");

  for (i = 0; i < 10; ++i)
	for (j = 0; j < 10; ++j)
		print ("%*.*d\n", i, j, 12345);

  /* short conversions */
  uh = 55555;  h = -12345;
  print("-12345 %hd\n55555 %uhd\n", h, uh);

  h = 32766; print("32766 %hd\n", h);
  h = 32767; print("32767 %hd\n", h);
  h = 32768; print("32768 %hd\n", h);
  h = 32769; print("32769 %hd\n", h);
  
  uh = 32766; print("32766 %uhd\n", uh);
  uh = 32767; print("32767 %uhd\n", uh);
  uh = 32768; print("32768 %uhd\n", uh);
  uh = 32769; print("32769 %uhd\n", uh);

  h = -3; print("-3 %hd\n", h);
  h = -2; print("-2 %hd\n", h);
  h = -1; print("-1 %hd\n", h);
  h = -0; print("-0 %hd\n", h);

  uh = -3; print("-3 %hd\n", uh);
  uh = -2; print("-2 %hd\n", uh);
  uh = -1; print("-1 %hd\n", uh);
  uh = -0; print("-0 %hd\n", uh);

  i = fprint(1, "junk=%s\n", junk);

  /* force extra memory allocations */
  print(">>> %s\n", 
	smprint(palloc, (SIZE_T*)&i,
		"%d %s %s %s %I %d %r\n", 
		strlen(junk), junk, "\n===\n", "\n---\n", in, 12312) );
  print("rc=%d %r\n", i);

  i = fprint(1, "!!! %d %s %s %s %I %d %r\n", 
	 strlen(junk), junk, "\n===\n", "\n---\n", in, 12312);
  print("rc=%d\n", i);
  _exit(0);
}
