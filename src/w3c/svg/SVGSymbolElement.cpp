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
#include <w3c/svg/SVGSymbolElement.hpp>
#include <w3c/svg/SVGPreserveAspectRatio.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <svgl/getattr.hpp>

namespace svg {

#ifdef WITH_XERCESC
  void
  SVGSymbolElement::parse(const DOM_Node& node, SVGSVGElement* owner, SVGElement* viewport)
  {
    SVGElement::parse(node, owner, this);
    ownerSVGElement.setValue(owner);
    viewportElement.setValue(viewport);
  }
#endif

  void
  SVGSymbolElement::setOwnerAndViewPort(SVGSVGElement* owner, SVGElement* viewport)
  {
    SVGElement::setOwnerAndViewPort(owner, this);
    ownerSVGElement.setValue(owner);
    viewportElement.setValue(viewport);
  }

  void
  SVGSymbolElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
  }

  void
  SVGSymbolElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
  }

  void
  SVGSymbolElement::glTraverseFromUse(svgl::Context * svglContext, svgl::GLInfo* glinfo, float width, float height)
  {
    glPushMatrix();

    const SVGRect& viewbox = GETVAL(ViewBox);
    SVGPreserveAspectRatio& pAR =  MYGETVAL(preserveAspectRatio);

    pAR.glPreTraverse(svglContext, glinfo, viewbox, width, height);

    float oldwidth = svglContext->viewport_width;
    float oldheight = svglContext->viewport_height;
    svglContext->setViewportWidthHeight(width, height);

    glinfo->pushPickInfo(this);
    glTraverseChildren(svglContext, glinfo);
    glinfo->popPickInfo();

    svglContext->setViewportWidthHeight(oldwidth, oldheight);

    pAR.glPostTraverse(svglContext, glinfo, viewbox, width, height);

    glPopMatrix();

  }

  void
  SVGSymbolElement::pickTraverseFromUse(svgl::Context * svglContext, svgl::GLInfo* glinfo, float width, float height)
  {
    glPushMatrix();

    const SVGRect& viewbox = GETVAL(ViewBox);
    SVGPreserveAspectRatio& pAR =  MYGETVAL(preserveAspectRatio);

    pAR.glPreTraverse(svglContext, glinfo, viewbox, width, height);

    float oldwidth = svglContext->viewport_width;
    float oldheight = svglContext->viewport_height;
    svglContext->setViewportWidthHeight(width, height);

    glinfo->pushPickInfo(this);
    pickTraverseChildren(svglContext, glinfo);
    glinfo->popPickInfo();

    svglContext->setViewportWidthHeight(oldwidth, oldheight);

    pAR.glPostTraverse(svglContext, glinfo, viewbox, width, height);

    glPopMatrix();

  }


}

