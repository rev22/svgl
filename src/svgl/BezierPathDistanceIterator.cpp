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
#include <svgl/BezierPathDistanceIterator.hpp>

#include <svgl/debug.hpp>


namespace svgl {

  BezierPathDistanceIterator::BezierPathDistanceIterator(const BezierPath& bezier, float precision)
    : _orientation(0), _lastdistance(0)
  {
    bezier.flatten(_flatten, precision);
    //bezier.flatten(_flatten, 200);
    _end = _flatten.end();
    reset();
  }

  void
  BezierPathDistanceIterator::reset()
  {
    _orientation=0;
    _lastdistance=0;
    _iter=_flatten.begin();

    if(_iter!=_end) {
      const BezierPath::Point& p0 = (*_iter)[0];
      x=p0.x();
      y=p0.y();
    }

    dx=0;
    dy=0;
  }

  void
  BezierPathDistanceIterator::advance(float distance)
  {

    BezierPath::Coord xsav=x, ysav=y;

    distance += _lastdistance;

    while(_iter!=_end && distance>0) {
      BezierPath::const_iterator& i = _iter;
      switch ( (*i).type() ) {

      case BezierPath::move: {
	const BezierPath::Point& p0 = (*i)[0];
	x=p0.x();
	y=p0.y();
	++_iter;
      }
      break;

      case BezierPath::line: {
	const BezierPath::Point& p0 = (*i)[0];
	const BezierPath::Point& p1 = (*i)[1];
	BezierPath::Coord dx = p1.x()-p0.x();
	BezierPath::Coord dy = p1.y()-p0.y();

	BezierPath::Coord l = ::hypot(dx, dy);

	if(l!=0.0) {
	  //std::cerr << "-> " << DBGVAR(distance) << DBGVAR(l) << DBGVAR(_lastdistance) << std::endl;	  
	  //std::cerr << DBGVAR(x) << DBGVAR(y) << std::endl;
	  float actualDistance;
	  if( distance>l ) {
	    actualDistance = (l - _lastdistance);
	    _lastdistance=0;
	    ++_iter;
	  }
	  else {
	    actualDistance = (distance - _lastdistance);
	    _lastdistance = distance;
	  }
	  float ratio = actualDistance/l;
	  dx *= ratio;
	  dy *= ratio;
	  x += dx;
	  y += dy;
	  distance -= l;
	  //std::cerr << "<- " << DBGVAR(distance) << DBGVAR(l) << DBGVAR(_lastdistance) << std::endl;
	  //std::cerr << DBGVAR(x) << DBGVAR(y) << std::endl;
	}
	else {
	  _lastdistance=0;
	  ++_iter;
	}
      }
      break;
      default:
	break;
      }
    }

    dx = x-xsav;
    dy = y-ysav;

    //std::cerr << "<- " << DBGVAR(x) << DBGVAR(y) << DBGVAR(dx) << DBGVAR(dy) << std::endl;

    BezierPath::Coord seglen = ::hypot(dx, dy);
    if(seglen) {

      float cos_orient = dx/seglen;
      float sin_orient = dy/seglen;

      _orientation = acos(cos_orient)*180/M_PI;
      //std::cerr << DBGVAR(_orientation) << std::endl;
      sin_orient=-sin_orient; // don't know why dy is reverse sign
      if(sin_orient<0) {
	if(cos_orient>0)
	  _orientation=-_orientation;
	else
	  _orientation = -_orientation;
      }
    }
    //std::cerr << DBGVAR(cos_orient) << DBGVAR(sin_orient) << DBGVAR(_orientation) << std::endl;

  }


  float
  BezierPathDistanceIterator::getOrientation() const
  {
    return _orientation;
  }

  bool
  BezierPathDistanceIterator::finished() const
  {
    return _iter==_end;
  }


}
