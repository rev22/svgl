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
#include <svgl/parseTransform.hpp>
#include <w3c/svg/SVGTransform.hpp>
#include <w3c/svg/SVGTransformList.hpp>

#include <w3c/svg/SVGTransforms.hpp>
#include <vector>

#include <utils/StringUtils.hpp>
#include <svgl/debug.hpp>

using namespace unicode;

namespace svg {

  extern String* matrix_string;
  extern String* translate_string;
  extern String* rotate_string;
  extern String* scale_string;
  extern String* skewX_string;
  extern String* skewY_string;

String::size_type
find_balanced_patenthesis(const String& s, String::size_type beg=0)
{
  static const String::char_type left_parenthesis='(';
  static const String::char_type right_parenthesis=')';

  //  const String& s = *s_ptr;

  const String::size_type len = s.getLength();
  String::size_type pos=beg;

  if (pos==String::npos) {
    return pos;
  }
  int depth=1;
  ++pos;
  while (pos<len && depth) {
    if (s[pos]==left_parenthesis)
      ++depth;
    else if (s[pos]==right_parenthesis)
      --depth;
    ++pos;
  }
  if (depth)
    return String::npos;
  else
    return pos;
}

void
parseTransform(unicode::String* s_ptr, SVGTransformList *thelist)
{
  const String& s = *s_ptr;

  String::size_type beg=0, pos=0, len=s.getLength();
  static const String::char_type left_parenthesis='(';
  
  while (pos<len) {
    beg = eat_whitespaces(s, beg);
    if(beg==len)
      // ok, it's the end
      break;
    // find balanced parenthesis
    pos = s.indexOf(left_parenthesis, beg);
    if (pos==String::npos) {
      // mmm, must be an error
      std::cerr << "error in transform list \"" << s << "\" especially from here \"" << s.substring(beg) << "\"" << std::endl;
      break;
    }
    pos = find_balanced_patenthesis(s, pos);
    if (pos==String::npos) {
      std::cerr << "error in transform list \"" << s << "\" unbalanced parenthesis from here \"" << s.substring(beg) << "\"" << __FL__;
      break;
    }
    // ok, get the transform now
    SVGTransform * t = parseTransform(&s, beg, pos);
    beg=pos;
    if(t)
      thelist->appendItem(t);
  }

}

SVGTransform*
parseTransform(const unicode::String* str_ptr, String::size_type beg, String::size_type end)
{
  /*
  matrix(<a> <b> <c> <d> <e> <f>)
  translate(<tx> [<ty>])
  scale(<sx> [<sy>])
  rotate(<rotate-angle> [<cx> <cy>])
  skewX(<skew-angle>)
  skewY(<skew-angle>)
  */

  if(!str_ptr)
    return 0;

  static const String::char_type left_parenthesis='(';

  const unicode::String& str = *str_ptr;
  String::size_type pos = str.indexOf(left_parenthesis, beg);
  if(pos==unicode::String::npos) {
    return 0;
  }
  String* desc = str.substring(beg, pos);
  beg=pos;
  pos = find_balanced_patenthesis(str, pos);
  ++beg;

  std::vector<float> floatlist;
  get_float_list(str, &floatlist, beg, pos-1);
#if 0
  for (std::vector<float>::const_iterator i=floatlist.begin(); i<floatlist.end(); ++i)
    std::cerr << *i << " ";
  std::cerr << std::endl;
#endif

  SVGTransform * result=0;
  try {
  if (*desc == *matrix_string) {
#if 0
    std::cerr << desc << std::endl;
#endif
    result = new SVGMatrixTransform(floatlist);
  }
  else if (*desc == *translate_string) {
#if 0
    std::cerr << desc << std::endl;
#endif
    result = new SVGTranslateTransform(floatlist);
  }
  else if (*desc == *scale_string) {
#if 0
    std::cerr << desc << std::endl;
#endif
    result = new SVGScaleTransform(floatlist);
  }
  else if (*desc == *rotate_string) {
#if 0
    std::cerr << desc << std::endl;
#endif
    result = new SVGRotateTransform(floatlist);
  }
  else if (*desc == *skewX_string) {
#if 0
    std::cerr << desc << std::endl;
#endif
    result = new SVGSkewXTransform(floatlist);
  }
  else if (*desc == *skewY_string) {
#if 0
    std::cerr << desc << std::endl;
#endif
    result = new SVGSkewYTransform(floatlist);
  }
  else {
    std::cerr << "unkown transform " << desc << __FL__;
    result = 0;
  }
  }
  catch(const SVGTransform::WrongNumberOfArguments&) {
    result=0;
  }

  return result;
}

}
