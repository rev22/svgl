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
#include <svgl/parseHelper.hpp>
#include <w3c/svg/SVGPreserveAspectRatio.hpp>

#include <w3c/svg/SVGLength.hpp>
#include <w3c/svg/SVGNumber.hpp>

#include <svgl/debug.hpp>

namespace svgl {

void
read(unicode::String* pARstring, svg::SVGPreserveAspectRatio& pAR)
{
  if(!pARstring)
    return;
  unicode::String::size_type pARsize=pARstring->getLength();
  static const dom::string none_string = unicode::String::internString("none");
  dom::string align = none_string;
  dom::string meetOrSlice=0;

  pAR.dom_setAlign(svg::SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_NONE);
  //pAR.dom_setMeetOrSlice(svg::SVGPreserveAspectRatio::SVG_MEETORSLICE_UNKNOWN);
  pAR.dom_setMeetOrSlice(svg::SVGPreserveAspectRatio::SVG_MEETORSLICE_MEET);

  if (pARsize) {
    if(*pARstring==*none_string) {
      return;
    }

    static const dom::string spaces = unicode::String::internString(" \t\n\r");
    unicode::String::size_type beg = unicode::eat_whitespaces(*pARstring), pos;
    if (beg<pARsize) {
      pos = unicode::find_among(*pARstring, *spaces, beg, pARsize);
      if (pos>pARsize)
	pos=pARsize;

      align = pARstring->substring(beg, pos);

      beg = unicode::eat_whitespaces(*pARstring, pos);
      pos = unicode::find_among(*pARstring, *spaces, beg, pARsize);
      if (pos>pARsize)
	pos=pARsize;
      if(beg<pos) {
	meetOrSlice = pARstring->substring(beg, pos);
      }
    }

    if( (!align || !align->getLength()) /*&& (!meetOrSlice || !meetOrSlice->getLength())*/ ) {
      std::cerr << "wrong preserveAspectRatio \"" << pARstring << "\"" << __FL__;
    }
  }
  else {
    return;
  }
  //std::cerr << "preserveAspectRatio \"" << pARstring << "\" " << align << " " << meetOrSlice << __FL__;; 
  
  const static dom::string
#define STRDEF(x) x##_string = unicode::String::internString(#x)
    STRDEF(xMinYMin),
    STRDEF(xMidYMin),
    STRDEF(xMaxYMin),
    STRDEF(xMinYMid),
    STRDEF(xMidYMid),
    STRDEF(xMaxYMid),
    STRDEF(xMinYMax),
    STRDEF(xMidYMax),
    STRDEF(xMaxYMax);
#undef STRDEF

#define TESTIT(a,A) \
  if(*align==*a##_string) { \
    pAR.dom_setAlign(svg::SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_##A); \
  } else \

    TESTIT(none,NONE)
    TESTIT(xMinYMin,XMINYMIN)
    TESTIT(xMidYMin,XMIDYMIN)
    TESTIT(xMaxYMin,XMAXYMIN)
    TESTIT(xMinYMid,XMINYMID)
    TESTIT(xMidYMid,XMIDYMID)
    TESTIT(xMaxYMid,XMAXYMID)
    TESTIT(xMinYMax,XMINYMAX)
    TESTIT(xMidYMax,XMIDYMAX)
    TESTIT(xMaxYMax,XMAXYMAX)

#undef TESTIT
    {
      std::cerr << "wrong preserveAspectRatio \"" << align << "\"" __FL__;
      pAR.dom_setAlign(svg::SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_UNKNOWN);
    }

    const static dom::string
      meet_string=unicode::String::internString("meet"),
      slice_string=unicode::String::internString("slice");
    
    if(meetOrSlice) {
      if(*meetOrSlice==*meet_string) {
	pAR.dom_setMeetOrSlice(svg::SVGPreserveAspectRatio::SVG_MEETORSLICE_MEET);
      }
      else if (*meetOrSlice==*slice_string) {
	pAR.dom_setMeetOrSlice(svg::SVGPreserveAspectRatio::SVG_MEETORSLICE_SLICE);
      }
      else {
	std::cerr << "wrong meetOrSlice \"" << meetOrSlice << "\"" __FL__;
	pAR.dom_setMeetOrSlice(svg::SVGPreserveAspectRatio::SVG_MEETORSLICE_UNKNOWN);
      }
    }
}


void
read (unicode::String* in, svg::SVGLengthList& l)
{
  if(!in)
    return;
  unicode::String::size_type len = in->getLength();
  if (!len)
    return;

  unicode::String::size_type beg = unicode::eat_whitespaces(*in);
  static const dom::string sep = unicode::String::internString(" \t\n\r,;");

  while(beg<len) {
    unicode::String::size_type pos = unicode::find_among(*in, *sep, beg);
    svg::SVGLength length;
    if(pos==unicode::String::npos)
      pos = in->getLength();
    dom::string sub = in->substring(beg, pos);
    length.dom_setValueAsString(sub);
    l.appendItem(length);
    beg = unicode::eat_among(*in, *sep, pos);
  }

}


void
read (unicode::String* in, svg::SVGNumberList& l)
{
  if(!in)
    return;
  std::vector<float> v;
  unicode::get_float_and_percentage_list(*in, &v);
  for(std::vector<float>::const_iterator i=v.begin();
      i!=v.end();
      ++i) {
    svg::SVGNumber n;
    n.dom_setValue(*i);
    l.appendItem(n);
  }
}

void
read (unicode::String* in, svg::SVGList<dom::string>& l)
{
  if(!in)
    return;
  unicode::String::size_type len = in->getLength();
  if (!len)
    return;
  static const dom::string sep = unicode::String::internString(" \t\n\r,;");

  unicode::String::size_type beg = unicode::eat_whitespaces(*in);
  if (beg>=len)
    return;

  while(beg<len) {
    unicode::String::size_type pos = unicode::find_among(*in, *sep, beg);
    unicode::String* s=0;
    if(pos==unicode::String::npos) {
      s=in->substring(beg,len-1);
      beg=len;
    }
    else {
      s=in->substring(beg,pos);
      beg = unicode::eat_whitespaces(*in, pos);
    }

    l.appendItem(s);

  }

}

} // namespace svgl

	

