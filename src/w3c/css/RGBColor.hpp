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
#ifndef __css_RGBColor__
#define __css_RGBColor__

#include <svgl/config.hpp>
namespace svgl {
  class Context;
  class GLInfo;
}

namespace svg {
  class SVGElement;
  class SVGStylable;
}

namespace css {

  class NoneColor;

  class Color {
  public:
    //float r,g,b;
    virtual ~Color() {}
    virtual void glPreTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGElement *e, float x, float y, float width, float height) const = 0;
    virtual void glPostTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGElement *e, float x, float y, float width, float height) const = 0;
    virtual void glPreTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGStylable *e, float sw=0) const = 0;
    virtual void glPostTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGStylable *e, float sw=0) const = 0;

    static NoneColor* getNoneColor() { return _noneColor; }
    static void initNoneColor();
  protected:
    static NoneColor * _noneColor;
  };

  struct RGBColor : Color {
    float r,g,b;
    virtual void glPreTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGElement *e, float x, float y, float width, float height) const;
    virtual void glPostTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGElement *e, float x, float y, float width, float height) const;
    virtual void glPreTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGStylable *e, float sw=0) const;
    virtual void glPostTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGStylable *e, float sw=0) const;
  };

  struct NoneColor : Color {
    virtual void glPreTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGElement *e, float x, float y, float width, float height) const;
    virtual void glPostTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGElement *e, float x, float y, float width, float height) const;
    virtual void glPreTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGStylable *e, float sw=0) const;
    virtual void glPostTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGStylable *e, float sw=0) const;
  protected:
    NoneColor();
    friend void Color::initNoneColor();
  };

}

//typedef css::RGBColor RGBColor;

#endif // __css_RGBColor__
