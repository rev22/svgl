.SILENT: @DISABLE_SILENT@

noinst_LTLIBRARIES = libsrcw3c_css.la
libsrcw3c_css_la_SOURCES = CSSRGBColor.cpp\
			CSSStyle.cpp\
			CSSURI.cpp
AM_CPPFLAGS = -I$(top_builddir)/src/

pkgincludedir = @SVGL_INCLUDEDIR@/w3c/css
pkginclude_HEADERS = \
	CSSRule.hpp\
	CSSStyleDeclaration.hpp\
	CSSURI.hpp\
	CSSValue.hpp\
	CurrentColor.hpp\
	DocumentCSS.hpp\
	FontStyle.hpp\
	RGBColor.hpp\
	TextAnchor.hpp\
	URIColor.hpp\
	ViewCSS.hpp\
	inherited_style_defs.hpp\
	not_inherited_style_defs.hpp\
	style_defs.hpp

