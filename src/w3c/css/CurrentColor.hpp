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
#ifndef __css_currentcolor__
#define __css_currentcolor__

#include <svgl/config.hpp>
#include <w3c/css/RGBColor.hpp>
#include <w3c/css/CSSStyle.hpp>
#include <w3c/dom/string.hpp>

namespace svg {
  class GLInfo;
  class SVGElement;
}


namespace css {
  struct CurrentColor : RGBColor {
    CurrentColor(CSSStyle* s) : _style(s) {}
    virtual ~CurrentColor() {}
    virtual void glPreTraverse(GLInfo*, SVGElement *e);
    virtual void glPostTraverse(GLInfo*, SVGElement *e);
  private:
    CSSStyle * _style;
  };

};

#endif // __css_currentcolor__

