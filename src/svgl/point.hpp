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
#ifndef _point_h
#define _point_h

class istream;
class ostream;

enum dimension { x_dimension, y_dimension, z_dimension };

template <int Dim, class Coord>
struct vector_;

template <int Dim, class Coord>
struct point_ {
  Coord p[Dim];

  point_() { }
  point_(Coord nc[Dim]);
  point_(const point_<Dim, Coord>&);

  point_<Dim, Coord> operator - () const;
  point_<Dim, Coord> operator + (const vector_<Dim, Coord>& v) const;
  point_<Dim, Coord> operator - (const vector_<Dim, Coord>& v) const;
  vector_<Dim, Coord> operator - (const point_<Dim, Coord>& pt) const;
  point_<Dim, Coord>& operator += (const vector_<Dim, Coord>& v);
  point_<Dim, Coord>& operator -= (const vector_<Dim, Coord>& v);
  bool operator == (const point_<Dim, Coord>& pt) const;
  vector_<Dim, Coord>& to_vector();
  const vector_<Dim, Coord>& to_vector() const;
  Coord& operator [](int i) { return p[i]; }
  const Coord& operator [](int i) const { return p[i]; }
};

template <int Dim, class Coord>
struct vector_ {
  Coord v[Dim];

  vector_() { }
  vector_(Coord nv[Dim]);
  vector_(const vector_<Dim, Coord>&);

  vector_<Dim, Coord> operator - () const;
  vector_<Dim, Coord> operator + (const vector_<Dim, Coord>&) const;
  vector_<Dim, Coord> operator - (const vector_<Dim, Coord>&) const;
  vector_<Dim, Coord> operator * (const Coord&) const;
  vector_<Dim, Coord> operator / (const Coord&) const;
  vector_<Dim, Coord>& operator += (const vector_<Dim, Coord>&);
  vector_<Dim, Coord>& operator -= (const vector_<Dim, Coord>&);
  vector_<Dim, Coord>& operator *= (const Coord&);
  vector_<Dim, Coord>& operator /= (const Coord&);
  bool operator == (const vector_<Dim, Coord>&) const;
  point_<Dim, Coord>& to_point();
  const point_<Dim, Coord>& to_point() const;
  const Coord& operator [](int i) const { return v[i]; }
  Coord& operator [](int i) { return v[i]; }
};

#define CFOREACH for (int i = 0; i < Dim; i++)

template <int Dim, class Coord>
inline Coord norm2(const vector_<Dim, Coord>& ve)
{
  Coord c = 0;
  CFOREACH { c += v[i]*v[i]; }
  return c;
}

template <int Dim, class Coord>
inline
point_<Dim, Coord>::point_(Coord nc[Dim])
{
  CFOREACH { p[i] = nc[i]; }
}

template <int Dim, class Coord>
inline
point_<Dim, Coord>::point_(const point_<Dim, Coord>& op)
{
  CFOREACH { p[i] = op.p[i]; }
}

template <int Dim, class Coord>
inline
vector_<Dim, Coord>::vector_(Coord nv[Dim])
{
  CFOREACH { v[i] = nv[i]; }
}

template <int Dim, class Coord>
inline
vector_<Dim, Coord>::vector_(const vector_<Dim, Coord>& ov)
{
  CFOREACH { v[i] = ov.v[i]; }
}

template <int Dim, class Coord>
inline point_<Dim, Coord>
point_<Dim, Coord>::operator - () const
{
  point_<Dim, Coord> pt;

  CFOREACH { pt.p[i] = -p[i]; }
  return pt;
}

template <int Dim, class Coord>
inline point_<Dim, Coord>
point_<Dim, Coord>::operator + (const vector_<Dim, Coord>& v) const
{
  point_<Dim, Coord> pt;

  CFOREACH { pt.p[i] = p[i] + v.v[i]; }
  return pt;
}

template <int Dim, class Coord>
inline point_<Dim, Coord>
point_<Dim, Coord>::operator - (const vector_<Dim, Coord>& v) const
{
  point_<Dim, Coord> pt;

  CFOREACH { pt.p[i] = p[i] - v.v[i]; }
  return pt;
}

template <int Dim, class Coord>
inline vector_<Dim, Coord>
point_<Dim, Coord>::operator - (const point_<Dim, Coord>& pt) const
{
  vector_<Dim, Coord> ve;

  CFOREACH { ve.v[i] = p[i] - pt.p[i]; }
  return ve;
}

template <int Dim, class Coord>
inline point_<Dim, Coord>&
point_<Dim, Coord>::operator += (const vector_<Dim, Coord>& ve)
{
  CFOREACH { p[i] += ve.v[i]; }
  return *this;
}

