.SILENT: @DISABLE_SILENT@

noinst_LTLIBRARIES = libsrcutils.la
libsrcutils_la_SOURCES = EntityResolver.cpp\
			FileInputSource.cpp\
			IstreamInputSource.cpp\
			StdioInputSource.cpp\
			String.cpp\
			StringBuffer.cpp\
			StringImpl.cpp\
			StringInputSource.cpp\
			StringUtils.cpp
AM_CPPFLAGS = -I$(top_builddir)/src/
pkgincludedir = @SVGL_INCLUDEDIR@/utils
pkginclude_HEADERS = \
	EntityResolver.hpp\
	FileInputSource.hpp\
	InputSource.hpp\
	List.hpp\
	StdioInputSource.hpp\
	IstreamInputSource.hpp\
	String.hpp\
	StringBuffer.hpp\
	StringImpl.hpp\
	StringInputSource.hpp\
	StringList.hpp\
	StringUtils.hpp\
	gc_alloc.hpp\
	gc_cpp.h

