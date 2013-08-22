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
#ifndef __svgl_AnimateAdapter__
#define __svgl_AnimateAdapter__

#include <svgl/config.hpp>
#include <w3c/svg/Attribute.hpp>

namespace css {
  class Color;
}

namespace svgl {

  class AnimateAdapter {
  public:
    virtual ~AnimateAdapter() {}

    struct exc {};
    struct NotADouble : exc {};
    struct NotAColor : exc {};

    virtual void setAsDouble(double) throw (NotADouble) { throw NotADouble(); }
    virtual bool canBeSetAsDouble() { return false; }
    virtual double getBaseVal() const throw (NotADouble) { throw NotADouble(); }

    virtual void setAsColor(const css::Color*) throw (NotAColor) { throw NotAColor(); }
    virtual bool canBeSetAsColor() { return false; }
  };

  template <class __Attr>
  struct AnimateAdapterTemplate : AnimateAdapter {
    typedef __Attr Attr;
    AnimateAdapterTemplate(Attr* attr) : _attr(attr) {}
    Attr* _attr;
    virtual void setAsDouble(double) throw (NotADouble) { throw NotADouble(); }
    virtual bool canBeSetAsDouble() { return false; }
    virtual double getBaseVal() const throw (NotADouble) { throw NotADouble(); }

    virtual void setAsColor(const css::Color*) throw (NotAColor) { throw NotAColor(); }
    virtual bool canBeSetAsColor() { return false; }
  };


}



#endif // __svgl_AnimateAdapter__
