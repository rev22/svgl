bin_PROGRAMS = 

all : .gendone


.gendone: 
	env PYTHONPATH="$(svgl_builddir)/scripts" $(PYTHON) genFromIdl.py
	env PYTHONPATH="$(svgl_builddir)/scripts" $(PYTHON) makeParseAttribute.py
	env PYTHONPATH="$(svgl_builddir)/scripts" $(PYTHON) genAttributeParser.py
	env PYTHONPATH="$(svgl_builddir)/scripts" $(PYTHON) genSvgElement.py
	env PYTHONPATH="$(svgl_builddir)/scripts" $(PYTHON) makeColors.py
	$(TOUCH) .gendone

clean:
	$(RM) *.pyc
	$(RM) .gendone

EXTRA_DIST = *.py
