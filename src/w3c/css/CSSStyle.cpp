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
#include <w3c/css/CSSStyleDeclaration.hpp>
#include <w3c/css/RGBColor.hpp>
#include <w3c/css/TextAnchor.hpp>
#include <utils/String.hpp>
#include <svgl/parseHelper.hpp>
#include <w3c/svg/SVGLength.hpp>
#include <w3c/css/CSSURI.hpp>
#include <svgl/AnimateAdapterColor.hpp>

#include <svgl/debug.hpp>

namespace svg {
  extern ::css::Color * currentColor;
  extern ::css::Color * inheritColor;

  extern
  bool
  getRGBColor(const unicode::String* s, css::Color**, unicode::String::size_type beg=0, unicode::String::size_type end=unicode::String::npos);

  extern unicode::String * fill_attribute_string, * stroke_attribute_string, * color_attribute_string;

}

#define DBG std::cerr << __FILE__ << ":" << __LINE__ << std::endl;

namespace css {

  TextAnchor
  makeTextAnchor(dom::String* value)
  {
    static const unicode::String
      *start_string=unicode::String::internString("start"),
      *middle_string=unicode::String::internString("middle"),
      *end_string=unicode::String::internString("end"),
      *inherit_string=unicode::String::internString("inherit");

    TextAnchor t(TextAnchor::CSS_TEXTANCHOR_UNKNOWN);
    if(!value)
      return t;
    if( *value==*start_string ) {
      t.value = TextAnchor::CSS_TEXTANCHOR_START;
    } else
    if( *value==*middle_string ) {
      t.value = TextAnchor::CSS_TEXTANCHOR_MIDDLE;
    } else
    if( *value==*end_string ) {
      t.value = TextAnchor::CSS_TEXTANCHOR_END;
    } else
    if( *value==*inherit_string ) {
      t.value = TextAnchor::CSS_TEXTANCHOR_INHERIT;
    }
    return t;
  }


CSSStyle* CSSStyle::defaultStyle;

void
CSSStyle::init()
{
  css::Color::initNoneColor();

  CSSStyle::PaintType fill = 0;
  {
    RGBColor *c = new RGBColor;
    c->r = c->g= c->b=0;
    fill = c; // or None ? check it...
  }

  CSSStyle::PaintType color = 0;

  CSSStyle::PaintType stroke=0;
  CSSStyle::PaintType stopcolor=0;

  svg::SVGLength l;
  l.dom_setValueAsString(unicode::String::internString("1"));
  CSSStyle::StrokeWidthType strokeWidth = l;

  CSSStyle::OpacityType opacity=1, fill_opacity=1, stroke_opacity=1, stop_opacity=1;

  //  CSSStyle::ClipPathType clippath=0;

  CSSStyle::FontFamilyType fontfamily = unicode::String::internString("Helvetica");
  l.dom_setValueAsString(unicode::String::internString("12"));
  CSSStyle::FontSizeType fontsize=l;
  //CSSStyle::FontStyleType fontstyle =  unicode::String::internString("normal");
  //CSSStyle::FontWeightType fontweight =  unicode::String::internString("normal");
  CSSStyle::FontStyleType fontstyle =  glft::FontInfo::fs_normal;
  CSSStyle::FontWeightType fontweight =  glft::FontInfo::fw_normal;
  //  CSSStyle::TextDecorationType fontweight = "normal";
  css::URI uri; // filter, marker
  css::URI &clippathuri=uri, &filteruri=uri, &markeruri=uri, &markerStartUri=uri, &markerEndUri=uri, &markerMidUri=uri;
  CSSStyle::VisibilityType visibility=true;
  CSSStyle::DisplayType display=true;

  CSSStyle::TextAnchorType textAnchor(css::TextAnchor::CSS_TEXTANCHOR_START);
	
	
	CSSStyle::StringType
		strokeLinecap = unicode::String::internString("butt"),
		strokeLinejoin = unicode::String::internString("mitter");

	CSSStyle::FloatType strokeMiterLimit = 4;
	CSSStyle::StrokeDasharrayType strokeDasharray;
	l.dom_setValueAsString(unicode::String::internString("0"));
	CSSStyle::LengthType strokeDashoffset = l;
		

  defaultStyle = new PlainStyle(
				color, fill, stroke, stopcolor,
				strokeWidth,
				opacity, fill_opacity, stroke_opacity, stop_opacity,
				clippathuri,
				fontfamily,
				fontsize,
				fontstyle,
				fontweight,
				filteruri,
				markeruri,
				markerStartUri,
				markerEndUri,
				markerMidUri,
				visibility,
				display,
				textAnchor,
				strokeLinecap,
				strokeLinejoin,
				strokeMiterLimit,
				strokeDasharray,
				strokeDashoffset
				);
}

PlainStyle::PlainStyle(
#define ATTRDEF(type, name, dtdstr) type name,
#include <w3c/css/style_defs.hpp>
#undef ATTRDEF
	       bool foo
)
:
#define ATTRDEF(type, name, dtdstr) _##name(name),
#include <w3c/css/style_defs.hpp>
#undef ATTRDEF
_foo(foo)
{
}

CSSOnePatchStyle*
CSSStyle::newOnePatchStyle(unicode::String* attr_ptr, unicode::String* value, const CSSStyle* next, const CSSStyle* parent)
{
  if(!attr_ptr || !value)
    return 0;
  const unicode::String& attr = *attr_ptr;
#define ATTRDEF(type, name, dtdstr) static const unicode::String& name##_string = *unicode::String::internString(dtdstr);
#include <w3c/css/style_defs.hpp>
#undef ATTRDEF
  static const unicode::String
    *none_string=unicode::String::internString("none"),
    *hidden_string=unicode::String::internString("hidden");

  CSSOnePatchStyle* res=0;

  // color type

#define ATTRDEF(type,name,dtdstr) \
  if (attr==name##_string) { \
    Color* color; \
    if (svg::getRGBColor(value, &color)) { \
      if (color==svg::currentColor) { \
	if (next) {\
          const Color* color; \
          const CSSOnePatchStyle* ops = next->findOnePatchColor(); \
          if(ops) \
             color = ops->getColor(); \
          else \
             color = parent->getColor(); \
          res = new name##Style(next, color); \
        } \
	else { \
	  res = 0; \
        } \
      } \
      else if (color==svg::inheritColor) { \
	if(parent) {\
          const Color* color; \
	  color = parent->get##name(); \
          res = new name##Style(next, color); \
        } \
	else { \
	  res = 0; \
        } \
      } \
      else \
	res = new name##Style(next, color); \
    } \
  } else

ATTRDEF(const_PaintType,Fill,"fill")
ATTRDEF(const_PaintType,Stroke,"stroke")
ATTRDEF(const_PaintType,StopColor,"stop-color")
ATTRDEF(const_PaintType,Color,"color")
#undef ATTRDEF

  // length type

	
	
#define ATTRDEF(type,name,dtdstr) \
    if (attr==name##_string) { \
      svg::SVGLength l; \
      svgl::read(value, l); \
      res = new name##Style(next, l); \
    } else
ATTRDEF(StrokeWidthType,StrokeWidth,"stroke-width")
ATTRDEF(FontSizeType,FontSize,"font-size")
ATTRDEF(svg::SVGLength,StrokeDashoffset,"stroke-dashoffset")
#undef ATTRDEF

  // float

#define ATTRDEF(type,name,dtdstr) \
  if (attr==name##_string) { \
    type v; \
    if (unicode::get_float(*value, &v)) { \
      res = new name##Style(next, v); \
    } \
  } else

ATTRDEF(OpacityType,Opacity,"opacity")
ATTRDEF(OpacityType,FillOpacity,"fill-opacity")
ATTRDEF(OpacityType,StrokeOpacity,"stroke-opacity")
ATTRDEF(OpacityType,StopOpacity,"stop-opacity")
ATTRDEF(float,StrokeMiterlimit,"stroke-miterlimit")
#undef ATTRDEF

  // string
#define ATTRDEF(type,name,dtdstr) \
    if (attr==name##_string) { \
       res = new name##Style(next, value); \
    } else
ATTRDEF(FontFamilyType,FontFamily,"font-family")
ATTRDEF(StringType,StrokeLinecap,"stroke-linecap")
ATTRDEF(StringType,StrokeLinejoin,"stroke-linejoin")	
#undef ATTRDEF

  if (attr==FontStyle_string) {
    std::string v = value->transcode();
    FontStyleType t = glft::FontInfo::getStyle(v);
    res = new FontStyleStyle(next, t);
  } else
  if (attr==FontWeight_string) {
    std::string v = value->transcode();
    FontWeightType t = glft::FontInfo::getWeight(v);
    res = new FontWeightStyle(next, t);
  } else

  // Uri
#define ATTRDEF(type,name,dtdstr) \
    if (attr==name##_string) { \
       css::URI uri; \
       get_css_uri(value, &uri); \
       res = new name##Style(next, uri); \
    } else
ATTRDEF(UriType,ClipPath,"clip-path")
ATTRDEF(UriType,Filter,"filter")
ATTRDEF(UriType,Marker,"marker")
ATTRDEF(UriType,MarkerStart,"marker-start")
ATTRDEF(UriType,MarkerEnd,"marker-end")
ATTRDEF(UriType,MarkerMid,"marker-mid")
#undef ATTRDEF

  if(attr==Visibility_string) {
    bool vis=true;
    if(*value==*hidden_string)
      vis=false;
    res = new VisibilityStyle(next, vis);
  }
  else 
  if(attr==Display_string) {
    bool vis=true;
    if(value && *value==*none_string) {
      vis=false;
    }
    res = new DisplayStyle(next, vis);
  }
  else 

  if(attr==TextAnchor_string) {
    TextAnchor t = makeTextAnchor(value);
    if(t.value != TextAnchor::CSS_TEXTANCHOR_START) {
      res = new TextAnchorStyle(next, t);
    }
  }
  else

#if 0
#define DBGMSG std::cerr << "invalid clip path " << value << std::endl;
  if (attr==ClipPath_string) {
    if (value==none_string)
      res=0;
    else if (value.size() > 3 && value.substr(0, 3)=="url") {
      unicode::String::size_type beg = value.find(left_parenthesis, 3);
      if( beg==unicode::String::npos ) {
	DBGMSG;
	res = 0;
      }
      beg+=1;
      unicode::String::size_type pos = value.find(right_parenthesis, beg);
      if( pos==unicode::String::npos ) {
	DBGMSG;
	res = 0;
      }
      __URIClipPath* cp = new __URIClipPath;
      cp->uri = value.substr(beg, pos);
      res = new ClipPathStyle(next, cp);
    }
  } else
#undef DBGMSG
#endif
		
	if(attr==StrokeDasharray_string) {
		//ATTRDEF(StrokeDashArrayType,StrokeDasharray,"stroke-dasharray")
		StrokeDasharrayType * da = new StrokeDasharrayType;
		unicode::get_float_list(*value, da);
		res = new StrokeDasharrayStyle(next, *da);
		
	} else
		

  {
    //    std::cerr << "unrecognized " << attr << ":" << value << std::endl;
  }
  return res;
}

//template<>
svgl::AnimateAdapterColor*
CSSStyleDeclaration::getNewPatchStyle_const_PaintType(unicode::String* attrName_ptr)
{
  unicode::String& attrName = *attrName_ptr;
  static unicode::String& black = *unicode::String::internString("rgb(0,0,0)");
  // FIXME
  CSSOnePatchStyle* res = CSSStyle::newOnePatchStyle(&attrName, &black, _style, 0);// O is dangerous, but we don't know the parent...
  CSSconst_PaintTypeStyle* res2 = dynamic_cast<CSSconst_PaintTypeStyle*>(res);

  if(res2) {
    _style = res2;
    //return new svgl::AnimateAdapterTemplate<CSSconst_PaintTypeStyle>(res2);
    return new svgl::AnimateAdapterColor(res2);
  }
  else {
    //return new svgl::AnimateAdapterTemplate<CSSOnePatchStyle>(res);
    return 0;
  }
}



CSSOnePatchStyle*
CSSStyle::newPatchedStyles(unicode::String* cssstyle_ptr, const CSSStyle* next, const CSSStyle* parent)
{
  const unicode::String& cssstyle = *cssstyle_ptr;
  unicode::String::size_type len = cssstyle.getLength();
  unicode::String::size_type beg=0, pos=0;
  static const unicode::String::char_type semicolon = ';';
  static const unicode::String::char_type doublecolon = ':';

  //std::cerr << "Processing \"" << cssstyle << "\"" << __FL__;

  CSSOnePatchStyle *res=0;
  beg = unicode::eat_whitespaces(cssstyle, beg);
  while (beg<len) {
    pos = cssstyle.indexOf(semicolon, beg);
    if (pos==unicode::String::npos) {
      pos=len; // because it should be the pos of ';'
    }
    {
      unicode::String::size_type mid = cssstyle.indexOf(doublecolon, beg);
      if (mid>pos) {
	std::cerr << "error in style declaration: " << cssstyle.substring(beg, pos) << " " __FILE__ ":" << __LINE__ << std::endl;
	break;
      }
      else {
	unicode::String::size_type beg_attr, end_attr, beg_val, end_val;
	beg_attr = beg;
	end_attr = unicode::eat_whitespaces_reverse(cssstyle, mid);
	beg_val = unicode::eat_whitespaces(cssstyle, mid+1);
	end_val = unicode::eat_whitespaces_reverse(cssstyle, pos);

#if 0	
	std::cerr << DBGVAR(len) << DBGVAR(pos) <<  DBGVAR(beg_attr) << DBGVAR(end_attr) << DBGVAR(beg_val) << DBGVAR(end_val) << __FL__;
	std::cerr << cssstyle.substring(beg_attr, end_attr) << __FL__;
	std::cerr << cssstyle.substring(beg_val, end_val) << __FL__;
#endif
	CSSOnePatchStyle * p = CSSStyle::newOnePatchStyle(cssstyle.substring(beg_attr, end_attr), cssstyle.substring(beg_val, end_val), next, parent);
	if(p)
	  next=res=p;
	if(pos==len)
	  beg=len;
	else
	  beg = unicode::eat_whitespaces(cssstyle, pos+1);
      }
    }
  }

  return res;
}


// non inherited attributes

void
CSSStyleDeclaration::updateStyle(const CSSStyleDeclaration* parent)
{
  _parent=parent;
  _style=CSSStyle::getDefaultStyle();
  const CSSStyle * parentStyle = (parent ? parent->getStyle() : _style);

  //std::cerr << DBGVAR(parentStyle) << __FL__;

  if (_style_string && _style_string->getLength()) {
    //std::cerr << DBGVAR(_style_string) << __FL__;
    _style = CSSStyle::newPatchedStyles(_style_string, CSSStyle::getDefaultStyle(), parentStyle);
    if(!_style)
      _style = CSSStyle::getDefaultStyle();
    //std::cerr << DBGVAR(_style) << __FL__;
  }
}

// parents inherited attributes

#define ATTRDEF(type, name, dtdstr)			\
CSSStyle::type						\
CSSStyleDeclaration::get##name() const {		\
  const CSSOnePatchStyle* p = 0;			\
  if(_style) {						\
     p = _style->findOnePatch##name();			\
  }							\
  if (p) {						\
    return p->get##name();				\
  } 							\
  if(_parent)						\
    return _parent->get##name();			\
  return CSSStyle::getDefaultStyle()->get##name();	\
}
#include <w3c/css/inherited_style_defs.hpp>
#undef ATTRDEF

