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

#include <utils/StringUtils.hpp>
#include <w3c/svg/SVGPointList.hpp>
#include <w3c/svg/SVGPoint.hpp>
#include <svgl/getattr.hpp>

#include <vector>

namespace svg {

#define DBG std::cerr << "invalid point list \"" << s << "\" " __FILE__ ":" << __LINE__ << std::endl;

  bool
  parsePoints(unicode::String* s_ptr, svg::SVGPointList* l)
  {
    const unicode::String& s = *s_ptr;
    const unicode::String::size_type len = s.getLength();

    if (!len)
      return true;

    std::vector<float> v;
    if(!unicode::get_float_list(s, &v)) {
      DBG;
      return false;
    }

    if ((v.size() % 2)!=0 ) {
      DBG;
      return false;
    }

    std::vector<float>::size_type vlen=v.size();

    for(std::vector<float>::size_type i=0; i<vlen; i+=2) {
      SVGPoint p;
      p.SET_SIMPLE_VAL(X,v[i]);
      p.SET_SIMPLE_VAL(Y,v[i+1]);
      //std::cerr << "(" << v[i] << "," << v[i+1] << ") ";
      l->appendItem(p);
    }
    /*
    if (vlen)
      std::cerr << std::endl;
    */
    return true;
  }

}