template <int Dim, class Coord>
inline point_<Dim, Coord>&
point_<Dim, Coord>::operator -= (const vector_<Dim, Coord>& v)
{
  CFOREACH { p[i] -= v.v[i]; }
  return *this;
}

template <int Dim, class Coord>
inline bool
point_<Dim, Coord>::operator == (const point_<Dim, Coord>& pt) const
{
  CFOREACH {
    if (p[i] != pt.p[i])
      return false;
  }
  return true;
}

template <int Dim, class Coord>
inline ostream&
operator << (ostream& out, const point_<Dim, Coord> p)
{
  out << "point<" << Dim << ">::(" << p[0];
  for (int i = 1; i < Dim; i++) { out << ',' << p[i]; }
  return out << ')';
}

template <int Dim, class Coord>
inline istream&
operator >> (istream& in, point_<Dim, Coord>& p)
{
  in.ignore(sizeof("point<2>::(")-1);
  CFOREACH { in >> p[i]; in.ignore(1); }
  return in;
}

template <int Dim, class Coord>
inline vector_<Dim, Coord>&
point_<Dim, Coord>::to_vector()
{
  return *(vector_<Dim, Coord>*)(this);
}

template <int Dim, class Coord>
inline const vector_<Dim, Coord>&
point_<Dim, Coord>::to_vector() const
{
  return *(const vector_<Dim, Coord>*)(this);
}


template <int Dim, class Coord>
inline vector_<Dim, Coord>
vector_<Dim, Coord>::operator - () const
{
  vector_<Dim, Coord> ve;

  CFOREACH { ve.v[i] = -v[i]; }
  return ve;
}

template <int Dim, class Coord>
inline vector_<Dim, Coord>
vector_<Dim, Coord>::operator + (const vector_<Dim, Coord>& ve) const
{
  vector_<Dim, Coord> nv;

  CFOREACH { nv.v[i] = v[i] + ve.v[i]; }
  return nv;
}

template <int Dim, class Coord>
inline vector_<Dim, Coord>
vector_<Dim, Coord>::operator - (const vector_<Dim, Coord>& ve) const
{
  vector_<Dim, Coord> nv;

  CFOREACH { nv.v[i] = v[i] - ve.v[i];}
  return nv;
}

template <int Dim, class Coord>
inline vector_<Dim, Coord>
vector_<Dim, Coord>::operator * (const Coord& n) const
{
  vector_<Dim, Coord> nv;

  CFOREACH { nv.v[i] = v[i] * n; }
  return nv;
}

template <int Dim, class Coord>
inline vector_<Dim, Coord>
vector_<Dim, Coord>::operator / (const Coord& n) const
{
  vector_<Dim, Coord> nv;

  CFOREACH { nv.v[i] = v[i] / n; }
  return nv;
}

template <int Dim, class Coord>
inline vector_<Dim, Coord>&
vector_<Dim, Coord>::operator += (const vector_<Dim, Coord>& ve)
{
  CFOREACH { v[i] += ve.v[i]; }
  return *this;
}

template <int Dim, class Coord>
inline vector_<Dim, Coord>&
vector_<Dim, Coord>::operator -= (const vector_<Dim, Coord>& ve)
{
  CFOREACH { v[i] -= ve.v[i]; }
  return *this;
}

template <int Dim, class Coord>
inline vector_<Dim, Coord>&
vector_<Dim, Coord>::operator *= (const Coord& n)
{
  return *this = (*this) * n;
}

template <int Dim, class Coord>
inline vector_<Dim, Coord>&
vector_<Dim, Coord>::operator /= (const Coord& n)
{
  return *this = *this / n;
}

template <int Dim, class Coord>
inline bool
vector_<Dim, Coord>::operator == (const vector_<Dim,Coord>& ve) const
{
  CFOREACH { if (v[i] != ve.v[i]) return false; }
  return true;
}

template <int Dim, class Coord>
inline ostream&
operator << (ostream& out, const vector_<Dim, Coord>& v)
{
  out << "vector<" << Dim << ">::(" << v[0];
  for (int i = 1; i < Dim; i++) { out << ',' << v[i]; }
  return out << ')';
}

template <int Dim, class Coord>
inline istream&
operator >> (istream& in, vector_<Dim, Coord>& v)
{
  in.ignore(sizeof("vector<2>::(")-1);
  CFOREACH { in >> v[i]; in.ignore(1); }
  return in;
}

template <int Dim, class Coord>
inline point_<Dim, Coord>&
vector_<Dim, Coord>::to_point()
{
  return *(point_<Dim, Coord>*)(this);
}


template <int Dim, class Coord>
inline const point_<Dim, Coord>&
vector_<Dim, Coord>::to_point() const
{
  return *(const point_<Dim, Coord>*)(this);
}

#undef CFOREACH
#endif
