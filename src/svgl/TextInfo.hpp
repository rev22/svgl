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
#ifndef __TextInfo__
#define __TextInfo__

#include <svgl/config.hpp>
#include <vector>

namespace svg {
  class SVGTextPathElement;
}

namespace svgl {

  class BezierPathDistanceIterator;

  class TextInfo {
  public:

    TextInfo() : distanceIterator(0), textPathElement(0), x(0), y(0), dx(0), dy(0) {};

    BezierPathDistanceIterator* distanceIterator;
    svg::SVGTextPathElement* textPathElement;
    typedef float coordinate;
    coordinate x,y; // position of the base line (can be a path)
    coordinate dx,dy; // position relative to the base line
  };

}

#endif // __TextInfo__
