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
#ifndef __dom_string__
#define __dom_string__

#include <utils/String.hpp>

namespace dom {

#if 0
  template <class __S>
  class svgl_string {
  public:
    typedef __S string_type;
    typedef string_type::char_type value_type;
    typedef string_type::size_type size_type;
  };

  typedef svgl_string<unicode::String> string;
#endif

  typedef unicode::String* string;

} // namespace dom


#endif // __dom_string__


