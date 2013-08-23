.SILENT: @DISABLE_SILENT@

SUBDIRS = agg svgl utils w3c
lib_LTLIBRARIES = libsvgl.la
libsvgl_la_LDFLAGS = $(abs_top_srcdir)/glft/glft/libglft.la
libsvgl_la_SOURCES =
libsvgl_la_LIBADD = agg/libsrcagg.la\
		    svgl/libsrcsvgl.la\
		    utils/libsrcutils.la\
		    w3c/libsrcw3c.la
