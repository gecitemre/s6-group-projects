TEX=pdflatex
TFLAGS=-shell-escape -interaction=nonstopmode -file-line-error

hw3.pdf: hw3.tex q8c.py q8d.py
	$(TEX) $(TFLAGS) $<

q8c.py q8d.py: q8a.py q8b.py
	python3 $<

%.pdf: %.tex
	$(TEX) $(TFLAGS) $<

clean:
	rm	-f	*.aux *.log *.out *.toc *.pdf