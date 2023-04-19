#	No spaces are allowed, only tabs
#	compiler
TEX=pdflatex
#	compiler	flags
TFLAGS=-shell-escape

PYTHON = python

IMAGES = images/q7a.png	images/original.png

hw2.pdf: hw2.tex $(IMAGES)
	$(TEX)	$(TFLAGS) hw2.tex

$(IMAGES): hw2.py
	$(PYTHON) hw2.py

clean:
	rm	hw2.pdf	hw2.aux	hw2.log	hw2.out	hw2.toc
	rm -rf images/