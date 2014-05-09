## -*- Makefile -*- mode
## Process this file with automake to create Makefile.in
#
# glft/Makefile.am --
#
# Copyright (C) Nicolas Roussel
#
# See the file LICENSE for information on usage and redistribution of
# this file, and for a DISCLAIMER OF ALL WARRANTIES.
#

.SILENT: @DISABLE_SILENT@

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir}

noinst_LTLIBRARIES = libglft.la

libglft_la_SOURCES = FontManager.cpp glft.cpp

libglftincludedir = @GLFT_INCLUDEDIR@
libglftinclude_HEADERS = FontInfo.hpp FontManager.hpp glft.hpp 
# debug.hpp and config.hpp are for internal use only
