TEX=pdflatex
TFLAGS=-shell-escape -interaction=nonstopmode -file-line-error

SVG = $(shell find assets -type f -name '*.svg' ! -name '*-registered.svg')
REGISTERED = $(SVG:.svg=-registered.svg)
TARGET = sad

$(TARGET).pdf: $(TARGET).tex assets $(REGISTERED)
	$(TEX)	$(TFLAGS)	$(TARGET).tex

%-registered.svg: %.svg
	scripts/register.py $<

group20.zip: $(TARGET).pdf diagrams/
	zip -r group20.zip $(TARGET).pdf diagrams/

clean:
	rm $(TARGET).pdf $(TARGET).aux $(TARGET).log $(TARGET).out $(TARGET).toc $(TARGET).lof $(TARGET).lot
