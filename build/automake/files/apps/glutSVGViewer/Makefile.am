
#.SILENT: @DISABLE_SILENT@

noinst_PROGRAMS = glutSVGViewer

glutSVGViewer_SOURCES = glutSVGViewer.cpp GlutTimeManager.cpp
glutSVGViewer_LDADD = -lsvgl
glutSVGViewer_LDFLAGS = -framework GLUT -framework Foundation -L$(top_builddir)/src -lstdc++_ZeroLink

INCLUDES = -I$(top_builddir)/src/

