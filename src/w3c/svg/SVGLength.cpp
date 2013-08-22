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
#include <w3c/svg/SVGLength.hpp>
#include <utils/StringUtils.hpp>

#include <svgl/debug.hpp>

namespace svg {

	
void
SVGLength::setValueAsString(const DOMString& s_ptr)
{
	dom_setValueAsString(s_ptr);
}

void
SVGLength::dom_setValueAsString(const DOMString& s_ptr)
{

#define MKLENGTH(token,name) \
  static unicode::String* name##_string_ptr = unicode::String::internString(#token); \
  unicode::String& name##_string = * name##_string_ptr;
  
  MKLENGTH(%, PERCENTAGE);
  MKLENGTH(em, EMS);
  MKLENGTH(ex, EXS);
  MKLENGTH(px, PX);
  MKLENGTH(cm, CM);
  MKLENGTH(mm, MM);
  MKLENGTH(in, IN);
  MKLENGTH(pt, PT);
  MKLENGTH(pc, PC);
  //MKLENGTH("", NUMBER);
#undef MKLENGTH
    
  if(s_ptr==0) {
    unitType.setValue(SVG_LENGTHTYPE_NUMBER);
    valueAsString.setValue(0);
    _computed = false;
    computeValue();
    return;
  }

  unicode::String& s = *s_ptr;

  float v;
  unicode::String::size_type beg, after;
  beg = unicode::eat_whitespaces(s);
  if (beg>=s.getLength())
    return;
  if(!unicode::get_float(s, &v, beg, unicode::String::npos, &after)) {
    std::cerr << "invalid length " << s << " " << __FILE__ << ":" << __LINE__ << std::endl;
    return;
  }
  valueInSpecifiedUnits.setValue(v);

  //std::cerr << "(" << this << ") value -" << v << "- after -" << after << "- (" <<  s << ")" << std::endl;

  unicode::String* unit_ptr = s.substring(after);
  unicode::String& unit= *unit_ptr;
  //std::cerr << "unit -" << unit << "-" << std::endl;

    if(unit_ptr==0 || unit.getLength()==0) {
      unitType.setValue(SVG_LENGTHTYPE_NUMBER);
    }
    else

#define MKLENGTH(token, name) \
  if(unit==name##_string) { \
    unitType.setValue(SVG_LENGTHTYPE_##name); \
  } else \

  MKLENGTH(%, PERCENTAGE)
    MKLENGTH(em, EMS)
    MKLENGTH(ex, EXS)
    MKLENGTH(px, PX)
    MKLENGTH(cm, CM)
    MKLENGTH(mm, MM)
    MKLENGTH(in, IN)
    MKLENGTH(pt, PT)
    MKLENGTH(pc, PC)
    //    MKLENGTH("", NUMBER)
#undef MKLENGTH

    {
      std::cerr << "invalid length " << s << " (value:" << v << " unit: " << unit << ")" << __FILE__ << ":" << __LINE__  << std::endl;
      //throw invalid_length(sch,__FILE__, __LINE__);
    }
  
  valueAsString.setValue(&s);
  _computed = false;
  computeValue();
}

float
SVGLength::computeValue(float dpi, float percentage, float fontSize, float xHeight)
{
  if (_computed && (unitType.getValue()!=SVG_LENGTHTYPE_PERCENTAGE))
    return value.getValue();

  float val = valueInSpecifiedUnits.getValue();

  /*
"1pt" equals "1.25px" (and therefore 1.25 user units)
"1pc" equals "15px" (and therefore 15 user units)
"1mm" would be "3.543307px" (3.543307 user units)
"1cm" equals "35.43307px" (and therefore 35.43307 user units)
"1in" equals "90px" (and therefore 90 user units) 
  */

  /*
"1in" equals "2.54cm"
"72pt" equals "1in"
"1pc" equals "12pt" (pica)
  */
  if (val != 0) {
    switch (unitType.getValue()) {
    case SVG_LENGTHTYPE_PT:
      //    val *= dpi*1.25/90.;
      val *= dpi/72.;
      break;
    case SVG_LENGTHTYPE_PC:
      //    val *= dpi*15/90.;
      val *= dpi/6.;
      break;
    case SVG_LENGTHTYPE_MM:
      //    val *= dpi*3.543307/90.;
      val *= dpi/25.4;
      break;
    case SVG_LENGTHTYPE_CM:
      //    val *= dpi*35.43307/90.;
      val *= dpi/2.54;
      break;
    case SVG_LENGTHTYPE_IN:
      val *= dpi;
      break;
    case SVG_LENGTHTYPE_PERCENTAGE:
      val *= percentage/100.;
      break;
    case SVG_LENGTHTYPE_EMS:
      val *= fontSize;
      break;
    case SVG_LENGTHTYPE_EXS:
      val *= xHeight;
      break;

    case SVG_LENGTHTYPE_PX:
    case SVG_LENGTHTYPE_NUMBER:
      break;
    default:
      std::cerr << valueAsString.getValue() << " " << valueInSpecifiedUnits.getValue() << __FL__;
      break;
    }
  }

  value.setValue(val);
  _computed = true;
  return val;

}

bool
SVGLength::isAbsolute() const
{
  bool res=true;
  switch (unitType.getValue()) {
  case SVG_LENGTHTYPE_PERCENTAGE:
    res=false;
    break;
  default:
    break;
  }
  //  std::cerr << valueAsString.getValue() << " absolute: " << res << std::endl;
  return res;
}

void
SVGLength::newValueSpecifiedUnits(unsigned short ut, float vu)
{
  //std::cerr << DBGVAR(this) << DBGVAR(vu) << __FL__;
  _computed = false;
  valueInSpecifiedUnits.setValue(vu);
  svg::SVGLength::SVG_LENGTHTYPE tmp = static_cast<svg::SVGLength::SVG_LENGTHTYPE>(ut);
  unitType.setValue(tmp);
  computeValue();
}

void
SVGLength::convertToSpecifiedUnits ( unsigned short unitType )
{
  STDDBG;
}


}


