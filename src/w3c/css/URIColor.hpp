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
#ifndef __css_uricolor__
#define __css_uricolor__

#include <svgl/config.hpp>
#include <w3c/css/RGBColor.hpp>
#include <w3c/dom/string.hpp>

namespace svgl {
  class Context;
  class GLInfo;
}

namespace svg {
  class SVGElement;
  class SVGStylable;
}


namespace css {
  struct URIColor : RGBColor {
    dom::string uri;
    virtual ~URIColor() {}
    virtual void glPreTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGElement *e, float x, float y, float width, float height) const;
    virtual void glPostTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGElement *e, float x, float y, float width, float height) const;
    virtual void glPreTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGStylable *e, float sw=0) const;
    virtual void glPostTraverse(svgl::Context*, svgl::GLInfo*, svg::SVGStylable *e, float sw=0) const;
  };

};

#endif // __css_uricolor__

