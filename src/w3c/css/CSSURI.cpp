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
#include <w3c/css/CSSURI.hpp>
#include <utils/StringUtils.hpp>



namespace css {
  
  bool
  get_css_uri(const unicode::String* s_ptr, URI* u)
  {
    bool res = false;
    static const unicode::String::char_type left_parenthesis = '(', right_parenthesis = ')';
    static const unicode::String* url = unicode::String::internString("url");

    const unicode::String& s = *s_ptr;

    if(!s.getLength())
      return res;

    unicode::String::size_type pos = unicode::eat_whitespaces(s);
    if(!s.substring(pos, 3)->equals(url)) {
      return res;
    }
    pos = s.indexOf(left_parenthesis);
    if(pos==unicode::String::npos)
      return res;
    unicode::String::size_type end = s.indexOf(right_parenthesis);
    if(pos==unicode::String::npos)
      return res;

    u->uri = s.substring(pos+1, end);

    return true;
  }

}
