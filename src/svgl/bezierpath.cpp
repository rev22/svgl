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
#include <svgl/bezierpath.hpp>
#include <svgl/point2d.hpp>
#include <assert.h>
#include <cmath>
#include <iostream>

//#define FLATTEN_COUNT
#ifdef FLATTEN_COUNT
static int flatten_count;
#endif

template <class T>
inline T abs(const T& a) { return a < 0 ? -a : a; }

template <class T>
inline T sqr(const T& a) { return a*a; }

static bool
find_subpath_end(vector<BezierPath::Type>& types,
		 unsigned int& element, unsigned int& vertex)
{
  bool started = false;

  while (element < types.size()) {
    switch(types[element++]) {
    case BezierPath::move:
      if (started) {	// move of a new subpath
	return true;
      }
      started = true;
      vertex++;
      break;
    case BezierPath::cubic:
      vertex++;
      // fall through
    case BezierPath::parabolic:
      vertex++;
      // fall through
    case BezierPath::line:
      vertex++;
      break;
    case BezierPath::close:
      vertex++;
      return true;
    }
  }
  return started;
}

static void
reverse_subpath(vector<BezierPath::Type>& types,
		vector<BezierPath::Point>& vertices,
		unsigned int from_element, unsigned int from_vertex,
		unsigned int to_element, unsigned int to_vertex)
{
  vector<BezierPath::Type> new_types(to_element - from_element);
  vector<BezierPath::Point> new_vertices(to_vertex - from_vertex);
  bool was_closed = false;

  new_types.push_back(BezierPath::move);
  new_vertices.push_back(vertices[to_vertex-1]);
  for (unsigned int element = to_element - 1, vertex = to_vertex - 2;
       element >= from_element;
       element--) {
    BezierPath::Type t = types[element];

    switch (t) {
    case BezierPath::move:
      if (was_closed) {
	new_types.push_back(BezierPath::close);
      }
      break;
    case BezierPath::cubic:
      new_vertices.push_back(vertices[vertex]);
      vertex--;
      // fall through
    case BezierPath::parabolic:
      new_vertices.push_back(vertices[vertex]);
      vertex--;
      // fall through
    case BezierPath::line:
      new_vertices.push_back(vertices[vertex]);
      vertex--;
      new_types.push_back(t);
      break;
    case BezierPath::close:
      was_closed = true;
      break;
    }
  }
  assert(new_types.size() == (to_element - from_element));
  assert(new_vertices.size() == (to_vertex - from_vertex));
  for (unsigned int i = 0; i < new_types.size(); i++) {
    types[from_element + i] = new_types[i];
  }
  for (unsigned int j = 0; j < new_vertices.size(); j++) {
    vertices[from_vertex + j] = new_vertices[j];
  }
}

void
BezierPath::reverse() 
{
  unsigned int start_element = 0, start_vertex = 0;
  unsigned int end_element = 0, end_vertex = 0;

  while (find_subpath_end(types_, end_element, end_vertex)) {
    reverse_subpath(types_, vertices_, 
		    start_element, start_vertex,
		    end_element, end_vertex);
    start_element = end_element;
    start_vertex = end_vertex;
  }
}

void BezierPath::flatten(BezierPath& p, Coord flat) const
{
  p.reset();
  vertices v;

  for (const_iterator i = begin(); i != end(); ++i) {
    switch(i.type()) {
    case move:
      p.move_to(i[0].x(), i[0].y());
      break;
    case line:
      p.line_to(i[1].x(), i[1].y());
      break;
    case close:
      p.close_path();
      break;
    case parabolic:
    case cubic:
      {
	v.erase(v.begin(), v.end());
	i.flatten(v, flat);
	//i.flatten_adaptive(v, flat);
	vertices::iterator j = v.begin();
	j++;
	for (; j != v.end(); j++) {
	  p.line_to(j[0].x(), j[0].y());
	}
      }
    }
  }
#ifdef FLATTEN_COUNT
  std::cerr << "Flatten count: " << flatten_count << std::endl;
#endif
}

bool
BezierPath::bounds(Coord& min_x, Coord& min_y,
		   Coord& max_x, Coord& max_y) const
{
  if (vertices_.empty()) {
    return false;
  }
  vertices::const_iterator i = vertices_.begin();
  min_x = max_x = i[0].x();
  min_y = max_y = i[0].y();
  i++;
  while(i != vertices_.end()) {
    const Point& p = i[0];
    if (p.x() < min_x)
      min_x = p.x();
    else if (p.x() > max_x)
      max_x = p.x();
    if (p.y() < min_y)
      min_y = p.y();
    else if (p.y() > max_y)
      max_y = p.y();
    i++;
  }
  return true;
}

