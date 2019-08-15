FBCC = ../fbcc/fbcc
FBAS = ../fbas/fbas
FBINCLUDE = .
FBLIBS = startup.ss fblib.s

all: fblib.s getopt.s

#
# getopt.c
#
getopt.s: getopt.c

#
# stdlib
#
fblib.s: fblib.c


clean:
	rm -f *.s *.i *~

.c.s:
	gcc -E -I$(FBINCLUDE) $*.c >$*.i
	$(FBCC) < $*.i > $*.s
