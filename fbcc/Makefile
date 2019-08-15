OBJS = fblist.o fbgen.o fbtype.o fbconst.o fberror.o fbsym.o fbparse.o fbcc.o
CFLAGS = -m32 -g -Wall -O2 -I../fbvm
LIBS =
LFLAGS	= -m32

all: fbcc

clean:
	rm -f *.o *.i *.s *~ fbcc fbfbcc fbfbcc1 lex.yy.c fbparse.c

fbcc: $(OBJS)
	gcc -g $(OBJS) $(LFLAGS) -o fbcc $(LIBS)

fbsym.o: fbsym.c fbcc.h

fblist.o: fblist.c fbcc.h

fbtype.o: fbtype.c fbcc.h

fbgen.o: fbgen.c fbcc.h

fbconst.o: fbconst.c fbcc.h

lex.yy.c: fblex.l fbcc.h
	flex fblex.l

fbparse.c: lex.yy.c fbparse.y fbcc.h
	bison -o fbparse.c fbparse.y

.c.o:
	gcc	$(CFLAGS) -c $*.c

