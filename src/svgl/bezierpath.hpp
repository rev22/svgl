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

#ifndef _bezierpath_h
#define _bezierpath_h

#include <svgl/config.hpp>
#include <svgl/point2d.hpp>
#include <vector>
#include <iosfwd>
using std::vector;

class BezierPath;
class Transform;
class ostream;

class BezierPath {
public:
  enum Type { close, move, line, parabolic, cubic };
  typedef double Coord;
	//typedef float Coord;
  typedef point2d<Coord> Point;
  typedef vector<Point> vertices;
  typedef vector<Type> types;
  class iterator;
  class const_iterator;

protected:
  vertices vertices_;
  types types_;
  int subpath_;
public:
  BezierPath();
  BezierPath(const BezierPath& bp);
  ~BezierPath();

  BezierPath& operator = (const BezierPath& bp);

  bool operator == (const BezierPath& bp) const;
  bool closed() const;
  bool started() const;
  void reset();
  bool move_to(Coord x, Coord y);
  bool line_to(Coord x, Coord y);
  bool parabole_to(Coord x1, Coord y1,
		   Coord x2, Coord y2);
  bool cubic_to(Coord x1, Coord y1,
		Coord x2, Coord y2,
		Coord x3, Coord y3);
  bool close_path();
  bool current_point(Coord& x, Coord& y) const;
  bool before_current_point(Coord& x, Coord& y) const;

  void reverse();
  void flatten(BezierPath&, Coord flat) const;
  bool empty() const;
  bool bounds(Coord& min_x, Coord& min_y, Coord& max_x, Coord& max_y) const;
  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

  const vertices& dbg_vertices() const { return vertices_; }
  const vector<Type>& dbg_types() const { return types_; }

  class const_iterator {
  public:
    const_iterator() {}
    //const_iterator(const Type * t, const Point * p) : type_(t), control_(p) {}
    const_iterator(types::const_iterator t, vertices::const_iterator p) : type_(&(*t)), control_(&(*p)) {}
    const_iterator(const iterator& i);

    const_iterator& operator *();
    const_iterator& operator ++();
    const_iterator operator ++(int) {
      const_iterator tmp(*this);
      ++(*this);
      return tmp;
    }
    const Point& operator [] (int i) const { return control_[i]; }

    bool operator == (const const_iterator& i) const;
    bool operator != (const const_iterator& i) const {
      return ! operator == (i);
    }

    Type type() const { return *type_; }
    int count() const;
    void flatten(vertices& v, Coord flat) const;
    void flatten_adaptive(vertices& v, Coord flat) const;
    void flatten_adaptive_angular(vertices& v, float sin_angle) const;
    bool closed(const const_iterator&) const;

  protected:
    const Type * type_;
    const Point * control_;
  };

  class iterator {
  public:
    iterator() {}
    //iterator(Type * t, Point * p) : type_(t), control_(p) {}
    iterator(types::iterator t, vertices::iterator p) : type_(&*t), control_(&*p) {}

    iterator& operator *();
    iterator& operator ++();
    Point& operator [] (int i) { return control_[i]; }

    Type type() const { return const_iterator(*this).type(); }
    int count() const { return const_iterator(*this).count(); }
    void flatten(vertices& v, Coord flat) const { const_iterator(*this).flatten(v,flat); }
    bool closed(const const_iterator& e) const { return const_iterator(*this).closed(e); }

  protected:
    friend const_iterator::const_iterator(const iterator&);
    Type * type_;
    Point * control_;
  };


  static int count(Type t);
  static int advance(Type t);
};

inline bool
BezierPath::const_iterator::operator == (const BezierPath::const_iterator& i) const
{
  //  return type_ == i.type_ && control_ == i.control_;
  return type_ == i.type_;
}

inline int BezierPath::const_iterator::count() const
{
  return BezierPath::count(type());
}

inline bool BezierPath::const_iterator::closed(const const_iterator& e) const
{
  const_iterator t = (*this);
  while (t!=e) {
    switch (t.type()) {
    case close: return true;
    case move: return false;
    default: ++t;
    }
  }
  return false;
}


inline BezierPath::BezierPath() { subpath_ = -1; }

inline BezierPath::BezierPath(const BezierPath& bp)
  : vertices_(bp.vertices_),
    types_(bp.types_),
    subpath_(bp.subpath_) { }

inline BezierPath::~BezierPath() { }

inline BezierPath& BezierPath::operator = (const BezierPath& bp)
{
  vertices_ = bp.vertices_;
  types_ = bp.types_;
  subpath_ = bp.subpath_;
  return *this;
}

inline int BezierPath::count(BezierPath::Type t)
{
  switch(t) {
  case move:
    return 1;
  case close:
  case line:
    return 2;
  case parabolic:
    return 3;
  case cubic:
    return 4;
  }
  return 0;
}

inline int BezierPath::advance(BezierPath::Type t)
{
  switch(t) {
  case move:
    return 0;
  case close:
  case line:
    return 1;

  case parabolic:
    return 2;
  case cubic:
    return 3;
  }
  return 0;
}


