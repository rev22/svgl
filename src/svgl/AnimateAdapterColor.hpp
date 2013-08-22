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
#ifndef __svgl_AnimateAdapterColor__
#define __svgl_AnimateAdapterColor__

#include <svgl/config.hpp>
#include <svgl/AnimateAdapter.hpp>
#include <w3c/css/CSSStyleDeclaration.hpp>

namespace svgl {

  template <>
  struct AnimateAdapterTemplate< css::CSSconst_PaintTypeStyle > : AnimateAdapter {
    typedef css::CSSconst_PaintTypeStyle Attr;
    AnimateAdapterTemplate(Attr * attr) : _attr(attr) {/*std::cerr << attr << std::endl;*/}
    Attr * _attr;
    virtual void setAsDouble(double v) throw (NotADouble) { throw NotADouble(); }
    virtual bool canBeSetAsDouble() { return false; }
    virtual double getBaseVal() const throw (NotADouble) { throw NotADouble(); }

    virtual void setAsColor(const css::Color* c) throw (NotAColor) { _attr->get() = c; }
    virtual bool canBeSetAsColor() { return true; }

  };

}

#endif // __svgl_AnimateAdapterColor__