static inline unsigned int log4(long d)
{
  int i;
  for (i = 0; d > 0; i++) d >>= 2;
  return i;
}

static inline double __myabs(double d) { return (d < 0) ? -d : d; }
static inline float __myabs(float d) { return (d < 0) ? -d : d; }

static unsigned int compute_subdivisions(const BezierPath::Point& p0, 
					 const BezierPath::Point& p1,
					 const BezierPath::Point& p2,
					 const BezierPath::Point& p3,
					 BezierPath::Coord flat)
{
  vector2d<BezierPath::Coord> d01(p1.x() - p0.x(), p1.y() - p0.y());
  vector2d<BezierPath::Coord> d12(p2.x() - p1.x(), p2.y() - p1.y());
  vector2d<BezierPath::Coord> d23(p3.x() - p2.x(), p3.y() - p2.y());
  vector2d<BezierPath::Coord> d03(p3.x() - p0.x(), p3.y() - p0.y());
  vector2d<BezierPath::Coord> d01_12 = d12 - d01;
  vector2d<BezierPath::Coord> d12_23 = d23 - d12;
  double dist = norm(d03);
  double dist2;

  if (dist != 0) {
    dist2 = __myabs(cross(d01_12, d03) / dist);
    double tmp = __myabs(cross(d12_23, d03) / dist);
    if (tmp > dist2) {
      dist2 = tmp;
    }
  } else {
    vector2d<BezierPath::Coord> d0(p0.x(), p0.y());
    dist2 = norm(vector2d<BezierPath::Coord>(d01_12 - d0));
    double tmp = norm(vector2d<BezierPath::Coord>(d12_23 - d0));
    if (tmp > dist2) {
      dist2 = tmp;
    }
  }
  unsigned int subdivisions = 0;
  if (dist2 > 0) {
    subdivisions = log4(long((3.0 / 4.0) * dist2 / flat + 0.5));
  }
  return subdivisions;
}

static void dda_init(const BezierPath::Point& p0,
		     const BezierPath::Point& p1,
		     const BezierPath::Point& p2,
		     const BezierPath::Point& p3,
		     int subdivisions,
										 vector2d<BezierPath::Coord>& dd1,
										 vector2d<BezierPath::Coord>& dd2,
										 vector2d<BezierPath::Coord>& dd3)
{
  vector2d<BezierPath::Coord> d0(p0.x(), p0.y());
  vector2d<BezierPath::Coord> d1(p1.x(), p1.y());
  vector2d<BezierPath::Coord> d2(p2.x(), p2.y());
  vector2d<BezierPath::Coord> d3(p3.x(), p3.y());
  vector2d<BezierPath::Coord> c = (d1 - d0) * 3;
  vector2d<BezierPath::Coord> b = (d2 - d1) * 3 - c;
  vector2d<BezierPath::Coord> a = d3 - b - c - d0;
  double delta = 1.0 / (1 << subdivisions);
  double delta_2 = delta * delta;
  double delta_3 = delta_2 * delta;
  a *= delta_3;
  b *= delta_2;
  c *= delta;
  dd1 = a + b + c; 
  dd2 = a * 6 + b * 2;
  dd3 = a * 6;
}

