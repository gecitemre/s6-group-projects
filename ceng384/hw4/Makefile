TEX=pdflatex
TFLAGS=-shell-escape -interaction=nonstopmode -file-line-error
FIGURES = $(wildcard figures/*.pgf)
TARGET = hw4

$(TARGET).pdf: $(TARGET).tex $(FIGURES)
	$(TEX) $(TFLAGS) $<

$(FIGURES): q5.py q1c.py
	python3 q5.py
	python3 q1c.py

%.pdf: %.tex
	$(TEX) $(TFLAGS) $<

clean:
	rm	-f	*.aux *.log *.out *.toc *.pdf
