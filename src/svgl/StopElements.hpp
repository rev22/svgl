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
#ifndef __StopElements__
#define __StopElements__

#include <svgl/config.hpp>
#include <vector>

namespace css {
  class RGBColor;
}

namespace svgl {

  struct StopElementInfo {
    const css::RGBColor* color;
    float opacity;
    float offset;
  };

  typedef std::vector<StopElementInfo> StopElements;

}

#endif // __StopElements__