void BezierPath::const_iterator::flatten(BezierPath::vertices& v,
					 Coord flat) const
{
  unsigned int subdivisions;
  vector2d<BezierPath::Coord> d1, d2, d3;

  v.push_back(control_[0]);
  switch(type()) {
  case move:
    return;
  case line:
  case close:
    v.push_back(control_[1]);
    return;
  case parabolic:{
    BezierPath::Point p1,p2;
    p1.x() = (2*control_[1].x() + control_[0].x())/3.;
    p1.y() = (2*control_[1].y() + control_[0].y())/3.;

    p2.x() = (2*control_[1].x() + control_[2].x())/3.;
    p2.y() = (2*control_[1].y() + control_[2].y())/3.;

    subdivisions = compute_subdivisions(control_[0], p1,
					p2, control_[2],
					flat);
    dda_init(control_[0], p1, p2, control_[2],
	     subdivisions, d1, d2, d3);

    /*
    subdivisions = compute_subdivisions(control_[0], control_[1],
					control_[1], control_[2],
					flat);
    dda_init(control_[0], control_[1], control_[1], control_[2],
	     subdivisions, d1, d2, d3);
    */
  }
    break;
  case cubic:
    subdivisions = compute_subdivisions(control_[0], control_[1],
					control_[2], control_[3],
					flat);
    dda_init(control_[0], control_[1], control_[2], control_[3],
	     subdivisions, d1, d2, d3);
        break;
  default:
    assert(type() == line ||
	   type() == move ||
	   type() == parabolic ||
	   type() == cubic);
    return;
  }
  int size = 1 << subdivisions;
  v.reserve(v.size()+size);
  point2d<BezierPath::Coord> c(control_[0].x(), control_[0][y_dimension]);
  Point p;

  for (int i = 0; i < (size-1); i++) {
    c += d1;
    p.x() = Coord(c.x());
    p.y() = Coord(c.y());
    v.push_back(p);
#ifdef FLATTEN_COUNT
    ++flatten_count;
#endif
    d1 += d2;
    d2 += d3;
  }
  v.push_back(control_[count()-1]);
}

static inline double dist2(const BezierPath::Point& p0, 
			   const BezierPath::Point& p1,
			   const BezierPath::Point& p)
{
  vector2d<BezierPath::Coord> d(p1-p0);
  vector2d<BezierPath::Coord> d1(p-p0);
  return sqr(cross(d,d1));
}

static inline bool flat_enough(const BezierPath::Point& p0, 
			       const BezierPath::Point& p1,
			       const BezierPath::Point& p2,
			       const BezierPath::Point& p3,
			       BezierPath::Coord flat2)
{
  static const BezierPath::Coord epsilon = 0.001;
  vector2d<BezierPath::Coord> d(p3-p0);
  vector2d<BezierPath::Coord> d0(p1-p0);
  vector2d<BezierPath::Coord> d1(p3-p2);
  double n = norm(d);
  if (n < epsilon) {
    return
      sqr(norm(d0)) < flat2 &&
      sqr(norm(d1)) < flat2;
  }
  if (dist2(p0,p3,p1) > flat2)
    return false;
  if (dist2(p0,p3,p2) > flat2)
    return false;
  return true;
}

static inline BezierPath::Point mid(const BezierPath::Point& p0, 
				    const BezierPath::Point& p1)
{
  return BezierPath::Point((p0.x()+p1.x())/2,
			   (p0.y()+p1.y())/2);
}

static void subdivide(BezierPath::vertices& v,
		      const BezierPath::Point& p0, 
		      const BezierPath::Point& p1,
		      const BezierPath::Point& p2,
		      const BezierPath::Point& p3,
		      BezierPath::Coord flat2)
{
  if (flat_enough(p0, p1, p2, p3, flat2)) {
#ifdef FLATTEN_COUNT
    ++flatten_count;
#endif
    v.push_back(p3);
    return;
  }
  const BezierPath::Point p01(mid(p0,p1));
  const BezierPath::Point p12(mid(p1,p2));
  const BezierPath::Point p23(mid(p2,p3));
  const BezierPath::Point p01_12(mid(p01,p12));
  const BezierPath::Point p12_23(mid(p12,p23));
  const BezierPath::Point p_mid(mid(p01_12,p12_23));
  subdivide(v,p0,p01,p01_12,p_mid, flat2);
  subdivide(v,p_mid,p12_23,p23,p3,flat2);
}

void BezierPath::const_iterator::flatten_adaptive(BezierPath::vertices& v,
						  Coord flat) const
{
  unsigned int subdivisions;
  //vector2d<double> d1, d2, d3;

  v.push_back(control_[0]);
  switch(type()) {
  case move:
    return;
  case line:
  case close:
    v.push_back(control_[1]);
    return;
  case parabolic:{
    BezierPath::Point p1,p2;
    p1.x() = (2*control_[1].x() + control_[0].x())/3.;
    p1.y() = (2*control_[1].y() + control_[0].y())/3.;

    p2.x() = (2*control_[1].x() + control_[2].x())/3.;
    p2.y() = (2*control_[1].y() + control_[2].y())/3.;

    subdivisions = compute_subdivisions(control_[0], p1,
					p2, control_[2],
					flat);
    v.reserve(v.size()+(1 << subdivisions));
    subdivide(v, 
	      control_[0], p1,
	      p2, control_[2],
	      flat*flat);
    return;
  }
  case cubic:
    subdivisions = compute_subdivisions(control_[0], control_[1],
					control_[2], control_[3],
					flat);
    v.reserve(v.size()+(1 << subdivisions));
    subdivide(v, 
	      control_[0], control_[1],
	      control_[2], control_[3],
	      flat*flat);
    return;
  default:
    assert(type() == line ||
	   type() == move ||
	   type() == parabolic ||
	   type() == cubic);
    return;
  }
}

