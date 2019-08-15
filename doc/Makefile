TEX_FILE = fbcc_slides


all: $(TEX_FILE).dvi

clean:
	rm *~
	
$(TEX_FILE).dvi: $(TEX_FILE).tex fbcc1.tex
	slitex $(TEX_FILE).tex
	rcp $(TEX_FILE).dvi poly:.
	

