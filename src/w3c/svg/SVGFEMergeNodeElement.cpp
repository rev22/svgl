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
#include <w3c/svg/SVGFEMergeNodeElement.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/FilterContext.hpp>
#include <svgl/TextureSnapshot.hpp>
#include <svgl/getattr.hpp>

namespace svg {

  unicode::String *
  SVGFEMergeNodeElement::getOptionalIn1()
  {
    return GETVAL(In1);
  }

  unicode::String *
  SVGFEMergeNodeElement::getOptionalIn2()
  {
    return 0;
  }

  unicode::String *
  SVGFEMergeNodeElement::getOptionalResult()
  {
    return 0;
  }


  void
  SVGFEMergeNodeElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo*, SVGElement * elem, svgl::FilterContext* filterContext)
  {
  }

  void
  SVGFEMergeNodeElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, SVGElement * elem, svgl::FilterContext* filterContext)
  {
    unicode::String * in1 = GETVAL(In1);

    if(!in1 || in1->getLength()==0) {
      svgl::TextureSnapshot * snap1 = filterContext->results[in1];
      snap1->pop();
    }
  }

}
