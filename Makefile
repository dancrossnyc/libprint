PREFIX:=	/usr/local
LDFLAGS:=	-L. -lprint
CFLAGS:=	-O -Wall -Werror -pedantic -std=c11

OBJS:=		print.o mprint.o conv.o runeconv.o rune.o
HDRS:=		print.h rune.h
MAN:=		print.3
LIB:=		libprint.a
T:=		t1 t2 t3 t4 t5 t6 t7 t8

all:		$(LIB) tests

$(LIB):		$(OBJS)
		$(AR) rcv $(LIB) $(OBJS)
		ranlib libprint.a

install:
		install -m 644 libprint.so.$(REV) $(PREFIX)/lib
		install -m 644 libprint.a $(PREFIX)/lib
		ranlib  -t $(PREFIX)/lib/libprint.a
		install -m 644 $(MAN) $(PREFIX)/man/man3
		install -m 644 $(HDRS) $(PREFIX)/include

clean:
		rm -f *.o a.out *.a *.s *.so.* t?

depend:
		gcc -MM $(CFLAGS) *.c >.depend

COPYRIGHT:	print.3
		sed -e '/COPYRIGHT/,$$p' -e d < print.3 > COPYRIGHT

tests:		$(T)
$(T):		$(LIB)

include		.depend
