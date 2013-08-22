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
#ifndef __bezier__
#define __bezier__

#include <svgl/config.hpp>
#include <vector>
#include <w3c/svg/point2d.hpp>


namespace Bezier {

  template <class __Coord=float>
  class Path {
  public:
    enum Type { close, move, line, parabolic, cubic, end };
    typedef __Coord Coord;
    typedef point2d<Coord> Vertice;
    typedef std::vector<Vertice> Vertices;
    typedef std::vector<Type> Types;

    class SubPath;

    struct exc {
      exc(const char* msg): _msg(msg) {}
      const char* what() const { return _msg; }
    private:
      const char * _msg;
    };

    typedef exc Exception;

    void move_to(const Coord& x, const Coord& y);
    void line_to(const Coord& x, const Coord& y);
    void parabole_to(const Coord& x1, const Coord& y1,
		   const Coord& x2, const Coord& y2);
    void cubic_to(const Coord& x1, const Coord& y1,
		const Coord& x2, const Coord& y2,
		const Coord& x3, const Coord& y3);
    void close_path();
    void current_point(Coord& x, Coord& y) const;
    void before_current_point(const Coord& x, const Coord& y) const;

    class iterator;
    class const_iterator;

    iterator begin() {}

  };

}

#endif // __bezier__
