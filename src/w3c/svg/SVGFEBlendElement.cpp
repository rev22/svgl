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
#include <w3c/svg/SVGFEBlendElement.hpp>
#include <svgl/GLInfo.hpp>
#include <w3c/svg/String.hpp>
#include <svgl/FilterContext.hpp>

#include <svgl/getattr.hpp>


#define DBG std::cerr << __FILE__":"<<__LINE__<<std::endl;

namespace svg {

  extern
  const String&
  print_SVGFEBlendElement_SVG_FEBLEND_MODE(SVGFEBlendElement::SVG_FEBLEND_MODE val);

  unicode::String *
  SVGFEBlendElement::getOptionalIn1()
  {
    return GETVAL(In1);
  }

  unicode::String *
  SVGFEBlendElement::getOptionalIn2()
  {
    return GETVAL(In2);
  }

  unicode::String *
  SVGFEBlendElement::getOptionalResult()
  {
    return GETVAL(Result);
  }


  void
  SVGFEBlendElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, SVGElement * elem, svgl::FilterContext* filterContext)
  {
    //const dom::string& in1_ = GET_BASE_VAL(In1);
    //const dom::string& in2_ = GET_BASE_VAL(In2);
    
    //std::cerr << in1_ << std::endl;
    //std::cerr << in2_ << std::endl;

    const SVG_FEBLEND_MODE& mode_ = GETVAL(Mode);
    switch (mode_) {
    case SVG_FEBLEND_MODE_NORMAL:
      break;
      
    case SVG_FEBLEND_MODE_MULTIPLY:
      glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
      std::cerr << "warning: 'multiply' mode for <feblend> not properly implemented " __FILE__":" << __LINE__ << std::endl;
      break;

    case SVG_FEBLEND_MODE_SCREEN:
      glBlendFunc(GL_ONE_MINUS_SRC_COLOR, GL_ONE);
      break;

    case SVG_FEBLEND_MODE_DARKEN:
    case SVG_FEBLEND_MODE_LIGHTEN:
    default:
      //std::cerr << "warning: '" << print_SVGFEBlendElement_SVG_FEBLEND_MODE(mode_) << "' mode for <feblend> not implemented " __FILE__":" << __LINE__ << std::endl;
      break;
    }
  }

  void
  SVGFEBlendElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, SVGElement * elem, svgl::FilterContext* filterContext)
  {
    const SVG_FEBLEND_MODE& mode_ = GETVAL(Mode);
    if(mode_!=SVG_FEBLEND_MODE_NORMAL)
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }



}
