TEX=pdflatex
TFLAGS=-shell-escape -interaction=nonstopmode -file-line-error

SVG = $(shell find assets -type f -name '*.svg' ! -name '*-registered.svg')
REGISTERED = $(SVG:.svg=-registered.svg)
PYTHON = python3

srs.pdf: srs.tex assets $(REGISTERED)
	$(TEX)	$(TFLAGS)	srs.tex

%-registered.svg: %.svg
	scripts/register.py $< $@

clean:
	rm	srs.pdf	srs.aux	srs.log	srs.out	srs.toc
