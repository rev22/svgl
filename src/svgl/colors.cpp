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
#include <w3c/css/RGBColor.hpp>
#include <w3c/css/URIColor.hpp>
//#include <w3c/css/CurrentColor.hpp>
#include <utils/String.hpp>
#include <utils/StringUtils.hpp>

#include <map>
#include <vector>

#include <svgl/debug.hpp>

namespace svg {

  typedef std::map<unicode::String*, css::RGBColor, string_less> ColorsType;
  ColorsType colors;

  ::css::RGBColor * currentColor;
  ::css::RGBColor * inheritColor;

  void
  init_colors()
  {
    {
      currentColor = new css::RGBColor;
      inheritColor = new css::RGBColor;
    }

    css::RGBColor c;
#define COLORDEF(name,R,G,B) c.r=float(R/255.); c.g=float(G/255.); c.b=float(B/255.); colors[unicode::String::internString(#name)] = c;
#include <w3c/svg/colors_def.hpp>
#undef COLORDEF
  }


#define DBG std::cerr << "invalid RGB color \"" << (end>beg ? s.substring(beg,end)->transcode() : "") << "\" in \""  << s << "\"" __FL__;

  bool
  getRGBColor(const unicode::String* s_ptr, css::Color** pcolor, unicode::String::size_type beg=0, unicode::String::size_type end=unicode::String::npos) {
    static const unicode::String::char_type sharp = '#', left_parenthesis='(', right_parenthesis=')', percentage='%', comma=',';
    static const unicode::String
      &none_string = *unicode::String::internString("none"),
      &currentColor_string = *unicode::String::internString("currentColor"),
      &inherit_string = *unicode::String::internString("inherit"),
      &url_string = *unicode::String::internString("url"),
      &rgb_string = *unicode::String::internString("rgb");

    if(!s_ptr)
      return false;

    const unicode::String& s = *s_ptr;

    const unicode::String::size_type len = s.getLength();
    if (!len)
      return false;

    if (end>len)
      end=len;

    if (s[beg]==sharp) {
      unsigned int val;
      if(unicode::get_hex_int(s, &val, beg+1, end)) {
	unsigned int dec, mask;
	float div;
	if (end-(beg+1)==6) {
	  dec = 8;
	  mask = 0xFF;
	  div=255.;
	}
	else if (end-(beg+1)==3) {
	  dec = 4;
	  mask = 0xF;
	  div=255.;
	}
	else {
	  DBG;
	  return false;
	}
	//std::cerr << "-- " << s.substring(beg+1, end) << std::hex << " #" << val << std::dec << std::endl;
	css::RGBColor color;
	unsigned int R,G,B;
	B = val & mask;
	val >>= dec;
	G = val & mask;
	val >>= dec;
	R = val & mask;
	if(dec==4) {
	  R += R << 4;
	  G += G << 4;
	  B += B << 4;
	}
	//std::cerr << std::hex << "#" << int(R) << " " << int(G) << " " << int(B) << std::dec << std::endl;
	color.r=R/div;
	color.g=G/div;
	color.b=B/div;
	*pcolor = new css::RGBColor(color);
	return true;
      }
      
    }
    else if ( (end-beg)>3 && *s.substring(beg, beg+3) == rgb_string ) {
      beg = s.indexOf(left_parenthesis, beg);
      if( beg>=end || beg==unicode::String::npos ) {
	DBG;
	return 0;
      }

      unsigned int val;
      css::RGBColor c;
      unicode::String::size_type pos;

      // red

      beg = unicode::eat_whitespaces(s, beg+1);
      if( beg>=end ) {
	DBG;
	return false;
      }
      
      pos = s.indexOf(comma, beg);
      if( pos>=end || pos==unicode::String::npos ) {
	DBG;
	return false;
      }

      if (!unicode::get_dec_int(s, &val, beg, pos) ) {
	DBG;
	return false;
      }

      if (s.indexOf(percentage, beg)<pos)
	c.r=val/100.;
      else
	c.r=val/255.;

      beg=pos+1;

      // green

      beg = unicode::eat_whitespaces(s, beg);
      if( beg>=end ) {
	DBG;
	return false;
      }
      pos = s.indexOf(comma, beg);
      if( pos>=end || pos==unicode::String::npos ) {
	DBG;
	return false;
      }

      if (!unicode::get_dec_int(s, &val, beg, pos) ) {
	DBG;
	return false;
      }

      if (s.indexOf(percentage, beg)<pos)
	c.g=val/100.;
      else
	c.g=val/255.;

      beg=pos+1;
      
      // blue

      beg = unicode::eat_whitespaces(s, beg);
      if( beg>=end ) {
	DBG;
	return 0;
      }
      pos=s.indexOf(right_parenthesis, beg);
      if( pos>=end || pos==unicode::String::npos ) {
	DBG;
	return 0;
      }

      if (!unicode::get_dec_int(s, &val, beg, pos) ) {
	DBG;
	return 0;
      }

      if (s.indexOf(percentage, beg)<pos)
	c.b=val/100.;
      else
	c.b=val/255.;

      // std::cerr << s << " " << c.r << " " << c.g << " " << c.b << std::endl;

      *pcolor = new css::RGBColor(c);
      return true;
    }
    else if ( (end-beg)>3 && *s.substring(beg, beg+3) == url_string ) {
      beg = s.indexOf(left_parenthesis, beg);
      if( beg>=end || beg==unicode::String::npos ) {
	DBG;
	return 0;
      }
      beg+=1;
      unicode::String::size_type pos = s.indexOf(right_parenthesis, beg);
      if( pos>=end ) {
	DBG;
	return 0;
      }
      
      css::URIColor * uc = new css::URIColor;
      uc->uri = s.substring(beg, pos);
      *pcolor=uc;
      return true;

    }
    else if (s == none_string) {
      *pcolor = css::Color::getNoneColor();
      return true;
    }
    else if (s == currentColor_string) {
      *pcolor = currentColor;
      return true;
    }
    else if (s == inherit_string) {
      *pcolor = inheritColor;
      return true;
    }
    else {
      unicode::String* s1_ptr = s.substring(beg,end);;
      //unicode::String& s1 = *s1_ptr;
      //std::cerr << s1.as_uniString() << " (" << colors->getLength() << ")" << __FL__;
      ColorsType::const_iterator it = colors.find(s1_ptr);
      if (it == colors.end()) {
	DBG
	return false;
      }
      else {
	css::RGBColor *tmp = new css::RGBColor((*it).second);
	//	std::cerr << "-- " << tmp << std::endl;
	*pcolor = tmp;
	return true;
      }
    }
    DBG;
    return false;
  }


}
