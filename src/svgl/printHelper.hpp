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
#ifndef __printHelper__
#define __printHelper__

#include <svgl/config.hpp>
//#include <ostream>
#include <iostream>  // because of gcc 2.96
#include <w3c/svg/Animated.hpp>
#include <w3c/svg/Attribute.hpp>
#include <w3c/svg/SVGLength.hpp>
#include <w3c/svg/SVGNumber.hpp>
#include <w3c/svg/SVGList.hpp>
#include <w3c/svg/SVGRect.hpp>
#include <w3c/svg/SVGPreserveAspectRatio.hpp>
#include <w3c/svg/SVGPreserveAspectRatio.hpp>
#include <w3c/css/CSSStyleDeclaration.hpp>
#include <w3c/svg/SVGTransforms.hpp>

template <class X>
inline
std::ostream&
operator << (std::ostream& out, const svg::Attribute<X>& a)
{
  out << a.getValue();
  return out;
}

template <class X>
inline
std::ostream&
operator << (std::ostream& out, const svg::Animated<X>& a)
{
  out << a.getBaseVal();
  return out;
}

#if 0
// special for <view>
template <class X>
inline
std::ostream&
operator << (std::ostream& out, const svg::Attribute<X*>& a)
{
  return out;
}
#endif

template <class X>
inline
std::ostream&
operator << (std::ostream& out, const svg::SVGList<X>& a)
{
  //  out << a.getBaseVal();
  return out;
}


inline
std::ostream&
operator << (std::ostream& out, const svg::SVGLength& a)
{
  out << a.dom_getValueAsString();
  //  out << const_cast<svg::SVGLength&>(a).getValueAsString();
  return out;
}

inline
std::ostream&
operator << (std::ostream& out, const svg::SVGNumber& a)
{
  out << a.dom_getValue();
  //out << const_cast<svg::SVGNumber&>(a).getValue();
  return out;
}

inline
std::ostream&
operator << (std::ostream& out, const svg::SVGRect& a)
{
  out << "Rect";
  return out;
}

inline
std::ostream&
operator << (std::ostream& out, const svg::SVGPreserveAspectRatio& a)
{
  //  out << "Rect";
  return out;
}

inline
std::ostream&
operator << (std::ostream& out, const css::CSSStyleDeclaration& a)
{
  out << a.getStyleString();
  return out;
}

inline
std::ostream&
operator << (std::ostream& out, const svg::SVGTransform& t)
{
  t.print(out);
  return out;
}

//template <class X>
inline
std::ostream&
operator << (std::ostream& out, const svg::SVGList<svg::SVGTransform*>& a)
{
	// avoid printing a trailing whitespace
  int size = a.numberOfItems();
  if (size>1) {
		for(unsigned int i = 0; i<size-1; ++i) {
			//std::cerr << i << std::endl;
			a.getItem(i)->print(out);
			out << " ";
		}
	}
	if (size>0)
		a.getItem(size-1)->print(out);
  return out;
}


#endif // __printHelper__
