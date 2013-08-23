## -*- Makefile -*- mode
## Process this file with automake to create Makefile.in
#
# demo/Makefile.am --
#
# Copyright (C) Nicolas Roussel
#
# See the file LICENSE for information on usage and redistribution of
# this file, and for a DISCLAIMER OF ALL WARRANTIES.
#

.SILENT: @DISABLE_SILENT@

AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir}
AM_LDFLAGS = ${top_builddir}/glft/libglft.la

if OS_IS_MACOSX
  AM_LDFLAGS += -framework glut -framework Cocoa
else
if OS_IS_CYGWIN
  AM_LDFLAGS += -lglut32
else
  AM_LDFLAGS += -lglut
endif
endif

noinst_PROGRAMS = simple autoscale

simple_SOURCES = simple.cpp
autoscale_SOURCES = autoscale.cpp
