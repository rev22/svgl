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
#include <w3c/svg/SVGFEMergeElement.hpp>
#include <w3c/svg/SVGFEMergeNodeElement.hpp>
#include <svgl/GLInfo.hpp>

#include <svgl/FilterContext.hpp>
#include <svgl/getattr.hpp>

namespace svg {

  unicode::String *
  SVGFEMergeElement::getOptionalIn1()
  {
    return 0;
  }

  unicode::String *
  SVGFEMergeElement::getOptionalIn2()
  {
    return 0;
  }

  unicode::String *
  SVGFEMergeElement::getOptionalResult()
  {
    return GETVAL(Result);
  }


  void
  SVGFEMergeElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo*, SVGElement * elem, svgl::FilterContext* filterContext)
  {
  }

  void
  SVGFEMergeElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, SVGElement * elem, svgl::FilterContext* filterContext)
  {
    glColor4f(1,1,1,0);
    filterContext->glRect();
    for(dom::Node *domchild = getFirstChild(); domchild; domchild = domchild->getNextSibling() ) {
      SVGFEMergeNodeElement * fe = dynamic_cast<SVGFEMergeNodeElement*>(domchild);
      if(fe) {
	fe->glPostTraverse(svglContext, glinfo, elem, filterContext);
      }
    }
  }

}
