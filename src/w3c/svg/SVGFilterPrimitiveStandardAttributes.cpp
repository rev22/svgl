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
#include <w3c/svg/SVGFilterPrimitiveStandardAttributes.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/TextureSnapshot.hpp>
#include <svgl/FilterContext.hpp>

#include <svgl/debug.hpp>

namespace svg {

  unicode::String *
  SVGFilterPrimitiveStandardAttributes::getOptionalIn1()
  {
    return 0;
  }

  unicode::String *
  SVGFilterPrimitiveStandardAttributes::getOptionalIn2()
  {
    return 0;
  }

  unicode::String *
  SVGFilterPrimitiveStandardAttributes::getOptionalResult()
  {
    return 0;
  }


  void
  SVGFilterPrimitiveStandardAttributes::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo*, SVGElement * elem, svgl::FilterContext* filterContext)
  {
  }

  void
  SVGFilterPrimitiveStandardAttributes::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo*, SVGElement * elem, svgl::FilterContext* filterContext)
  {
  }

  void
  SVGFilterPrimitiveStandardAttributes::glTraverse(svgl::Context*, svgl::GLInfo* glinfo, SVGElement* elem, svgl::FilterContext* filterContext, unicode::String* in1, unicode::String* result)
  {
#define STRDEF(x) static unicode::String * x = unicode::String::internString(#x);
    STRDEF(SourceGraphic)
      //STRDEF(SourceAlpha)
      //STRDEF(BackgroundImage)
      //STRDEF(BackgroundAlpha)
      //STRDEF(FillPaint)
      //STRDEF(StrokePaint)
#undef STRDEF

    if(!in1 || in1->getLength()==0) {
      in1=filterContext->lastResult;
    }

#if 1


    const int& xi = filterContext->x;
    const int& yi = filterContext->y;
    const int& widthi = filterContext->width;
    const int& heighti = filterContext->height;
    const int& dxi = filterContext->dx;
    const int& dyi = filterContext->dy;

    //std::cerr << *in1 << __FL__;
    //bool saveInTexture=false;
    GLenum format = GL_RGBA;

    if (*in1==*SourceGraphic) {
      svgl::TextureSnapshot * snapshot = filterContext->results[in1];
      if(!snapshot) {
	// do nothing
      }
      else {
	snapshot->pop();	
      }

    }
    else {
      svgl::TextureSnapshot * snapshot = filterContext->results[in1];
      if(!snapshot) {
	std::cerr << "filter source \"" << in1 << "\" not implemented or not found" __FL__;
      } else {
	//glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
	glBlendFunc(GL_ONE, GL_ZERO);
	//glinfo->stencilDisableTest();
	glColor4f(0,1,0,.2);
	#if 0
	glRectf(xi,yi,xi+widthi,yi+heighti);
	glEnd();
	#endif

	snapshot->pop();
	//glinfo->stencilEnableTest();
	format = snapshot->getFormat();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
    }

    if(!result || result->getLength()==0) {
      glRasterPos2i(xi+dxi, yi+dyi);
      //glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
      glBlendFunc(GL_ONE, GL_ZERO);
      //#define DBGCOPYPIX
#ifdef DBGCOPYPIX
      glRasterPos2i(xi+20, yi+20);
      glinfo->stencilDisableTest();
#endif
      //glDisable(GL_SEPARABLE_2D);
      if(format==GL_ALPHA) {
	glPixelTransferf(GL_RED_SCALE, 0);
	glPixelTransferf(GL_GREEN_SCALE, 0);
	glPixelTransferf(GL_BLUE_SCALE, 0);
      }
      glCopyPixels(xi, yi,widthi,heighti,GL_COLOR);
#ifdef DBGCOPYPIX
      glinfo->stencilEnableTest();
#endif
#undef DBGCOPYPIX
      if(format==GL_ALPHA) {
	glPixelTransferf(GL_RED_SCALE, 1);
	glPixelTransferf(GL_GREEN_SCALE, 1);
	glPixelTransferf(GL_BLUE_SCALE, 1);
      }
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      result=SourceGraphic;
    }
    else {
      //glinfo->stencilDisableTest();
      svgl::TextureSnapshot * snapshot = new svgl::TextureSnapshot(xi,yi,widthi,heighti, format);
      //glinfo->stencilEnableTest();
      filterContext->results[result] = snapshot;

    }

    filterContext->lastResult = result;
#endif

  }


}
