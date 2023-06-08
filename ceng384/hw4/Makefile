TEX=pdflatex
TFLAGS=-shell-escape -interaction=nonstopmode -file-line-error
FIGURES = $(wildcard figures/*.pgf)
TARGET = hw4
PYTHON_FILE = q5.py

$(TARGET).pdf: $(TARGET).tex $(FIGURES)
	$(TEX) $(TFLAGS) $<


decoded.pgf dft_decoded.pgf dft_encoded.pgf encoded.pgf: q5.py
	python3 q5.py

q1c.pgf: q1c.py
	python3 q1c.py

%.pdf: %.tex
	$(TEX) $(TFLAGS) $<

clean:
	rm	-f	*.aux *.log *.out *.toc *.pdf
