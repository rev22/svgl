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
#include <w3c/svg/SVGFESpecularLightingElement.hpp>
#include <svgl/GLInfo.hpp>

#include <svgl/FilterContext.hpp>
#include <svgl/getattr.hpp>


namespace svg {

  unicode::String *
  SVGFESpecularLightingElement::getOptionalIn1()
  {
    unicode::String * in1 = GETVAL(In1);
    return in1;
  }

  unicode::String *
  SVGFESpecularLightingElement::getOptionalIn2()
  {
    return 0;
  }

  unicode::String *
  SVGFESpecularLightingElement::getOptionalResult()
  {
    return GETVAL(Result);
  }


  void
  SVGFESpecularLightingElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo*, SVGElement * elem, svgl::FilterContext* filterContext)
  {
  }

  void
  SVGFESpecularLightingElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo*, SVGElement * elem, svgl::FilterContext* filterContext)
  {
  }

}
