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
#ifndef _point2d_h
#define _point2d_h

#include <svgl/point.hpp>
#include <cmath>

template <class Coord>
struct point2d : public point_<2, Coord> {
  point2d() { }
  point2d(Coord x, Coord y) { p[x_dimension] = x; p[y_dimension] = y; }
  point2d(const point2d& pe) : point_<2, Coord>(pe) { }
  point2d(const point_<2, Coord>& pe) : point_<2, Coord>(pe) { }

  Coord x() const { return p[x_dimension]; }
  Coord& x() { return p[x_dimension]; }
  Coord y() const { return p[y_dimension]; }
  Coord& y() { return p[y_dimension]; }
};

template <class Coord>
struct  vector2d : public vector_<2, Coord> {
  vector2d() { }
  vector2d(Coord dx, Coord dy) { v[x_dimension] = dx; v[y_dimension] = dy; }
  vector2d(const vector2d& ve) : vector_<2, Coord>(ve) { }
  vector2d(const vector_<2,Coord>& ve) : vector_<2, Coord>(ve) { }

  Coord dx() const { return v[x_dimension]; }
  Coord& dx() { return v[x_dimension]; }
  Coord dy() const { return v[y_dimension]; }
  Coord& dy() { return v[y_dimension]; }

};

template <class Coord>
inline double norm(const vector2d<Coord>& ve)
{
  return ::hypot(ve.v[x_dimension], ve.v[y_dimension]);
}


template <class Coord>
inline Coord cross(const vector2d<Coord>& v1, const vector2d<Coord>& v2)
{
  return v1.dx() * v2.dy() - v1.dy() * v2.dx();
}

template <class Coord>
inline Coord dot(const vector2d<Coord>& v1, const vector2d<Coord>& v2)
{
  return v1.dx() * v2.dx() + v1.dy() * v2.dy();
}

template <class Coord>
inline ostream&
operator << (ostream& out, const point2d<Coord> p)
{
  return
    out << "point<2>::(" << p[x_dimension] << "," << p[y_dimension] << ")";
}

template <class Coord>
inline istream&
operator >> (istream& in, point2d<Coord>& p)
{
  in.ignore(sizeof("point<2>::(")-1);
  in >> p[x_dimension];
  in.ignore(1);
  in >> p[y_dimension];
  return in;
}

template <class Coord>
inline ostream&
operator << (ostream& out, const vector2d<Coord>& v)
{
  return
    out << "vector<2>::(" << v[x_dimension] << "," << v[y_dimension] << ")";
}

template <class Coord>
inline istream&
operator >> (istream& in, vector2d<Coord>& v)
{
  in.ignore(sizeof("vector<2>::(")-1);
  in >> v[x_dimension];
  in.ignore(1);
  in >> v[y_dimension];
  return in;
}


#endif
