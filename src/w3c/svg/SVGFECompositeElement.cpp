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
#include <w3c/svg/SVGFECompositeElement.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/TextureSnapshot.hpp>
#include <svgl/FilterContext.hpp>

#include <svgl/screenshot.hpp>
#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

namespace svg {


  unicode::String * 
  SVGFECompositeElement::getOptionalIn1()
  {
    unicode::String * in1 = GETVAL(In1);
    return in1;
  }

  unicode::String *
  SVGFECompositeElement::getOptionalIn2()
  {
    return GETVAL(In2);
  }

  unicode::String *
  SVGFECompositeElement::getOptionalResult()
  {
    return GETVAL(Result);
  }


  void
  SVGFECompositeElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo*, SVGElement * elem, svgl::FilterContext* filterContext)
  {
  }

  void
  SVGFECompositeElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, SVGElement * elem, svgl::FilterContext* filterContext)
  {

#define STRDEF(x) static unicode::String * x = unicode::String::internString(#x);
    STRDEF(SourceGraphic)
      //STRDEF(SourceAlpha)
      //STRDEF(BackgroundImage)
      //STRDEF(BackgroundAlpha)
      //STRDEF(FillPaint)
      //STRDEF(StrokePaint)
#undef STRDEF

    unicode::String * result = GETVAL(Result);
    unicode::String * in1 = GETVAL(In1);
    unicode::String * in2 = GETVAL(In2);

    svgl::TextureSnapshot * snap1=0;
    if(in1 && in1->getLength()!=0) {
      //std::cerr << DBGVAR(in1) << __FL__;
      snap1 = filterContext->results[in1];
    }
    else {
      snap1 = filterContext->results[SourceGraphic];
    }

    svgl::TextureSnapshot * snap2=0;
    if(in2 && in2->getLength()!=0) {
      //std::cerr << DBGVAR(in2) << __FL__;
      snap2 = filterContext->results[in2];
    }
    else {
      snap2 = filterContext->results[SourceGraphic];
    }

    const SVG_FECOMPOSITE_OPERATOR& op = GETVAL(Operator);

#if 0
    SVGNumber
      k1 = GETVAL(K1),
      k2 = GETVAL(K2),
      k3 = GETVAL(K3),
      k4 = GETVAL(K4);
#endif

    const int& xi = filterContext->x;
    const int& yi = filterContext->y;
    const int& widthi = filterContext->width;
    const int& heighti = filterContext->height;
    //const int& dxi = filterContext->dx;
    //const int& dyi = filterContext->dy;

    switch(op) {
    default:
    case SVG_FECOMPOSITE_OPERATOR_OVER:
      if(snap2) { snap2->pop(); }
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      if(snap1) { snap1->pop(); }
      break;

    case SVG_FECOMPOSITE_OPERATOR_IN:
#if 1
      glBlendFunc(GL_ONE, GL_ZERO);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
      if(snap2) { snap2->pop();}
      glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
#endif
      if(snap1) snap1->pop();
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      break;

    case SVG_FECOMPOSITE_OPERATOR_OUT:
#if 1
      glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
      //svgl::screenshot("1.pam", GL_RGBA);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
      if(snap2) {snap2->pop();}
      //svgl::screenshot("2.pam", GL_RGBA);
      glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
#endif
      if(snap1) {snap1->pop();}
      //svgl::screenshot("3.pam", GL_RGBA);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      break;
    }



    if(result && (*result!=*SourceGraphic)) {
      svgl::TextureSnapshot * snapshot = new svgl::TextureSnapshot(xi,yi,widthi,heighti);
      filterContext->results[result] = snapshot;	
    }
    filterContext->lastResult = result;


#if 0
    enum SVG_FECOMPOSITE_OPERATOR {
      SVG_FECOMPOSITE_OPERATOR_UNKNOWN = 0,
      SVG_FECOMPOSITE_OPERATOR_OVER = 1,
      SVG_FECOMPOSITE_OPERATOR_IN = 2,
      SVG_FECOMPOSITE_OPERATOR_OUT = 3,
      SVG_FECOMPOSITE_OPERATOR_ATOP = 4,
      SVG_FECOMPOSITE_OPERATOR_XOR = 5,
      SVG_FECOMPOSITE_OPERATOR_ARITHMETIC = 6
    };
#endif


  }

}
