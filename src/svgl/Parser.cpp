/*
 * $Id$
 *
Copyright (c) 2001 Stephane Conversy, Jean-Daniel Fekete and Ecole des
Mines de Nantes.
All rights reserved.
 
This software is proprietary information of Stephane Conversy,
Jean-Daniel Fekete and Ecole des Mines de Nantes.  You shall use it
only in accordance with the terms of the license agreement you
accepted when downloading this software.  The license is available in
the file licence.txt and at the following URL:
http://www.emn.fr/info/image/Themes/Indigo/licence.html

 */

#include <w3c/svg/svg.hpp>
#include <svgl/Parser.hpp>
#include <w3c/svg/String.hpp>
// #include <w3c/svg/strings.hpp>

//#include <w3c/dom/dom.hpp>
#include <stdlib.h>
#include <w3c/dom/ext/DOMParser.hpp>
#include <utils/StdioInputSource.hpp>
#include <utils/StringInputSource.hpp>
#include <utils/EntityResolver.hpp>
#include <w3c/dom/ext/DOMWriter.hpp>
#include <w3c/dom/DOMImplementation.hpp>
#include <w3c/dom/Document.hpp>

#include <w3c/svg/SVGDocument.hpp>
#include <svgl/debug.hpp>

static bool SVGRegistered=0;

static void
registerSVG()
{
  if (SVGRegistered)
    return;
  SVGRegistered=true;
  const char * cat = getenv("CATALOG_FILE");
  if (cat == 0)
    sdom::EntityResolver::addCatalogFile(unicode::String::createString("share/sdom_external_entities"));
  else
    sdom::EntityResolver::addCatalogFile(unicode::String::createString(cat));

  dom::DOMImplementation * impl = dom::DOMImplementation::getDOMImplementation();

  unicode::String * svg_string = unicode::String::internString("svg");

  unicode::String * svg_publicId_10 = unicode::String::internString("-//W3C//DTD SVG 1.0//EN");
  unicode::String * svg_systemId_10 = unicode::String::internString("http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd");
  dom::DocumentType * svg_doctype_10 = impl->createDocumentType(svg_string, svg_publicId_10, svg_systemId_10);
  svg::SVGDocument * svg_document_10 = new svg::SVGDocument();
  svg_document_10->appendChild(svg_doctype_10);
  impl->registerPrototype(svg_document_10);

  unicode::String * svg_publicId_11 = unicode::String::internString("-//W3C//DTD SVG 1.1 Tiny//EN");
  unicode::String * svg_systemId_11 = unicode::String::internString("http://www.w3.org/Graphics/SVG/1.1/DTD/svg11-tiny.dtd");
  dom::DocumentType * svg_doctype_11 = impl->createDocumentType(svg_string, svg_publicId_11, svg_systemId_11);
  svg::SVGDocument * svg_document_11 = new svg::SVGDocument();
  svg_document_11->appendChild(svg_doctype_11);
  impl->registerPrototype(svg_document_11);
}


namespace svgl {

  Parser::Parser()
  {
    registerSVG();
  }


	svg::SVGDocument *Parser::parseInputSource(sdom::InputSource *input) {
		dom::DOMParser parser;

		try {
			parser.parse(input);
		}
		catch(dom::DOMException &e) {
			std::cerr << "Error: " << e.what() << __FL__;
			return 0;
		}
		catch(std::exception &e) {
			std::cerr << "Exception: " << e.what() << __FL__;
			return 0;
		}
		dom::Document *domdoc = parser.getDocument();
		svg::SVGDocument *svgdoc = dynamic_cast< svg::SVGDocument * >(domdoc);
   
		if(!svgdoc) {
			std::cerr << "not an svg file" << __FL__;
			std::cerr << DBGVAR(domdoc) << DBGVAR(svgdoc) << __FL__;
			return 0;
		}

		dom::Node *node = svgdoc->getFirstChild(); // should be doctype
		svg::SVGSVGElement *svgroot = dynamic_cast< svg::SVGSVGElement * >(node);

		while(node && !svgroot) {
			node = node->getNextSibling();
			svgroot = dynamic_cast< svg::SVGSVGElement * >(node);
		}

		//std::cerr << DBGVAR(node) << DBGVAR(svgroot) << __FL__;

		if(!svgroot) {
			std::cerr << "not an svg file" << __FL__;
			return 0;
		}

		//svgdoc->getRootElement().setValue(svgroot);
		svgroot->setOwnerAndViewPort(svgroot, svgroot);
		svgdoc->updateStyle();

		svgroot->registerUse(0);
    
		return svgdoc;
	}
	
	
	svg::SVGDocument *Parser::parse(const char *url) {
		FILE * in  = fopen(url, "rb");
		if (in == 0) {
			std::cerr << "could not open file" __FL__;
			return 0;
		}

		sdom::StdioInputSource input(in, unicode::String::createString(url));
		return parseInputSource(&input);
	}

	svg::SVGDocument *Parser::parseFromString(unicode::String *svgdesc) {
		sdom::StringInputSource input(svgdesc);
		return parseInputSource(&input);
	}

}
