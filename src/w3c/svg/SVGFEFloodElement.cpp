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

#include <w3c/svg/SVGFEFloodElement.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/Context.hpp>
#include <w3c/css/RGBColor.hpp>
#include <svgl/TextureSnapshot.hpp>
#include <svgl/FilterContext.hpp>

#include <svgl/debug.hpp>
//#include <svgl/screenshot.hpp>
#include <svgl/getattr.hpp>

namespace svg {

  unicode::String *
  SVGFEFloodElement::getOptionalIn1()
  {
    return 0;
  }

  unicode::String *
  SVGFEFloodElement::getOptionalIn2()
  {
    return 0;
  }

  unicode::String *
  SVGFEFloodElement::getOptionalResult()
  {
    return GETVAL(Result);
  }

  void
  SVGFEFloodElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo*, SVGElement * elem, svgl::FilterContext* filterContext)
  {
  }

  void
  SVGFEFloodElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, SVGElement * elem, svgl::FilterContext* filterContext)
  {
    unicode::String * result = GETVAL(Result);
    //unicode::String * in1 = GETVAL(In1);

    css::RGBColor c;
    c.r=c.g=c.b=1;
    float opacity=1;

    glColor4f(c.r, c.g, c.b, opacity);
    filterContext->glRect();
    if(result && result->getLength()!=0) {
      //glinfo->stencilDisableTest();
      svgl::TextureSnapshot * snapshot = new svgl::TextureSnapshot(filterContext->x,filterContext->y,filterContext->width,filterContext->height);
      //glinfo->stencilEnableTest();
      filterContext->results[result] = snapshot;
    }

  }

}
