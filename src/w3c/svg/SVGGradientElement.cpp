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
#include <w3c/svg/SVGGradientElement.hpp>
#include <w3c/svg/SVGUnitTypes.hpp>
#include <w3c/svg/SVGStopElement.hpp>
#include <w3c/css/RGBColor.hpp>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>


namespace svg {

  SVGGradientElement::SVGGradientElement(dom::Document * doc, unicode::String * tagName)
    : SVGElement(doc, tagName)
  {
    SETVAL(spreadMethod, SVG_SPREADMETHOD_PAD);
    SETVAL(gradientUnits, SVG_UNIT_TYPE_OBJECTBOUNDINGBOX);
  }

  void
  SVGGradientElement::getStopElements(svgl::StopElements* stops)
  {
    const css::RGBColor* lastcolor=0;
    float lastoffset=0;
    //    bool mustBlend = false;

    for(dom::Node *n = getFirstChild();
	n!=0;
	n=n->getNextSibling()) {
      SVGStopElement * s = dynamic_cast<SVGStopElement*>(n);
      if(!s)
	continue;
      const css::CSSStyle* style = s->getStyle().getValue().getStyle();
      const css::RGBColor * c = 0;
      float opacity=1;
      if (style) {
	c = dynamic_cast<const css::RGBColor*>(style->getStopColor());
	if(!c) {
	  std::cerr << "not an RGB color" << __FL__;
	  continue;
	}
	opacity = style->getStopOpacity();
      }
      if(!c) {
	if (!lastcolor) {
	  static css::RGBColor tmp;
	  tmp.r=tmp.g=tmp.b=0;
	  lastcolor=&tmp;
	  c=lastcolor;
	  //std::cerr << "no stop color for gradient " __FILE__":" << __LINE__ << std::endl;
	  //break;
	}
	else
	  c=lastcolor;
      }
      else
	lastcolor=c;
    

      //std::cerr << c->r << " " << c->g << " " << c->b << std::endl;
      /*      if(opacity<1)
	mustBlend=true;
      */

      SVGLength offsetl = s->GETVAL(Offset);
      float offset = offsetl.GET_SIMPLE_VAL(Value);
      
      if(offset<lastoffset || offset>1) {
	std::cerr << "suspect offset value " << offset << std::endl;
	offset=lastoffset;
      }
      //std::cerr << DBGVAR(offset) << __FL__;

      svgl::StopElementInfo stopinfo;
      stopinfo.color = c;
      stopinfo.opacity = opacity;
      stopinfo.offset = offset;

      stops->push_back(stopinfo);

      lastoffset=offset;
    }

  }

}
