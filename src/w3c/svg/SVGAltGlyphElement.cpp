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
the file license.txt and at the following URL:
http://www.emn.fr/info/image/Themes/Indigo/licence.html

 */
#include <w3c/svg/SVGAltGlyphElement.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/svg/SVGFilterElement.hpp>

#include <svgl/GLInfo.hpp>
#include <svgl/TextInfo.hpp>
#include <glft/FontManager.hpp>
#include <w3c/css/CSSStyleDeclaration.hpp>
#include <w3c/css/RGBColor.hpp>
#include <w3c/css/URIColor.hpp>

#include <w3c/dom/Text.hpp>

#include <cmath>

namespace svg {


  void
  SVGAltGlyphElement::glTextTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo)
  {
    for(dom::Node *n = getFirstChild(); n; n=n->getNextSibling()) {
      dom::Text * pcdata = dynamic_cast<dom::Text*>(n);
      if(pcdata) {
	glTextTraversePCDATA(svglContext, glinfo, textinfo, pcdata );
      }
      else {
	SVGTextContentElement *e = dynamic_cast<SVGTextContentElement*>(n);
	if(e) {
	  e->glTextTraverse(svglContext, glinfo, textinfo);
	}
      }
    }
  }

  void
  SVGAltGlyphElement::glTextTraversePCDATA(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo, dom::Text* pcdata)
  {
  }

}

