.SILENT: @DISABLE_SILENT@

noinst_LTLIBRARIES = libsrcw3c_domimpl.la
libsrcw3c_domimpl_la_SOURCES = \
			AttributeMap.cpp\
			NamedNodeMapImpl.cpp\
			NodeListImpl.cpp

AM_CPPFLAGS = -I$(top_builddir)/src/

pkgincludedir = @SVGL_INCLUDEDIR@/w3c/dom/impl
pkginclude_HEADERS = \
		AttributeMap.hpp\
		NamedNodeMapImpl.hpp\
		NodeListImpl.hpp


