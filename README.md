libprint: a Plan 9-inspired formatted output library
====================================================

libprint 1.2
------------

Modernize this release of libprint so that it compiles with clang
and GCC.

libprint 1.1
------------

This release of libprint includes an important bug fix, and two
enhancements.  The bug was a typo in writeall() that could result in an
infinite loop.  The enhancements are a change in the return value of
snprint (to be more like 4.4bsd), and support for 16 bit Unicode
"Runes" (contributed by Erik Quanstrom based on code from Rob Pike's
sam editor).

libprint 1.0
------------

The routines in this library format strings similarly to the printf
family of functions.  They are better in the following ways:

* they are small and efficient
  (about 5K on a sparc)
* user installable format specifiers
  (e.g. %I to print an ip quad given a struct in_addr*)
* you can print into dynamically allocated memory
  (using either malloc or a user supplied allocation function)
* they are reentrant
  (as all good code should be)

This code is derived from Paul Haahr's print library, which is part of
Byron Rakitzis' popular implementation of the rc shell.  This release
cleans up a number of bugs, makes everything reentrant, and adds a
uniform set of entry points.

This package doesn't do floating point conversions, although they would
be easy to add.

To compile, edit the Makefile.  It is organized for modern systems
using GNU make.

There are a number of test programs included; see them for examples
if you don't want to read the manpage.

* Dan Cross <crossd@gmail.com>
* Scott Schwartz <schwartz@cse.psu.edu>
