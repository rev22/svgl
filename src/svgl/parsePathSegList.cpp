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
#include <w3c/svg/SVGPathSegList.hpp>

#include <w3c/svg/SVGPathSegArcAbs.hpp>
#include <w3c/svg/SVGPathSegArcRel.hpp>
#include <w3c/svg/SVGPathSegClosePath.hpp>
#include <w3c/svg/SVGPathSegCurvetoCubicAbs.hpp>
#include <w3c/svg/SVGPathSegCurvetoCubicRel.hpp>
#include <w3c/svg/SVGPathSegCurvetoCubicSmoothAbs.hpp>
#include <w3c/svg/SVGPathSegCurvetoCubicSmoothRel.hpp>
#include <w3c/svg/SVGPathSegCurvetoQuadraticAbs.hpp>
#include <w3c/svg/SVGPathSegCurvetoQuadraticRel.hpp>
#include <w3c/svg/SVGPathSegCurvetoQuadraticSmoothAbs.hpp>
#include <w3c/svg/SVGPathSegCurvetoQuadraticSmoothRel.hpp>
#include <w3c/svg/SVGPathSegLinetoAbs.hpp>
#include <w3c/svg/SVGPathSegLinetoRel.hpp>
#include <w3c/svg/SVGPathSegLinetoHorizontalAbs.hpp>
#include <w3c/svg/SVGPathSegLinetoHorizontalRel.hpp>
#include <w3c/svg/SVGPathSegLinetoVerticalAbs.hpp>
#include <w3c/svg/SVGPathSegLinetoVerticalRel.hpp>
#include <w3c/svg/SVGPathSegMovetoAbs.hpp>
#include <w3c/svg/SVGPathSegMovetoRel.hpp>


#include <utils/String.hpp>
#include <utils/StringUtils.hpp>
#include <vector>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

namespace svg {

