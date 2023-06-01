TEX=pdflatex
TFLAGS=-shell-escape -interaction=nonstopmode -file-line-error
FIGURES = $(wildcard figures/*.svg)
TARGET = hw4.pdf

$(TARGET).pdf: $(TARGET).tex $(FIGURES)
	$(TEX) $(TFLAGS) $<

%.pdf: %.tex
	$(TEX) $(TFLAGS) $<

clean:
	rm	-f	*.aux *.log *.out *.toc *.pdf