static inline bool flat_enough_angular(const BezierPath::Point& p0, 
			       const BezierPath::Point& p1,
			       const BezierPath::Point& p2,
			       const BezierPath::Point& p3,
			       float proj)
{
  vector2d<BezierPath::Coord> d0(p1-p0);
  vector2d<BezierPath::Coord> d1(p3-p2);
  vector2d<BezierPath::Coord> d(p3-p0);
  d /= norm(d);
  d0 /= norm(d0);
  if (abs(dot(d,d0)) > proj)
    return false;
  d1 /= norm(d1);
  if (abs(dot(d,d1)) > proj)
    return false;
  return true;
}
static void subdivide_angular(BezierPath::vertices& v,
			      const BezierPath::Point& p0, 
			      const BezierPath::Point& p1,
			      const BezierPath::Point& p2,
			      const BezierPath::Point& p3,
			      float proj)
{
  if (flat_enough_angular(p0, p1, p2, p3, proj)) {
#ifdef FLATTEN_COUNT
    ++flatten_count;
#endif
    v.push_back(p3);
    return;
  }
  const BezierPath::Point p01(mid(p0,p1));
  const BezierPath::Point p12(mid(p1,p2));
  const BezierPath::Point p23(mid(p2,p3));
  const BezierPath::Point p01_12(mid(p01,p12));
  const BezierPath::Point p12_23(mid(p12,p23));
  const BezierPath::Point p_mid(mid(p01_12,p12_23));
  subdivide(v,p0,p01,p01_12,p_mid, proj);
  subdivide(v,p_mid,p12_23,p23,p3,proj);
}

void
BezierPath::const_iterator::flatten_adaptive_angular(BezierPath::vertices& v,
						     float sin_angle) const
{
  //vector2d<BezierPath::Coord> d1, d2, d3;

  v.push_back(control_[0]);
  switch(type()) {
  case move:
    return;
  case line:
  case close:
    v.push_back(control_[1]);
    return;
  case parabolic:{
    BezierPath::Point p1,p2;
    p1.x() = (2*control_[1].x() + control_[0].x())/3.;
    p1.y() = (2*control_[1].y() + control_[0].y())/3.;

    p2.x() = (2*control_[1].x() + control_[2].x())/3.;
    p2.y() = (2*control_[1].y() + control_[2].y())/3.;

    subdivide_angular(v, 
		      control_[0], p1,
		      p2, control_[2],
		      abs(sin_angle));
    return;
  }
  case cubic:
    subdivide_angular(v, 
		      control_[0], control_[1],
		      control_[2], control_[3],
		      abs(sin_angle));
    return;
  default:
    assert(type() == line ||
	   type() == move ||
	   type() == parabolic ||
	   type() == cubic);
    return;
  }
}
    
static const BezierPath::Coord magic = 0.5522847498307934; // 4/3 * (sqrt(2) - 1)

void add_circle(BezierPath& p,
		BezierPath::Coord x, BezierPath::Coord y,
		BezierPath::Coord r)
{
    BezierPath::Coord r0 = magic * r;
    
    p.move_to(x + r, y);
    p.cubic_to(x + r , y - r0,
	       x + r0, y - r,
	       x     , y - r);
    p.cubic_to(x - r0, y - r ,
	       x - r , y - r0,
	       x - r , y);
    p.cubic_to(x - r , y + r0,
	       x - r0, y + r,
	       x     , y + r);
    p.cubic_to(x + r0, y + r ,
	       x + r , y + r0,
	       x + r , y);
    p.close_path();
}

static const BezierPath::Coord p0 = 1.00000000;
static const BezierPath::Coord p1 = 0.89657547;   // cos 30 * sqrt(1 + tan 15 * tan 15)
static const BezierPath::Coord p2 = 0.70710678;   // cos 45 
static const BezierPath::Coord p3 = 0.51763809;   // cos 60 * sqrt(1 + tan 15 * tan 15)
static const BezierPath::Coord p4 = 0.26794919;   // tan 15

