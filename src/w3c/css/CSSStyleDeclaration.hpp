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
#ifndef __css_StyleDeclaration__
#define __css_StyleDeclaration__

#include <svgl/config.hpp>
#include <w3c/svg/SVGLength.hpp>
#include <svgl/AnimateAdapter.hpp>
#include <utils/String.hpp>
#include <w3c/css/CSSURI.hpp>
#include <w3c/css/TextAnchor.hpp>
#include <glft/FontInfo.hpp>
#include <vector>


namespace css {

  class CSSStyleDeclaration;
  //class RGBColor;
  class Color;
  class CSSStyle;
  class CSSOnePatchStyle;
  class PlainStyle;

  class CSSStyle {
  public:

    /*
    struct __ClipPath {
      virtual ~__ClipPath() {}
    };
    struct __URIClipPath : __ClipPath {
      dom::string uri;
    };
    */

    //typedef RGBColor* PaintType;
    //typedef const RGBColor* const_PaintType;
    typedef Color* PaintType;
    typedef const Color* const_PaintType;
    typedef float OpacityType;
    typedef svg::SVGLength StrokeWidthType;
		typedef svg::SVGLength LengthType;
		typedef float FloatType;
	  typedef unicode::String* StringType;
    typedef unicode::String* FontFamilyType;
    typedef svg::SVGLength FontSizeType;
    //typedef unicode::String* FontStyleType;
    //typedef unicode::String* FontWeightType;
    typedef glft::FontInfo::Style FontStyleType;
    typedef glft::FontInfo::Weight FontWeightType;
    typedef css::URI UriType;
    typedef bool VisibilityType;
    typedef bool DisplayType;
    typedef css::TextAnchor TextAnchorType;
		typedef std::vector<float> StrokeDasharrayType;
//		typedef svg::SVGLength LengthType;
    //typedef unicode::String* TextAnchorType;
    //typedef __ClipPath* ClipPathType;

    static void init();
    static CSSStyle* getDefaultStyle() { return defaultStyle; }
    static CSSOnePatchStyle* newOnePatchStyle(
	    unicode::String* attr, unicode::String* value, const CSSStyle* next, const CSSStyle* parent);
    static CSSOnePatchStyle* newPatchedStyles(unicode::String* cssstyle, const CSSStyle* next, const CSSStyle* parent);

    // style attributes
#define ATTRDEF(type, name, dtdstr) \
    virtual type get##name() const=0; \
    virtual const CSSOnePatchStyle* findOnePatch##name() const=0;
#include <w3c/css/style_defs.hpp>
#undef ATTRDEF

    virtual const CSSStyle* getNext() const { return 0; }
    virtual void setNext(CSSStyle*) {}

  private:
    static CSSStyle* defaultStyle;
  };

  class PlainStyle : public CSSStyle {
  public:
    PlainStyle(
#define ATTRDEF(type, name, dtdstr) type name,
#include <w3c/css/style_defs.hpp>
#undef ATTRDEF
	       bool foo=false
	       );

    virtual ~PlainStyle() {}

    // style attributes
#define ATTRDEF(type, name, dtdstr) \
    virtual type get##name() const { return _##name; } \
    virtual const CSSOnePatchStyle* findOnePatch##name() const { return 0; }
#include <w3c/css/style_defs.hpp>
#undef ATTRDEF


  private:
    // style attributes
#define ATTRDEF(type, name, dtdstr) type _##name;
#include <w3c/css/style_defs.hpp>
#undef ATTRDEF

    bool _foo;
  };

  class CSSOnePatchStyle : public CSSStyle {
  public:
    CSSOnePatchStyle(const CSSStyle* next) : _next(next) {}
    virtual ~CSSOnePatchStyle() {}
    // style attributes
#define ATTRDEF(type, name, dtdstr) \
    virtual type get##name() const { if(_next) return _next->get##name(); else return CSSStyle::getDefaultStyle()->get##name(); } \
    virtual const CSSOnePatchStyle*  findOnePatch##name() const { if(_next) return _next->findOnePatch##name(); else return 0; }
#include <w3c/css/style_defs.hpp>
#undef ATTRDEF

    virtual const CSSStyle* getNext() const { return _next; }
    virtual void setNext(const CSSStyle* s) { _next = s; }

  protected:
    const CSSStyle* _next;
  };


