TEX=pdflatex
TFLAGS=-shell-escape
SVG = $(shell find assets -type f -name '*.svg' ! -name '*registered.svg')
REGISTERED = $(SVG:.svg=-registered.svg)

srs.pdf: srs.tex assets $(REGISTERED)
	$(TEX)	$(TFLAGS)	srs.tex

%-registered.svg: %.svg
	python3 scripts/register.py $< $@

clean:
	rm	srs.pdf	srs.aux	srs.log	srs.out	srs.toc