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
#ifndef __BezierPathDistanceIterator__
#define __BezierPathDistanceIterator__

#include <svgl/config.hpp>
#include <svgl/bezierpath.hpp>

namespace svgl {

  class BezierPathDistanceIterator {
  public:
    BezierPathDistanceIterator(const BezierPath& bezier, float precision=2.);
    void advance(float distance);
    float getOrientation() const;
    void reset();
    bool finished() const;
    BezierPath::Coord dx,dy,x,y;
  private:
    BezierPath _flatten;
    float _orientation;
    BezierPath::const_iterator _iter, _end;
    float _lastdistance;
  };


}

#endif // __BezierPathDistanceIterator__

