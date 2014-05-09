## -*- Makefile -*- mode
## Process this file with automake to create Makefile.in

.SILENT: @DISABLE_SILENT@

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool


SUBDIRS=glft demos

EXTRA_DIST = LICENSE

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = glft.pc
