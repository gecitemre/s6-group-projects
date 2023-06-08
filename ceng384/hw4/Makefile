TEX=pdflatex
TFLAGS=-shell-escape -interaction=nonstopmode -file-line-error
FIGURES = $(wildcard figures/*.pgf)
TARGET = hw4
PYTHON_FILE = $(TARGET).py

$(TARGET).pdf: $(TARGET).tex $(FIGURES) $(PYTHON_FILE)
	$(TEX) $(TFLAGS) $<

%.pdf: %.tex
	$(TEX) $(TFLAGS) $<

clean:
	rm	-f	*.aux *.log *.out *.toc *.pdf