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
#include <w3c/svg/SVGClipPathElement.hpp>

#include <svgl/GLInfo.hpp>
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <w3c/css/RGBColor.hpp>

#include <svgl/debug.hpp>

namespace svg {

  void
  SVGClipPathElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    glinfo->stencilPush();
    // pick because we don't mind about color, transparency etc.
    //STDDBG;
    SVGElement::pickTraverseChildren(svglContext, glinfo);
    glinfo->stencilEnableTop();
  }

  void
  SVGClipPathElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    glinfo->stencilPop();
    SVGElement::pickTraverseChildren(svglContext, glinfo);
    glinfo->stencilEnableTop();
  }

  // should not be used
  void
  SVGClipPathElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
//    std::cerr << "SVGClipPathElement::glTraverse should not be used" __FL__;
    glPreTraverse(svglContext, glinfo);
    glPostTraverse(svglContext, glinfo);
  }

  void
  SVGClipPathElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    glPushName(0);
    glinfo->pushPickInfo(this);

    SVGElement::pickTraverseChildren(svglContext, glinfo);

    glPopName(); // this
    glPopName(); // 0
  }


}
