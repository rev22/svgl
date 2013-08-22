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
#ifndef __svgl_KeyFrame__
#define __svgl_KeyFrame__

#include <svgl/config.hpp>

namespace svgl {

  template <class __ValueType=float>
  struct KeyFrame {
    typedef __ValueType ValueType;
    KeyFrame() {}
    KeyFrame(const ValueType& v, float t) : value(v), time(t) {}
    ValueType value;
    float time;
  };

}

#endif // __svgl_KeyFrame__