  // By type for animation...
#define ATTRDEF(type, name, dtdstr) \
  struct CSS##type##Style : CSSOnePatchStyle { \
    CSS##type##Style(const CSSStyle* next, type n) : CSSOnePatchStyle(next), _##type(n) {} \
    /*type& get##type() { return _##type; }*/ \
    /*const type& get##type() const { return _##type; }*/ \
    void set(type t) { _##type = t; } \
    const type& get() const { return _##type; } \
    type& get() { return _##type; } \
  protected: \
    type _##type; \
  };

  // only the types are considered here

ATTRDEF(const_PaintType,StopColor,"stop-color")
ATTRDEF(OpacityType,Opacity,"opacity")
  //ATTRDEF(ClipPathType,ClipPath,"clip-path")
ATTRDEF(UriType,Filter,"filter")
ATTRDEF(StrokeWidthType,StrokeWidth,"stroke-width")
ATTRDEF(FontFamilyType,FontFamily,"font-family")
ATTRDEF(FontSizeType,FontSize,"font-size")
ATTRDEF(FontStyleType,FontStyle,"font-style")
ATTRDEF(FontWeightType,FontWeight,"font-weight")
ATTRDEF(VisibilityType,Visibility,"visibility")
ATTRDEF(DisplayType,Display,"display")
ATTRDEF(TextAnchorType,TextAnchor,"text-anchor")
ATTRDEF(LengthType,_,_)
ATTRDEF(FloatType,_,_)
ATTRDEF(StringType,_,_)
ATTRDEF(StrokeDasharrayType,_,_)

	
#undef ATTRDEF

  // Patched styles

#define ATTRDEF(type, name, dtdstr) \
  struct name##Style : CSS##type##Style { \
    name##Style(const CSSStyle* next, type name) : CSS##type##Style(next,name) {} \
    virtual type  get##name() const { return _##type; } \
    virtual const CSSOnePatchStyle * findOnePatch##name() const { return this; } \
  };

#include <w3c/css/style_defs.hpp>
#undef ATTRDEF

#if 0  
  struct PaintTypeAdapter {
    struct exc {};
    PaintType getColor() { throw NotAPaintTypeStyle(); }
  };

  template <class X>
  struct PaintTypeAdapterTemplate : public PaintTypeAdapter {
    PaintTypeAdapter(X* x) : _paintStyle(x) {}
    X* _paintStyle;
    PaintType getColor() { return _paintStyle->_PaintType; }
    void setColor(CSSStyle::PaintType c) { _paintStyle=c; }
  };
#endif
}

namespace svgl {
  typedef AnimateAdapterTemplate<css::CSSconst_PaintTypeStyle> AnimateAdapterColor;
}


namespace css {

  class CSSStyleDeclaration {
  public:

    CSSStyleDeclaration() : _style( 0 ), _style_string(0), _parent(0) {}
    virtual ~CSSStyleDeclaration() {}

    void updateStyle(const CSSStyleDeclaration* parent);

    //    const CSSStyle* getStyle() { return _style; }
    const CSSStyle* getStyle() const { return _style; }
    //CSSStyle* getStyle() { return _style; }
    void setStyle(CSSStyle * s) { _style = s; }

    const CSSStyleDeclaration* getParent() { return _parent; }
    void setParent(const CSSStyleDeclaration* parent) { _parent=parent; }

#if 0
    template<class FooStyle>
    svgl::AnimateAdapter* getNewPatchStyle(const dom::string& attrName) { return 0; }
    template<>
#endif
    svgl::AnimateAdapterColor* getNewPatchStyle_const_PaintType(unicode::String* attrName);



    void setStyleString(unicode::String* s) { _style_string = s; }
    void appendStyleString(unicode::String* s) { _style_string = concat(_style_string, s); }
    const unicode::String* getStyleString() const { return _style_string; }
    static bool isStyleAttribute(unicode::String*);


    // style attributes (not inherited)
#define ATTRDEF(type, name, dtdstr) \
    virtual CSSStyle::type get##name() const; // { return _style->get##name(); }
#include <w3c/css/not_inherited_style_defs.hpp>
#undef ATTRDEF

  // parents inherited attributes
#define ATTRDEF(type, name, dtdstr)   \
virtual CSSStyle::type get##name() const;
#include <w3c/css/inherited_style_defs.hpp>
#undef ATTRDEF


  // By type for animation...
#define ATTRDEF(type, name, dtdstr) \
virtual CSSStyle::type get##type(unicode::String* attrName);

  // only the types are considered here

ATTRDEF(const_PaintType,StopColor,"stop-color")
  //ATTRDEF(OpacityType,Opacity,"opacity")
  //ATTRDEF(ClipPathType,ClipPath,"clip-path")
  //ATTRDEF(UriType,Filter,"filter")
  //ATTRDEF(StrokeWidthType,StrokeWidth,"stroke-width")
  //ATTRDEF(FontFamilyType,FontFamily,"font-family")
  //ATTRDEF(FontSizeType,FontSize,"font-size")
  //ATTRDEF(FontStyleType,FontStyle,"font-style")
  //ATTRDEF(FontWeightType,FontWeight,"font-weight")
  //ATTRDEF(VisibilityType,Visibility,"visibility")
  //ATTRDEF(DisplayType,Display,"display")
  //ATTRDEF(TextAnchorType,TextAnchor,"text-anchor")

#undef ATTRDEF


  private:
    const CSSStyle* _style;
    unicode::String* _style_string;
    const CSSStyleDeclaration* _parent;

  };


}



#endif // __css_StyleDeclaration__