  void
  parsePathSegList(unicode::String* s_ptr, SVGPathSegList* l)
  {
    static const unicode::String* all_commands_ptr=unicode::String::internString("MmZzLlHhVvCcSsQqTtAa");
    const unicode::String& all_commands = * all_commands_ptr;

    const unicode::String& s = *s_ptr;

    #define CMD(classname, com, foo) \
	static const unicode::String::char_type classname##_char=com; \
	
    //static const unicode::String& classname##_string = *unicode::String::internString(#classname);

CMD(SVGPathSegArcAbs, 'A', PATHSEG_ARC_ABS);
CMD(SVGPathSegArcRel, 'a', PATHSEG_ARC_REL);
CMD(SVGPathSegClosePath, 'z', PATHSEG_CLOSEPATH);
static const unicode::String::char_type SVGPathSegClosePath_char2='Z';
CMD(SVGPathSegCurvetoCubicAbs, 'C', PATHSEG_CURVETO_CUBIC_ABS);
CMD(SVGPathSegCurvetoCubicRel, 'c', PATHSEG_CURVETO_CUBIC_REL);
CMD(SVGPathSegCurvetoCubicSmoothAbs, 'S', PATHSEG_CURVETO_CUBIC_SMOOTH_ABS);
CMD(SVGPathSegCurvetoCubicSmoothRel, 's', PATHSEG_CURVETO_CUBIC_SMOOTH_REL);
CMD(SVGPathSegCurvetoQuadraticAbs, 'Q', PATHSEG_CURVETO_QUADRATIC_ABS);
CMD(SVGPathSegCurvetoQuadraticRel, 'q', PATHSEG_CURVETO_QUADRATIC_REL);
CMD(SVGPathSegCurvetoQuadraticSmoothAbs, 'T', PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS);
CMD(SVGPathSegCurvetoQuadraticSmoothRel, 't', PATHSEG_CURVETO_QUADRATIC_SMOOTH_REL);
CMD(SVGPathSegLinetoAbs, 'L', PATHSEG_LINETO_ABS);
CMD(SVGPathSegLinetoRel, 'l', PATHSEG_LINETO_REL);
CMD(SVGPathSegLinetoHorizontalAbs, 'H', PATHSEG_LINETO_HORIZONTAL_ABS);
CMD(SVGPathSegLinetoHorizontalRel, 'h', PATHSEG_LINETO_HORIZONTAL_REL);
CMD(SVGPathSegLinetoVerticalAbs, 'V', PATHSEG_LINETO_VERTICAL_ABS);
CMD(SVGPathSegLinetoVerticalRel, 'v', PATHSEG_LINETO_VERTICAL_REL);
CMD(SVGPathSegMovetoAbs, 'M', PATHSEG_MOVETO_ABS);
CMD(SVGPathSegMovetoRel, 'm', PATHSEG_MOVETO_REL);
 
    #undef CMD


      
    const unicode::String::size_type slen=s.getLength();
    if (!slen)
      return;

    unicode::String::size_type beg=0;

    beg = unicode::eat_whitespaces(s, beg);
    while (beg<slen) {
      
      switch(s[beg]) {
#define PARSEDBG std::cerr << "bad command \"" << ((end>beg && end<=s.getLength())? s.substring(beg, end)->transcode():"")  << "\" in \""<< s << "\"" __FL__; return;

#define CMD_FLOATLIST(cmd_class, following_class)					\
      case cmd_class##_char: {								\
	++beg;										\
	unicode::String::char_type end = unicode::find_among(s, all_commands, beg);	\
	beg = unicode::eat_whitespaces(s, beg, end);					\
	std::vector<float> v;								\
	if( !unicode::get_float_list(s, &v, beg, end)) { PARSEDBG; }			\
        const std::vector<float>::size_type dim=2;					\
	if ( (v.size()<dim) || (v.size()%dim) ) { PARSEDBG; }				\
											\
	cmd_class* p = new cmd_class;							\
	p->SET_SIMPLE_VAL(X,v[0]);							\
	p->SET_SIMPLE_VAL(Y,v[1]);							\
	l->appendItem(p);								\
	for(std::vector<float>::size_type i=2; i<v.size();) {				\
	  following_class* p = new following_class;					\
	  p->SET_SIMPLE_VAL(X,v[i++]);							\
	  p->SET_SIMPLE_VAL(Y,v[i++]);							\
	  l->appendItem(p);								\
	}										\
	beg = end;									\
      }											\
      break;

    CMD_FLOATLIST(SVGPathSegMovetoAbs, SVGPathSegLinetoRel);
    CMD_FLOATLIST(SVGPathSegMovetoRel, SVGPathSegLinetoRel);
    CMD_FLOATLIST(SVGPathSegLinetoAbs, SVGPathSegLinetoAbs);
    CMD_FLOATLIST(SVGPathSegLinetoRel, SVGPathSegLinetoRel);

    CMD_FLOATLIST(SVGPathSegCurvetoQuadraticSmoothAbs, SVGPathSegCurvetoQuadraticSmoothAbs);
    CMD_FLOATLIST(SVGPathSegCurvetoQuadraticSmoothRel, SVGPathSegCurvetoQuadraticSmoothRel);


#undef CMD_FLOATLIST

#define CMD_FLOATLIST(cmd_class, following_class, coord) \
      case cmd_class##_char: {\
	++beg;\
	unicode::String::char_type end = unicode::find_among(s, all_commands, beg);\
	beg = unicode::eat_whitespaces(s, beg, end);\
	std::vector<float> v;\
	if( !unicode::get_float_list(s, &v, beg, end)) { PARSEDBG; }\
        const std::vector<float>::size_type dim=1; \
	if ( (v.size()<dim) ) { PARSEDBG; }\
     \
	cmd_class* p = new cmd_class;\
	p->dom_get##coord().setValue(v[0]);\
	l->appendItem(p);\
	for(std::vector<float>::size_type i=1; i<v.size();) {\
	  following_class* p = new following_class;\
	  p->dom_get##coord().setValue(v[i++]);\
	  l->appendItem(p);\
	}\
	beg = end;\
      }\
      break;

    CMD_FLOATLIST(SVGPathSegLinetoHorizontalAbs, SVGPathSegLinetoHorizontalAbs, X);
    CMD_FLOATLIST(SVGPathSegLinetoHorizontalRel, SVGPathSegLinetoHorizontalRel, X);
    CMD_FLOATLIST(SVGPathSegLinetoVerticalAbs, SVGPathSegLinetoVerticalAbs, Y);
    CMD_FLOATLIST(SVGPathSegLinetoVerticalRel, SVGPathSegLinetoVerticalRel, Y);

#undef CMD_FLOATLIST

#define CMD_FLOATLIST(cmd_class) \
      case cmd_class##_char: {\
	++beg;\
	unicode::String::char_type end = unicode::find_among(s, all_commands, beg);\
	beg = unicode::eat_whitespaces(s, beg, end);\
	std::vector<float> v;\
	if( !unicode::get_float_list(s, &v, beg, end)) { PARSEDBG; }\
        const std::vector<float>::size_type dim=6; \
	if ( (v.size()<dim) || ( v.size() % dim ) ) { PARSEDBG; }\
     \
	for(std::vector<float>::size_type i=0; i<v.size(); ) {\
	  cmd_class* p = new cmd_class;\
	  p->SET_SIMPLE_VAL(X1,v[i++]);\
	  p->SET_SIMPLE_VAL(Y1,v[i++]);\
	  p->SET_SIMPLE_VAL(X2,v[i++]);\
	  p->SET_SIMPLE_VAL(Y2,v[i++]);\
	  p->SET_SIMPLE_VAL(X,v[i++]);\
	  p->SET_SIMPLE_VAL(Y,v[i++]);\
	  l->appendItem(p);\
	}\
	beg = end;\
      }\
      break;

