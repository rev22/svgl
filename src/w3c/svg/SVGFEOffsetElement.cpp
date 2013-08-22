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
#include <w3c/svg/SVGFEOffsetElement.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/Context.hpp>

#include <svgl/FilterContext.hpp>
#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>
//#include <svgl/screenshot.hpp>

namespace svg {

  unicode::String *
  SVGFEOffsetElement::getOptionalIn1()
  {
    unicode::String * in1 = GETVAL(In1);
    return in1;
  }

  unicode::String *
  SVGFEOffsetElement::getOptionalIn2()
  {
    return 0;
  }

  unicode::String *
  SVGFEOffsetElement::getOptionalResult()
  {
    return GETVAL(Result);
  }

  void
  SVGFEOffsetElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo*, SVGElement * elem, svgl::FilterContext* filterContext)
  {
  }

  void
  SVGFEOffsetElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, SVGElement * elem, svgl::FilterContext* filterContext)
  {
    unicode::String * in1 = GETVAL(In1);
    unicode::String * result = GETVAL(Result);
    svg::SVGLength dxl = GETVAL(Dx);
    svg::SVGLength dyl = GETVAL(Dy);

    float dx = svglContext->computeWidthLength(dxl);
    float dy = svglContext->computeHeightLength(dyl);
    int dxi = (int)dx;
    int dyi = (int)dy;

#if 0
    if(in1)
      std::cerr << DBGVAR(*in1) << __FL__;
    std::cerr << DBGVAR(dx) << DBGVAR(dy) << __FL__;
#endif
    bool willBeCopied = false;
    if(!result || result->getLength()==0)
      willBeCopied = true;


    if(dxi && dyi) {
      filterContext->dx=dxi;
      filterContext->dy=dyi;
      int stencil = glinfo->stencil;
      SVGFilterPrimitiveStandardAttributes::glTraverse(svglContext, glinfo, elem, filterContext, in1, result);
      //svgl::screenshot(glinfo->winWidth, glinfo->winHeight, GL_RGB);
      if(willBeCopied) {
	glPushMatrix();
	glinfo->stencilPush();
	glTranslatef( filterContext->x+dxi, filterContext->y+dyi, 0);
	glRectf(0,0,filterContext->width, filterContext->height);
	glinfo->stencilEnableTop();
	glStencilFunc(GL_EQUAL, stencil, static_cast<GLuint>(-1));
	glTranslatef(-dxi, -dyi, 0);
	//glDisable(GL_STENCIL_TEST);
	glColor3f(1,1,1);
	glRectf(0,0,filterContext->width, filterContext->height);
	glinfo->stencilPop();
	glTranslatef(dxi, dyi, 0);
	glRectf(0,0,filterContext->width, filterContext->height);
	glinfo->stencilEnableTop();
	glPopMatrix();
      }
      filterContext->dx=0;
      filterContext->dy=0;
    }
  }

}
