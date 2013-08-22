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
#ifndef __svg_Attribute__
#define __svg_Attribute__

#include <svgl/config.hpp>

namespace svg {

#if 0
  class AttributeBase {
  public:
    dom::string getValue()=0;
    void setValue(const dom::string& v) = 0;
  };
#endif

  template <typename X>
  class Attribute /*: public AttributeBase*/ {
  public:
    Attribute(const X& x=X()) : _x(x) {}
    virtual ~Attribute() {}
    virtual const X& getValue() const { return _x; }
    virtual X& getValue() { return _x; }
    virtual void setValue(const X& x) { _x = x; }

#if 0
    dom::string getValue() {
      return "";
    }

    void setValue(dom::string& v) {
    }
#endif


  public:
    //    operator X() const { return _x; }
  private:
    X _x;
  };

}

#endif // __svg_Attribute__