void add_ellipse(BezierPath& p,
		 BezierPath::Coord x, BezierPath::Coord y,
		 BezierPath::Coord r1, BezierPath::Coord r2)
{
  BezierPath::Coord px0 = p0 * r1, py0 = p0 * r2;
  BezierPath::Coord px1 = p1 * r1, py1 = p1 * r2;
  BezierPath::Coord px2 = p2 * r1, py2 = p2 * r2;
  BezierPath::Coord px3 = p3 * r1, py3 = p3 * r2;
  BezierPath::Coord px4 = p4 * r1, py4 = p4 * r2;

  p.move_to(x + r1, y);
  p.cubic_to(x + px1, y + py3, x + px2, y + py2, x + px0, y + py4);
  p.cubic_to(x + px4, y + py0, x, y + r2, x + px3, y + py1);
  p.cubic_to(x - px3, y + py1, x - px2, y + py2, x - px4, y + py0);
  p.cubic_to(x - px0, y + py4, x - r1, y, x - px1, y + py3);
  p.cubic_to(x - px1, y - py3, x - px2, y - py2, x - px0, y - py4);
  p.cubic_to(x - px4, y - py0, x, y - r2, x - px3, y - py1);
  p.cubic_to(x + px3, y - py1, x + px2, y - py2, x + px4, y - py0);
  p.cubic_to(x + px0, y - py4, x + r1, y, x + px1, y - py3);
  p.close_path();
}

void add_rectangle(BezierPath& p,
		   BezierPath::Coord x, BezierPath::Coord y,
		   BezierPath::Coord w, BezierPath::Coord h)
{
  p.move_to(x, y);
  p.line_to(x+w, y);
  p.line_to(x+w, y+h);
  p.line_to(x, y+h);
  p.close_path();
}

bool is_rectangle(const BezierPath& bp)
{
  BezierPath::const_iterator i = bp.begin();
  const BezierPath::const_iterator& end = bp.end();

  if (i == end || i.type() != BezierPath::move)
    return false;
  ++i;
  if (i == end || i.type() != BezierPath::line)
    return false;
  ++i;
  if (i == end || i.type() != BezierPath::line)
    return false;
  ++i;
  if (i == end || i.type() != BezierPath::line)
    return false;
  ++i;
  if (i == end || i.type() != BezierPath::line)
    return false;
  ++i;
  if (i == end || i.type() != BezierPath::close)
    return false;
  i = bp.begin();
  if (i[0].x() == i[1].x()) {
    return
      i[2].x() == i[3].x() &&
      i[1].y() == i[2].y() &&
      i[0].y() == i[3].y();
  }
  else if (i[0].y() == i[1].y()) {
    return
      i[2].y() == i[3].y() &&
      i[1].x() == i[2].x() &&
      i[0].x() == i[3].x();
  }
  else
    return false;
}

std::ostream& operator << (std::ostream& out, const BezierPath& bp)
{
  out << "BezierPath bp_" << (void*)(&bp) << ";\n";
  out << "{ BezierPath& bp = bp_" << (void*)(&bp) << ";\n";
  //  out << "end: " << &(*bp.end())[0] << std::endl;
  for (BezierPath::const_iterator i = bp.begin(); i != bp.end(); ++i) {
    //out << "this one: " << &(*i)[0] << std::endl;
    switch((*i).type()) {
    case BezierPath::close:
      out << "\tbp.tclose_path();\n";
      break;
    case BezierPath::move:
      out << "\tbp.move_to(" << i[0].x() << "," << i[0].y() << ");\n";
      break;
    case BezierPath::line:
      out << "\tbp.line_to(" << i[1].x() << "," << i[1].y() << ");\n";
      break;
    case BezierPath::parabolic:
      out <<
	"\tbp.parabole_to("  << i[1].x() << "," << i[1].y() << ",\n" <<
	"\t               "  << i[2].x() << "," << i[2].y() << ");\n";
      break;
    case BezierPath::cubic:
      out <<
	"\tbp.cubic_to("  << i[1].x() << "," << i[1].y() << ",\n" <<
	"\t            "  << i[2].x() << "," << i[2].y() << ",\n" <<
	"\t            "  << i[3].x() << "," << i[3].y() << ");\n";
      break;
    }
  }
  return out << "}\n";
}


