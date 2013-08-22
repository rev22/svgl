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
#ifndef __svg_strings__
#define __svg_strings__

#include <svgl/config.hpp>
#include <w3c/svg/String.hpp>
#include <vector>

namespace svg {

  typedef String string;
  typedef std::vector<string> strings;

  extern string empty_string;
  extern string unknown_string;

  // transform

  extern string matrix_string;
  extern string translate_string;
  extern string rotate_string;
  extern string scale_string;
  extern string skewX_string;
  extern string skewY_string;


  // xml syntax
  
  extern string open_element_string;
  extern string close_element_string;
  extern string open_close_element_string;
  extern string close_close_element_string;

  // elements
  extern string pcdata_element_string;

#define STRDEF(name, value) extern unicode::String* SVG##name##Element_element_string;
#include <w3c/svg/strings_def.hpp>
#undef STRDEF

  // attributes

#define STRDEF(name, value) extern string name##_attribute_string;
#include <w3c/svg/attribute_strings_def.hpp>
#undef STRDEF


  extern strings length_unit_strings;

void
init_strings();

}

#endif // __svg_strings__
