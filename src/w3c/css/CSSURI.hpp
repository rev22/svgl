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
#ifndef __css_URI__
#define __css_URI__

#include <svgl/config.hpp>
#include <utils/String.hpp>

namespace css {
  struct URI {
    unicode::String* uri;
    URI() : uri(0) {}
  };

  bool
  get_css_uri(const unicode::String*, URI*);
}

inline
unicode::String*
read (unicode::String* in, css::URI& u)
{
  css::get_css_uri(in, &u);
  return in;
}


#endif // __css_URI__
