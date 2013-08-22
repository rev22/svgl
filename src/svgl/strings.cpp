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
//#include <w3c/svg/string.hpp>
#include <w3c/svg/strings.hpp>


// #include <w3c/svg/length.hpp>

namespace svg {

  string empty_string;
  string unknown_string;

  // transform 

  string matrix_string;
  string translate_string;
  string rotate_string;
  string scale_string;
  string skewX_string;
  string skewY_string;

  // xml syntax

  string open_element_string;
  string close_element_string;
  string open_close_element_string;
  string close_close_element_string;

  // elements

  string pcdata_element_string;

#define STRDEF(name, value) unicode::String* SVG##name##Element_element_string;
#include <w3c/svg/strings_def.hpp>
#undef STRDEF

  // attributes

#define STRDEF(name, value) string name##_attribute_string;
#include <w3c/svg/attribute_strings_def.hpp>
#undef STRDEF

  // length units

  strings length_unit_strings;

void
init_strings()
{

  empty_string = unicode::String::internString("");
  unknown_string = unicode::String::internString("unknown");

  // transform 

  matrix_string = unicode::String::internString("matrix");
  translate_string = unicode::String::internString("translate");
  rotate_string = unicode::String::internString("rotate");
  scale_string = unicode::String::internString("scale");
  skewX_string = unicode::String::internString("skewX");
  skewY_string = unicode::String::internString("skewY");

  // elements

  pcdata_element_string = unicode::String::internString("#PCDATA");
#define STRDEF(name, value) SVG##name##Element_element_string = unicode::String::internString(value);
#include <w3c/svg/strings_def.hpp>
#undef STRDEF

  // attributes

#define STRDEF(name, value) name##_attribute_string = unicode::String::internString(value);
#include <w3c/svg/attribute_strings_def.hpp>
#undef STRDEF

  // length units

  length_unit_strings.resize(10+1);
  { int i=1;
#define MKLENGTH(token) \
  length_unit_strings[i]=unicode::String::internString(#token);++i;

  MKLENGTH("") // number
    MKLENGTH(%) //, percentage)
    MKLENGTH(em) //, ems)
    MKLENGTH(ex) //, exs)
    MKLENGTH(px) //,px)
    MKLENGTH(cm) // ,cm)
    MKLENGTH(mm) //,mm)
    MKLENGTH(in) //,in)
    MKLENGTH(pt) //,pt)
    MKLENGTH(pc) //,pc)
#undef MKLENGTH
  }


}


}
