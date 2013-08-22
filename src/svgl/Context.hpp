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
#ifndef __svgl_Context__
#define __svgl_Context__

#include <svgl/config.hpp>
#include <math.h>
#include <w3c/svg/SVGLength.hpp>

namespace glft {
  class FontManager;
}

namespace svg {
  class SVGSVGElement;
}

namespace svgl {

  class ImageManager;
  class ExternalEntityManager;

  class Context {
  public:
    //static const double invsqr2 = .70710678118654752440;
#define invsqr2 (.70710678118654752440)

    float dpi;
    float viewport_width, viewport_height;
    float viewport_hypot;
    float fontSize;
    float fontXHeight;
    glft::FontManager * fontManager;
    ImageManager * imageManager;
    ExternalEntityManager * externalEntityManager;
    svg::SVGSVGElement* svgOwner;

    Context() : dpi(96), fontSize(12), fontXHeight(12) {}

    float computeWidthLength(svg::SVGLength& l) { return l.computeValue(dpi, viewport_width, fontSize, fontXHeight); }
    float computeHeightLength(svg::SVGLength& l) { return l.computeValue(dpi, viewport_height, fontSize, fontXHeight); }
    float computeHypotLength(svg::SVGLength& l) { return l.computeValue(dpi, viewport_hypot, fontSize, fontXHeight); }

    void
    setViewportWidthHeight(float width, float height)
    {
      viewport_width = width;
      viewport_height = height;
      viewport_hypot = ::hypot(width,height)*invsqr2;
    }

    
  };

}

#endif // __svgl_Context__