// parents non inherited attributes

#define ATTRDEF(type, name, dtdstr)			\
CSSStyle::type						\
CSSStyleDeclaration::get##name() const {		\
  const CSSOnePatchStyle* p = 0;			\
  if(_style) {						\
     p = _style->findOnePatch##name();			\
  }							\
  if (p) {						\
    return p->get##name();				\
  }							\
  return CSSStyle::getDefaultStyle()->get##name();	\
}
#include <w3c/css/not_inherited_style_defs.hpp>
#undef ATTRDEF


bool
CSSStyleDeclaration::isStyleAttribute(unicode::String* s)
{
#define ATTRDEF(type, name, dtdstr) static const unicode::String* name##_string = unicode::String::internString(dtdstr);
#include <w3c/css/style_defs.hpp>
#undef ATTRDEF

#define ATTRDEF(type, name, dtdstr) \
if(name##_string==s) return true; else
#include <w3c/css/style_defs.hpp>
#undef ATTRDEF
  return false;
}

  // By type for animation...
CSSStyle::const_PaintType
CSSStyleDeclaration::getconst_PaintType(unicode::String* attrName)
{
  //std::cerr << attrName << " " << svg::stroke_attribute_string << __FL__;
#define ATTRDEF(strname, name, dtdstr) \
  if(*attrName == *svg::strname##_attribute_string) { \
    return get##name(); \
  } else

ATTRDEF(color,Color,"color")
ATTRDEF(fill,Fill,"fill")
ATTRDEF(stroke,Stroke,"stroke")
ATTRDEF(stop_color,StopColor,"stop-color")

    {
      std::cerr << "not a const_PaintType attribute" << __FL__;
      throw;
    }
}


}



