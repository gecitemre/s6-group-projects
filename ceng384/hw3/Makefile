TEX=pdflatex
TFLAGS=-shell-escape -interaction=nonstopmode -file-line-error
FIGURES = $(wildcard figures/*.svg)

hw3.pdf: hw3.tex $(FIGURES)
	$(TEX) $(TFLAGS) $<

square_wave%: q8c.py
	python3 q8c.py

sawtooth_wave%: q8d.py
	python3 q8d.py

%.pdf: %.tex
	$(TEX) $(TFLAGS) $<

clean:
	rm	-f	*.aux *.log *.out *.toc *.pdf