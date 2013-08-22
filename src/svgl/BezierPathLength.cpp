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
#include <svgl/BezierPathLength.hpp>

namespace svgl {

  float
  length(const BezierPath& bezier)
  {
    BezierPath flatten;
    bezier.flatten(flatten, 2.);
    float distance=0;
    for(BezierPath::const_iterator i = flatten.begin(); i!=flatten.end(); ++i) {
      switch ( (*i).type() ) {
      case BezierPath::line: {
	const BezierPath::Point& p0 = (*i)[0];
	const BezierPath::Point& p1 = (*i)[1];
	BezierPath::Coord dx = p1.x()-p0.x();
	BezierPath::Coord dy = p1.y()-p0.y();

	// std::cerr << DBGVAR(dx) << DBGVAR(dy) << std::endl;
	BezierPath::Coord l = ::hypot(dx, dy);
	distance+=l;
      }
      break;
      default:
	break;
      }
    }
    return distance;
  }


}
