DIRS = fbvm fbcc fbas lib test2

all:
	for i in $(DIRS); \
	do \
	  $(MAKE) -C $$i all || exit 1 ; \
	done

clean:
	for i in $(DIRS); \
	do \
	  $(MAKE) -C $$i clean || exit 1 ; \
	done

tar:
	tar zcvf fbcc.tar.gz -C .. fbcc/COPYING fbcc/README \
	fbcc/Makefile \
	fbcc/fbcc fbcc/fbas fbcc/fbvm fbcc/lib fbcc/test2 fbcc/doc

