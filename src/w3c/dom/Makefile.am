.SILENT: @DISABLE_SILENT@

SUBDIRS = impl ext
noinst_LTLIBRARIES = libsrcw3c_dom.la
libsrcw3c_dom_la_LIBADD = impl/libsrcw3c_domimpl.la\
		       ext/libsrcw3c_domext.la
libsrcw3c_dom_la_SOURCES = Attr.cpp\
			AttrDef.cpp\
			CDATASection.cpp\
			CharacterData.cpp\
			ChildNode.cpp\
			Comment.cpp\
			DOMException.cpp\
			DOMImplementation.cpp\
			Document.cpp\
			DocumentFragment.cpp\
			DocumentType.cpp\
			Element.cpp\
			Entity.cpp\
			EntityReference.cpp\
			Node.cpp\
			Notation.cpp\
			ParentNode.cpp\
			ProcessingInstruction.cpp\
			Text.cpp

AM_CPPFLAGS = -I$(top_builddir)/src/

pkgincludedir = @SVGL_INCLUDEDIR@/w3c/dom
pkginclude_HEADERS = \
	Attr.hpp\
	AttrDef.hpp\
	CDATASection.hpp\
	CharacterData.hpp\
	ChildNode.hpp\
	Comment.hpp\
	DOMException.hpp\
	DOMImplementation.hpp\
	Document.hpp\
	DocumentFragment.hpp\
	DocumentType.hpp\
	Element.hpp\
	Entity.hpp\
	EntityReference.hpp\
	NamedNodeMap.hpp\
	Node.hpp\
	NodeList.hpp\
	Notation.hpp\
	ParentNode.hpp\
	ProcessingInstruction.hpp\
	Text.hpp\
	config.hpp\
	dom.hpp\
	string.hpp

