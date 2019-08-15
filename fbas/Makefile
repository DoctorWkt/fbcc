all: fbas

fbas: fbas.y lex.yy.c
	bison -o fbas.c fbas.y
	gcc -O2 -m32 -Wall -I. -I../fbvm fbas.c -o fbas

lex.yy.c: fbas_lex.l
	flex fbas_lex.l

clean:
	rm -f *.o *~ fbas.c lex.yy.c fbas
