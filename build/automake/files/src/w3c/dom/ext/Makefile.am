.SILENT: @DISABLE_SILENT@

noinst_LTLIBRARIES = libsrcw3c_domext.la
libsrcw3c_domext_la_SOURCES = \
			DOMParser.cpp\
			DOMVisitor.cpp\
			DOMWriter.cpp

AM_CPPFLAGS = -I$(top_builddir)/src/

pkgincludedir = @SVGL_INCLUDEDIR@/w3c/dom/ext
pkginclude_HEADERS = \
		DOMParser.hpp\
		DOMVisitor.hpp\
		DOMWriter.hpp\
		ErrorHandler.hpp