inline bool
BezierPath::closed() const
{
  //  return subpath_ == -1;
  return types_.size()>0 && types_.back()==close;
}

inline bool
BezierPath::started() const
{
  return subpath_!=-1;
  //  return types_.size()>0 && types_[0]==move;
}

inline void
BezierPath::reset()
{
  types_.erase(types_.begin(), types_.end());
  vertices_.erase(vertices_.begin(), vertices_.end());
  subpath_ = -1;
}

inline bool
BezierPath::move_to(Coord x, Coord y)
{
#if 0
  if (! closed()) {
    return false;
  }
#endif
  subpath_ = vertices_.size();
  vertices_.push_back(Point(x, y));
  types_.push_back(move);
  return true;
}

inline bool
BezierPath::line_to(Coord x, Coord y)
{
  //  if (closed()) {
  if(!started()) {
    return false;
  }
  vertices_.push_back(Point(x, y));
  types_.push_back(line);
  return true;
}

inline bool
BezierPath::parabole_to(Coord x1, Coord y1,
			Coord x2, Coord y2)
{
  //  if (closed()) {
  if(!started()) {
    return false;
  }
  vertices_.push_back(Point(x1, y1));
  vertices_.push_back(Point(x2, y2));
  types_.push_back(parabolic);
  return true;
}

inline bool
BezierPath::cubic_to(Coord x1, Coord y1,
		     Coord x2, Coord y2,
		     Coord x3, Coord y3)
{
  //  if (closed()) {
  if(!started()) {
    return false;
  }
  vertices_.push_back(Point(x1, y1));
  vertices_.push_back(Point(x2, y2));
  vertices_.push_back(Point(x3, y3));
  types_.push_back(cubic);
  return true;
}

inline bool
BezierPath::close_path()
{
  //  if (closed()) {
  if(!started()) {
    return false;
  }
  types_.push_back(close);
  vertices_.push_back(vertices_[subpath_]);
  subpath_ = -1;
  return true;
}

inline bool 
BezierPath::current_point(Coord& x, Coord& y) const
{
  //  if (closed()) {
  /*
  if(!started()) {
    return false;
  }
  */
  if (!vertices_.size()) {
    return false;
  }
  const Point& p = vertices_.back();
  x = p.x();
  y = p.y();
  return true;
}


inline bool 
BezierPath::before_current_point(Coord& x, Coord& y) const
{
  //  if (closed()) {
  if(!started()) {
    return false;
  }

  vertices::const_reverse_iterator i = vertices_.rbegin();
  if(i==vertices_.rend())
    return false;
  ++i;
  if(i==vertices_.rend())
    return false;

  const Point& p = (*i);
  x = p.x();
  y = p.y();
  return true;
}



inline bool
BezierPath::empty() const
{
  return types_.empty();
}


inline BezierPath::iterator& BezierPath::iterator::operator * ()
{
  return *this;
}


inline BezierPath::iterator& BezierPath::iterator::operator ++()
{
  control_ += BezierPath::advance(type());
  ++type_;
  if(*type_==move)
    ++control_;
  return *this;
}

inline BezierPath::const_iterator::const_iterator(const BezierPath::iterator& i)
  : type_(i.type_), control_(i.control_)
{
  /*
  type_ = i.type_;
  control_ = i.control_;
  */
}

inline BezierPath::const_iterator& BezierPath::const_iterator::operator * ()
{
  return *this;
}

//#include <iostream>
inline BezierPath::const_iterator& BezierPath::const_iterator::operator ++()
{
  control_ += BezierPath::advance(type());
  //  std::cerr << "advance " << BezierPath::advance(type()) << std::endl;
  ++type_;
  if(*type_==move)
    ++control_;
  return *this;
}

inline BezierPath::iterator BezierPath::begin()
{
  return iterator(types_.begin(), vertices_.begin());
}

inline BezierPath::const_iterator BezierPath::begin() const
{
  return const_iterator(types_.begin(), vertices_.begin());
}

inline BezierPath::iterator BezierPath::end()
{
  return iterator(types_.end(), vertices_.end());
}

inline BezierPath::const_iterator BezierPath::end() const
{
  return const_iterator(types_.end(), vertices_.end());
}


inline bool BezierPath::operator == (const BezierPath& bp) const
{
  if (this == &bp) return true;
  const_iterator i, j;
  for (i = begin(), j = bp.begin();
       i != end() && j != bp.end();
       ++i, ++j) {
    if (i != j) return false;
  }
  return (i != end()) || (j != bp.end());
}

void add_rectangle(BezierPath&,
		   BezierPath::Coord x, BezierPath::Coord y,
		   BezierPath::Coord w, BezierPath::Coord h);
void add_circle(BezierPath&, 
		BezierPath::Coord x, BezierPath::Coord y,
		BezierPath::Coord r);
void add_ellipse(BezierPath&, 
		 BezierPath::Coord x, BezierPath::Coord y,
		 BezierPath::Coord r1, BezierPath::Coord r2);
std::ostream& operator << (std::ostream&, const BezierPath&);
bool is_rectangle(const BezierPath&);

#endif

