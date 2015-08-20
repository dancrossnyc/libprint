DEST=/usr/local	# /{lib,man,include}

CC=gcc
#CFLAGS=-fkeep-inline-functions -O2 -g -Wall
CFLAGS=-O2 -g -Wall
PIC=-fpic

#CC=acc
#CFLAGS=-O2
#PIC=-pic

LDFLAGS=-L. -lprint
REV=1.1

CO=co
COFLAGS=

SRC=print.c mprint.c conv.c runeconv.c rune.c
OBJ=print.o mprint.o conv.o runeconv.o rune.o
HDR=print.h rune.h
MAN=print.3
LIB=libprint.a libprint.so.$(REV)

all: $(LIB) test

T=t1 t2 t3 t4 t5 t6 t7 t8
test: $(T)
$(T): $(LIB)

libprint.a: $(OBJ)
	$(AR) rcv libprint.a $(OBJ)
	ranlib libprint.a

libprint.so.$(REV): shared $(OBJ)
	cd shared; ld -assert pure-text $(OBJ) -o ../$@

COPYRIGHT: print.3
	sed -e '/COPYRIGHT/,$$p' -e d <print.3 >COPYRIGHT

install:
	install -m 644 libprint.so.$(REV) $(DEST)/lib
	install -m 644 libprint.a $(DEST)/lib
	ranlib  -t $(DEST)/lib/libprint.a
	install -m 644 $(MAN) $(DEST)/man/man3
	install -m 644 print.h $(DEST)/include
clean:
	delete -f *.o a.out *.a *.s *.so.* t? *~ shared/* COPYRIGHT
depend:
	gcc -MM $(CFLAGS) $(SRC) >.depend

shar: COPYRIGHT
	shar COPYRIGHT blurb $(MAN) $(SRC) $(HDR) makefile .depend t*.c >print.shar

shared:
	mkdir shared

%.o: %.c
	rm -f $*.o shared/$*.o
	$(CC) $(CFLAGS) $(PIC) -c $< && mv $*.o shared
	$(CC) $(CFLAGS) -c $<
%.s: %.c
	$(CC) $(CFLAGS) -S -c $<
%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
%:: RCS/%,v
	test -f $@ || $(CO) $(COFLAGS) $< $@

include .depend
