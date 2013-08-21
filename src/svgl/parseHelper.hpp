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
#ifndef __parseHelper__
#define __parseHelper__

#include <svgl/config.hpp>
#include <w3c/svg/Attribute.hpp>
#include <w3c/svg/Animated.hpp>
#include <w3c/svg/SVGLength.hpp>
#include <w3c/svg/SVGRect.hpp>
#include <w3c/svg/SVGNumber.hpp>
#include <w3c/svg/SVGList.hpp>

#include <w3c/svg/SVGTransformList.hpp>
#include <w3c/svg/SVGPointList.hpp>
#include <w3c/svg/SVGPathSegList.hpp>

#include <w3c/svg/SVGAnimationElement.hpp>
#include <w3c/svg/SVGAnimatedLengthList.hpp>
#include <w3c/svg/SVGAnimatedNumberList.hpp>
#include <w3c/svg/SVGAnimateTransformElement.hpp>
#include <w3c/svg/SVGURIReference.hpp>
#include <w3c/svg/SVGLangSpace.hpp>
#include <w3c/svg/SVGZoomAndPan.hpp>
#include <w3c/svg/SVGFECompositeElement.hpp>
#include <w3c/svg/Attribute.hpp>
#include <w3c/svg/Animated.hpp>
#include <w3c/svg/SVGTextPathElement.hpp>
#include <w3c/svg/SVGFEColorMatrixElement.hpp>
#include <w3c/svg/SVGFEBlendElement.hpp>
#include <w3c/svg/SVGUnitTypes.hpp>
#include <w3c/svg/SVGFETurbulenceElement.hpp>
#include <w3c/svg/SVGFEConvolveMatrixElement.hpp>
#include <w3c/svg/SVGFEConvolveMatrixElement.hpp>
#include <w3c/svg/SVGFEMorphologyElement.hpp>
#include <w3c/svg/SVGComponentTransferFunctionElement.hpp>
#include <w3c/svg/SVGGradientElement.hpp>
#include <w3c/svg/SVGFEDisplacementMapElement.hpp>
#include <w3c/svg/SVGMarkerElement.hpp>
#include <w3c/svg/makeEnumType.hpp>

#include <w3c/svg/SVGRenderingIntent.hpp>
#include <w3c/css/CSSStyleDeclaration.hpp>
#include <utils/StringUtils.hpp>
#include <vector>

namespace svg {
  class SVGElement;
  class SVGPreserveAspectRatio;
}

namespace svg {
  extern void parseTransform(unicode::String*, svg::SVGList<svg::SVGTransform*>* );
  extern bool parsePoints(unicode::String*, svg::SVGPointList*);
  extern void parsePathSegList(unicode::String*, svg::SVGPathSegList*);
}

namespace svg {
  typedef svg::SVGList<svg::SVGLength> SVGLengthList;
}

// basic types

namespace svgl {
using namespace svg;
inline
void
read( unicode::String*& in,  unicode::String*& s)
{
  s=in;
}

inline
void
read ( unicode::String*& in, int& b)
{
  if(in)
    unicode::get_dec_signed_int(*in, &b);
}

inline
void
read ( unicode::String*& in, float& b)
{
  if(in)
    unicode::get_float(*in, &b);
}

inline
void
read ( unicode::String*& in, bool& b)
{
  if(in)
    unicode::get_bool(*in, &b);
}

inline
void
read( unicode::String*& in, svg::SVGLength& l)
{
  if(in)
    l.dom_setValueAsString(in);
}

inline
void
read( unicode::String*& in, svg::SVGRect& l)
{
  std::vector<float> v;
  if(in && !unicode::get_float_list(*in, &v)) {
    std::cerr << "could not read SVGRect " __FILE__ << ":" << __LINE__ << std::endl;
    return;
  }
#if 1
  if(!v.size())
    return;
#endif
  if(v.size()!=4) {
    std::cerr << "SVGRect " << in << " has not 4 coords " __FILE__ << ":" << __LINE__ << std::endl;
    return;
  }

  l.dom_setX(v[0]);
  l.dom_setY(v[1]);
  l.dom_setWidth(v[2]);
  l.dom_setHeight(v[3]);

}


inline
void
read ( unicode::String*& in, svg::SVGNumber& b)
{
  float f;
  if(in) {
    unicode::get_float(*in, &f);
    b.dom_setValue(f);
  }
}

// lists

void
read ( unicode::String*, svg::SVGLengthList& );
void
read ( unicode::String* in, svg::SVGNumberList& );
void
read ( unicode::String* in, svg::SVGList<dom::string>& );


// other stuff

void
read ( unicode::String* in, svg::SVGPreserveAspectRatio& l);

inline
void
read( unicode::String*& in, svg::SVGElement*& e)
{
}


inline
void
read( unicode::String*& in, css::CSSStyleDeclaration& l)
{
  //if(in)
  l.setStyleString(in);
}

inline
void
read ( unicode::String*& in, svg::SVGList<svg::SVGTransform*>& l)
{
  if(in)
    svg::parseTransform(in, &l);
}

inline
void
read ( unicode::String*& in, svg::SVGPointList& l)
{
  if(in)
    svg::parsePoints(in, &l);
}

inline
void
read ( unicode::String*& in, svg::SVGPathSegList& l)
{
  if(in)
    svg::parsePathSegList(in, &l);
}

template <class X>
inline
void
read( unicode::String*& in, svg::Animated<X>& anim)
{
  X& x = anim.getBaseVal().getValue();
  if(in)
    read(in, x);
  anim.getAnimatedVal().setValue(x);
}

// basic SVG types

template <class X>
inline
void
read( unicode::String* in, svg::Attribute<X>& attr)
{
  if(in) {
    X x=attr.getValue();
    read(in, x);
    attr.setValue(x);
  }
}

#if 0
template <>
inline
void
read( unicode::String* in, svg::Attribute<unicode::String*>& attr)
{
  if(in) {
    unicode::String *x = attr.getValue();
    read(in, x);
    attr.setValue(x);
  }
}
#endif

  
} // namespace svgl

template <class X>
inline
void
operator >> (unicode::String& in, X& x)
{
  svgl::read(&in, x);
}


#endif // __parseHelper__

