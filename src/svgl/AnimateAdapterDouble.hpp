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
#ifndef __svgl_AnimateAdapterDouble__
#define __svgl_AnimateAdapterDouble__

#include <svgl/config.hpp>
#include <svgl/AnimateAdapter.hpp>
#include <w3c/svg/SVGAnimatedLength.hpp>
#include <w3c/svg/SVGAnimatedNumber.hpp>

#include <svgl/getattr.hpp>

namespace svgl {

  template <>
  struct AnimateAdapterTemplate< svg::Attribute< svg::SVGAnimatedLength > > : AnimateAdapter {
    AnimateAdapterTemplate( svg::Attribute< svg::SVGAnimatedLength > * attr) : _attr(attr) {/*std::cerr << attr << std::endl;*/}
    svg::Attribute< svg::SVGAnimatedLength> * _attr;
    virtual void setAsDouble(double v) throw (NotADouble) {
      svg::SVGLength l;
      l.newValueSpecifiedUnits(svg::SVGLength::SVG_LENGTHTYPE_NUMBER,v);
      _attr->getValue().getAnimatedVal().setValue(l);
    }
    virtual bool canBeSetAsDouble() { return true; }
    virtual double getBaseVal() const throw (NotADouble) { svg::SVGLength l= _attr->getValue().getBaseVal().getValue(); return l.computeValue(); }
  };

  template <>
  struct AnimateAdapterTemplate< svg::Attribute< svg::SVGAnimatedNumber > > : AnimateAdapter {
    AnimateAdapterTemplate( svg::Attribute< svg::SVGAnimatedNumber > * attr) : _attr(attr) {/*std::cerr << attr << std::endl;*/}
    svg::Attribute< svg::SVGAnimatedNumber> * _attr;
    virtual void setAsDouble(double v) throw (NotADouble) {
      svg::SVGNumber l;
      l.dom_setValue(v);
      _attr->getValue().getAnimatedVal().setValue(l);
    }
    virtual bool canBeSetAsDouble() { return true; }
    virtual double getBaseVal() const throw (NotADouble) { svg::SVGNumber l= MYGET_BASE_VAL((*_attr)); return l.GET_SIMPLE_VAL(Value); }
  };


}

#endif // __svgl_AnimateAdapterColor__
