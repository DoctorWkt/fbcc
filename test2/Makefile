FBCC = ../fbcc/fbcc
FBAS = ../fbas/fbas
FBINCLUDE = ../lib
FBLIBS = ../lib/startup.ss ../lib/fblib.s

all: dijkstra test1 testopt
	chmod +x vm
	-./vm dijkstra
	-./vm test1
	-./vm testopt arg1 arg2 lastarg

# exemple 1
dijkstra: dijkstra.s
	cat $(FBLIBS) dijkstra.s | $(FBAS) -o dijkstra

# exemple 2
test1: test1.s
	cat $(FBLIBS) test1.s | $(FBAS) -o test1

# getopt test
testopt: testopt.s
	cat $(FBLIBS) ../lib/getopt.s testopt.s | $(FBAS) -o testopt

clean:
	rm -f *.s *.i *~ test1 testopt dijkstra tralala

.c.s:
	gcc -E -I$(FBINCLUDE) $*.c >$*.i
	$(FBCC) < $*.i > $*.s