    CMD_FLOATLIST(SVGPathSegCurvetoCubicAbs);
    CMD_FLOATLIST(SVGPathSegCurvetoCubicRel);

#undef CMD_FLOATLIST

#define CMD_FLOATLIST(cmd_class) \
      case cmd_class##_char: {\
	++beg;\
	unicode::String::char_type end = unicode::find_among(s, all_commands, beg);\
	beg = unicode::eat_whitespaces(s, beg, end);\
	std::vector<float> v;\
	if( !unicode::get_float_list(s, &v, beg, end)) { PARSEDBG; }\
        const std::vector<float>::size_type dim=4; \
	if ( (v.size()<dim) || ( v.size() % dim ) ) { PARSEDBG; }\
     \
	for(std::vector<float>::size_type i=0; i<v.size(); ) {\
	  cmd_class* p = new cmd_class;\
	  p->SET_SIMPLE_VAL(X2,v[i++]);\
	  p->SET_SIMPLE_VAL(Y2,v[i++]);\
	  p->SET_SIMPLE_VAL(X,v[i++]);\
	  p->SET_SIMPLE_VAL(Y,v[i++]);\
	  l->appendItem(p);\
	}\
	beg = end;\
      }\
      break;

    CMD_FLOATLIST(SVGPathSegCurvetoCubicSmoothAbs);
    CMD_FLOATLIST(SVGPathSegCurvetoCubicSmoothRel);

#undef CMD_FLOATLIST

#define CMD_FLOATLIST(cmd_class) \
      case cmd_class##_char: {\
	++beg;\
	unicode::String::char_type end = unicode::find_among(s, all_commands, beg);\
	beg = unicode::eat_whitespaces(s, beg, end);\
	std::vector<float> v;\
	if( !unicode::get_float_list(s, &v, beg, end)) { PARSEDBG; }\
        const std::vector<float>::size_type dim=4; \
	if ( (v.size()<dim) || ( v.size() % dim ) ) { PARSEDBG; }\
     \
	for(std::vector<float>::size_type i=0; i<v.size(); ) {\
	  cmd_class* p = new cmd_class;\
	  p->SET_SIMPLE_VAL(X1,v[i++]);\
	  p->SET_SIMPLE_VAL(Y1,v[i++]);\
	  p->SET_SIMPLE_VAL(X,v[i++]);\
	  p->SET_SIMPLE_VAL(Y,v[i++]);\
	  l->appendItem(p);\
	}\
	beg = end;\
      }\
      break;

    CMD_FLOATLIST(SVGPathSegCurvetoQuadraticAbs);
    CMD_FLOATLIST(SVGPathSegCurvetoQuadraticRel);

#undef CMD_FLOATLIST

#define CMD_FLOATLIST(cmd_class) \
      case cmd_class##_char: {\
	++beg;\
	unicode::String::char_type end = unicode::find_among(s, all_commands, beg);\
	beg = unicode::eat_whitespaces(s, beg, end);\
	std::vector<float> v;\
	if( !unicode::get_float_list(s, &v, beg, end)) { PARSEDBG; }\
        const std::vector<float>::size_type dim=7; \
	if ( (v.size()<dim) || ( v.size() % dim ) ) { PARSEDBG; }\
     \
	for(std::vector<float>::size_type i=0; i<v.size(); ) {\
	  cmd_class* p = new cmd_class;\
	  p->SET_SIMPLE_VAL(R1,v[i++]);\
	  p->SET_SIMPLE_VAL(R2,v[i++]);\
	  p->SET_SIMPLE_VAL(Angle,v[i++]);\
          bool b=false; \
          if(v[i++]) b=true; \
	  p->SET_SIMPLE_VAL(LargeArcFlag,b);\
          b=false; \
          if(v[i++]) b=true; \
	  p->SET_SIMPLE_VAL(SweepFlag,b);\
	  p->SET_SIMPLE_VAL(X,v[i++]);\
	  p->SET_SIMPLE_VAL(Y,v[i++]);\
	  l->appendItem(p);\
	}\
	beg = end;\
      }\
      break;

    CMD_FLOATLIST(SVGPathSegArcAbs);
    CMD_FLOATLIST(SVGPathSegArcRel);

#undef CMD_FLOATLIST
#undef PARSEDBG

      case SVGPathSegClosePath_char:
      case SVGPathSegClosePath_char2: {
	SVGPathSegClosePath* p = new SVGPathSegClosePath;
	l->appendItem(p);
	beg = unicode::find_among(s, all_commands, beg+1);
      }
      break;

      default:
	std::cerr << "unknown path command '" << s[beg] << "'" __FILE__ ":" << __LINE__ << std::endl;
	return;
	//	break;
      }
    }
  }

}
