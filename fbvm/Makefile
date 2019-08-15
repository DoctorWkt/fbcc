OBJS = fbvm.o
CFLAGS = -g -Wall -O2 -m32
LIBS =
LFLAGS	= -m32

all: fbvm

clean:
	rm -f *.o *~ fbvm fbvmspec.h

# Use the i386 version by default
fbvmspec.h: fbvmspec_i386.h
	cp fbvmspec_i386.h fbvmspec.h

fbvm.o: fbvm.c fbvmspec.h fbvminstr.h

fbvm: $(OBJS)
	gcc -g $(OBJS) $(LFLAGS) -o fbvm $(LIBS)

.c.o:
	gcc	$(CFLAGS) -c $*.c

