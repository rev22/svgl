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
#include <w3c/svg/SVGDefsElement.hpp>

#include <svgl/GLInfo.hpp>
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace svg {
  
  void
  SVGDefsElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
  }

  void
  SVGDefsElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
  }


}
