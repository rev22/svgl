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
#include <w3c/svg/SVGTextContentElement.hpp>
#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <glft/FontManager.hpp>

#include <w3c/css/CSSStyleDeclaration.hpp>
#include <w3c/css/RGBColor.hpp>
#include <w3c/css/URIColor.hpp>

#include <string>

#include <svgl/debug.hpp>

namespace glft {
  extern void lower(std::string& s);
}

namespace svg {

	
	void
	SVGTextContentElement::computeGeometryForDrawing(svgl::Context * svglContext, svgl::GLInfo* glinfo)
	{
	}

  void
  SVGTextContentElement::setFont(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo, glft::FontManager::font_type t)
  {
    setFont(svglContext, glinfo, textinfo);
    svglContext->fontManager->setFontType(t);
  }

#if 0
  void
  setFontFileInfo(svgl::FontFileInfo& f, unicode::String* fontFamily, unicode::String* fontStyle, unicode::String* fontWeight)
  {
    f.fontFamily = fontFamily->transcode();
    svgl::lower(f.fontFamily);

    std::string fs = fontStyle->transcode();
    std::string fw = fontWeight->transcode();

    svgl::lower(fs);
    svgl::lower(fw);
    if(fs.find("italic")!=std::string::npos) {
      f.fontStyle=svgl::FontFileInfo::fs_italic;
    }

    if(fs.find("oblique")!=std::string::npos) {
      f.fontStyle=svgl::FontFileInfo::fs_oblique;
    }

    if(fw.find("bold")!=std::string::npos) {
      f.fontWeight=svgl::FontFileInfo::fw_bold;
    }
  }
#else
  void
  setFontInfo(glft::FontInfo& f, css::CSSStyle::FontFamilyType fontFamily, css::CSSStyle::FontStyleType fontStyle, css::CSSStyle::FontWeightType fontWeight)
  {
    f.family = fontFamily->transcode();
    glft::lower(f.family);

    f.style = fontStyle;
    f.weight = fontWeight;
  }

#endif

  void
  SVGTextContentElement::setFont(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo*)
  {
    const css::CSSStyleDeclaration& style = getStyle().getValue();
    //const css::Color * fill = style.getFill(); // test if it's a gradient...
    css::CSSStyle::FontFamilyType fontfamily = style.getFontFamily();
    css::CSSStyle::FontStyleType fontstyle = style.getFontStyle();
    css::CSSStyle::FontWeightType fontweight = style.getFontWeight();

    css::CSSStyle::FontSizeType sizel = style.getFontSize();
    unsigned int size = (unsigned int) svglContext->computeWidthLength(sizel);

    //    int bbx=0,bby=0,width=0,height=0;

    //std::cerr << fontfamily << __FL__;

#if 1

    glft::FontInfo f;
    //std::cerr << DBGVAR(fontfamily) << DBGVAR(fontstyle) << DBGVAR(fontweight) << __FL__;
    setFontInfo(f, fontfamily, fontstyle, fontweight);
    //std::cerr << f.family << " " << glft::FontInfo::styleDesc[f.style] << " " << glft::FontInfo::weightDesc[f.weight] << __FL__;
    svglContext->fontManager->setCurrentFont(f, size);
    svglContext->fontSize=size;

#if 0    
    const css::URIColor * gradColor = dynamic_cast<const css::URIColor *>(fill);
    if(!gradColor) {
      svglContext->fontManager->setAutoScale(true);
    } else {
      svglContext->fontManager->setAutoScale(false);
      svglContext->fontManager->setFontType(svgl::FontManager::vector);
    }
#endif

#else
    // if size if small and this is not a gradient
    svgl::FontInfo f;
    setFontInfo(f, fontfamily, fontstyle, fontweight);
    svglContext->fontManager->setCurrentFont(f, size);
    svglContext->fontSize=size;

    if(size<20 && !gradColor) {
      svglContext->fontManager->setFontType(svgl::FontManager::texture);
    }
    else {
      svglContext->fontManager->setFontType(svgl::FontManager::vector);
    }
    //svglContext->fontManager->setCurrentFont(unicode::String::internString(fontname), static_cast<int>(size));
#endif
  }

long
SVGTextContentElement::getNumberOfChars (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

float
SVGTextContentElement::getComputedTextLength (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

float
SVGTextContentElement::getSubStringLength ( unsigned long charnum, unsigned long nchars )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPoint*
SVGTextContentElement::getStartPositionOfChar ( unsigned long charnum )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPoint*
SVGTextContentElement::getEndPositionOfChar ( unsigned long charnum )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}
SVGRect*
SVGTextContentElement::getExtentOfChar ( unsigned long charnum )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

float
SVGTextContentElement::getRotationOfChar ( unsigned long charnum )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

long
SVGTextContentElement::getCharNumAtPosition ( const SVGPoint& point )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGTextContentElement::selectSubString ( unsigned long charnum, unsigned long nchars )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}


}
