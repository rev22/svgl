.SILENT: @DISABLE_SILENT@

noinst_PROGRAMS = 00simplest 01loading 02zoomAndPan 03picking 04useExternal 05text 06animation

00simplest_SOURCES = 00-simplest.cpp
00simplest_LDADD =  -lsvgl -lglut
00simplest_LDFLAGS =  -framework GLUT -framework Foundation  -L$(top_builddir)/src
01loading_SOURCES = 01-loading.cpp
01loading_LDADD =  -lsvgl -lglut
01loading_LDFLAGS =  -framework GLUT -framework Foundation -L$(top_builddir)/src
02zoomAndPan_SOURCES = 02-zoomAndPan.cpp
02zoomAndPan_LDADD =  -lsvgl -lglut
02zoomAndPan_LDFLAGS =  -framework GLUT -framework Foundation -L$(top_builddir)/src
03picking_SOURCES = 03-picking.cpp
03picking_LDADD =  -lsvgl -lsvgltest -lglut
03picking_LDFLAGS =  -framework GLUT -framework Foundation -L$(top_builddir)/tests -L$(top_builddir)/src
04useExternal_SOURCES = 04-useExternal.cpp
04useExternal_LDADD =  -lsvgl -lsvgltest -lglut
04useExternal_LDFLAGS =  -framework GLUT -framework Foundation -L$(top_builddir)/tests -L$(top_builddir)/src
05text_SOURCES = 05-text.cpp
05text_LDADD =  -lsvgl -lglut
05text_LDFLAGS =  -framework GLUT -framework Foundation -L$(top_builddir)/src
06animation_SOURCES = 06-animation.cpp GlutTimeManager.cpp GlutTimeManager.hpp
06animation_LDADD =  -lsvgl -lglut
06animation_LDFLAGS =  -framework GLUT -framework Foundation -L$(top_builddir)/src

AM_CPPFLAGS = -I$(top_builddir)/src/ -L$(top_builddir)/src


