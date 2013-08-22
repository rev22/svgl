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
#include <w3c/css/RGBColor.hpp>
#include <w3c/css/URIColor.hpp>
#include <w3c/css/CSSStyleDeclaration.hpp>
#include <w3c/svg/SVGGradientElement.hpp>
//#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/dom/Document.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/ExternalEntityManager.hpp>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

namespace svg {
  extern css::RGBColor * currentColor;
  extern css::RGBColor * inheritColor;
}


namespace css {

  void
  Color::initNoneColor()
  {
    _noneColor = new NoneColor();
  }

  NoneColor::NoneColor() {}
  NoneColor* Color::_noneColor;


  void
  RGBColor::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svg::SVGElement *e, float x, float y, float width, float height) const
  {
      glColor4f(r, g, b, glinfo->alpha);
      //std::cerr << DBGVAR(this) << DBGVAR(r) << DBGVAR(g) << DBGVAR(b) << __FL__;
  }

  void
  RGBColor::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo*, svg::SVGElement *e, float x, float y, float width, float height) const
  {
  }

  void
  RGBColor::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svg::SVGStylable* e, float sw) const
  {
      glColor4f(r, g, b, glinfo->alpha);
      //std::cerr << DBGVAR(this) << DBGVAR(r) << DBGVAR(g) << DBGVAR(b) << __FL__;
  }

  void
  RGBColor::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo*, svg::SVGStylable* e, float sw) const
  {
  }

  void
  URIColor::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svg::SVGElement *e, float x, float y, float width, float height) const
  {
    if (uri->getLength()) {
      dom::Element* de = svglContext->externalEntityManager->get(uri, e->getOwnerDocument());
      //dom::Element* de = e->GET_SIMPLE_VAL(OwnerSVGElement)->getElementById(uri->substring(1));
      svg::SVGGradientElement * grad = dynamic_cast<svg::SVGGradientElement*>(de);
      if(grad) {
	//std::cerr << DBGVAR(this) << "linear gradient " << uri << " " << grad << __FL__;
	grad->glPreTraverse(svglContext, glinfo);
      } else {
	std::cerr << "couldn't find URI color " << uri << " " << __FL__;
      }

    }
  }

  void
  URIColor::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svg::SVGElement *e, float x, float y, float width, float height) const
  {
    if (uri->getLength()>1) {
      dom::Element* de = svglContext->externalEntityManager->get(uri, e->getOwnerDocument());
      //dom::Element* de = e->GET_SIMPLE_VAL(OwnerSVGElement)->getElementById(uri->substring(1));
      svg::SVGGradientElement * grad = dynamic_cast<svg::SVGGradientElement*>(de);
      if(grad) {
	//std::cout << DBGVAR(x) << DBGVAR(y) << DBGVAR(width) << DBGVAR(height) << __FL__;
	grad->glPostTraverse(svglContext, glinfo, x, y, width, height);
      } else {
	std::cerr << "couldn't find URI color" << uri << " " << __FL__;
      }
    }

  }

  void
  URIColor::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svg::SVGStylable* s, float sw) const
  {
    svg::SVGElement * e = dynamic_cast<svg::SVGElement*>(s);
    if (uri->getLength()>1 && e) {
      dom::Element* de = svglContext->externalEntityManager->get(uri, e->getOwnerDocument());
      //dom::Document * doc = svglContext->svgOwner->getOwnerDocument();
      //dom::Element * de = doc->getElementById(uri->substring(1));
      //std::cerr << DBGVAR(doc) << __FL__;
      svg::SVGGradientElement * grad = dynamic_cast<svg::SVGGradientElement*>(de);
      if(grad) {
	//std::cerr << DBGVAR(this) << "linear gradient " << uri << " " << grad << __FL__;
	grad->glPreTraverse(svglContext, glinfo);
      } else {
	std::cerr << "couldn't find URI color " << uri << " " << __FL__;
      }
    }
  }

  void
  URIColor::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svg::SVGStylable *s, float sw) const
  {
    svg::SVGElement * e = dynamic_cast<svg::SVGElement*>(s);
    if (uri->getLength()>1) {
      dom::Element* de = svglContext->externalEntityManager->get(uri, e->getOwnerDocument());
      // dom::Element* de = e->getOwnerSVGElement().getValue()->getElementById(uri->substring(1));
      //dom::Element* de = svglContext->svgOwner->getElementById(uri->substring(1));
      svg::SVGGradientElement * grad = dynamic_cast<svg::SVGGradientElement*>(de);
      if(grad) {
	float x, y, width, height;
	s->getBoundingBox(svglContext, glinfo, &x,&y,&width,&height);
	//std::cout << DBGVAR(x) << DBGVAR(y) << DBGVAR(width) << DBGVAR(height) << __FL__;
	grad->glPostTraverse(svglContext, glinfo, x-sw/2., y-sw/2., width+sw, height+sw);
      } else {
	std::cerr << "couldn't find URI color " << uri << " " << __FL__;
      }

    }

  }


  void
  NoneColor::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svg::SVGElement *e, float x, float y, float width, float height) const
  {
    STDDBG;
  }

  void
  NoneColor::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo*, svg::SVGElement *e, float x, float y, float width, float height) const
  {
    STDDBG;
  }

  void
  NoneColor::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svg::SVGStylable* e, float sw) const
  {
    STDDBG;
  }

  void
  NoneColor::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo*, svg::SVGStylable* e, float sw) const
  {
    STDDBG;
  }


}

