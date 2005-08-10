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
#include <w3c/svg/SVGTextElement.hpp>
#include <w3c/svg/SVGTextContentElement.hpp>
#include <w3c/svg/SVGSVGElement.hpp>

#include <svgl/InitHelper.hpp>
#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/TextInfo.hpp>
#include <glft/FontManager.hpp>
#include <svgl/BoundingBoxManager.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

#include <w3c/dom/Text.hpp>

#define GLFT_TEXT(x) (new std::string((x)->transcode()))

namespace svg {

  void
  SVGTextElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
		//return;
    svgl::TextInfo textinfo;
    _textinfo=&textinfo;

    svg::SVGLength xl = GETVAL(X);
    svg::SVGLength yl = GETVAL(Y);

    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);
	  
    textinfo.x=x;
    textinfo.y=y;

    //double sx = glinfo->scaleX;
    //double sy = glinfo->scaleY;

    glPushMatrix();

#define DOIT  glTraverse(svglContext, glinfo, false, true);

    for(dom::Node *n = getFirstChild(); n; n=n->getNextSibling()) {
      _currentTextContent = 0;
      _currentPCDATA = dynamic_cast<dom::Text*>(n);
      //std::cerr << DBGVAR(textinfo.x) << DBGVAR(textinfo.y) << __FL__;
      if (_currentPCDATA) {
				glPushMatrix();
				SVGTransformable::glTraverse(svglContext, glinfo);
				//glTranslated(x,y,0);
				SVGStylable::DOIT;
				glPopMatrix();
				//glTraverseBoundingBoxPCDATA(svglContext,glinfo,pcdata);
      }
      else {
				SVGTextContentElement *e = dynamic_cast<SVGTextContentElement*>(n);
				if(e) {
					//std::cerr << "text may not be at the right place since transforms are not traversed" __FL__;
					glPushMatrix();
					SVGTransformable::glTraverse(svglContext, glinfo);
					_currentTextContent = e;
					e->glTextTraverse(svglContext, glinfo, _textinfo);
					glPopMatrix();
					//SVGElement *e2 = dynamic_cast<SVGElement*>(e);
					//e2->DOIT;
				}
      }
    }
#undef DOIT

    glPopMatrix();
    //glinfo->scaleX = sx;
    //glinfo->scaleY = sy;

    _currentPCDATA = 0;
    _currentTextContent = 0;

  }

  void
  SVGTextElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    svgl::TextInfo textinfo;
    _textinfo=&textinfo;

    svg::SVGLength xl = GETVAL(X);
    svg::SVGLength yl = GETVAL(Y);

    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);
	  
    textinfo.x=x;
    textinfo.y=y;

    glPushMatrix();
#define DOIT  pickTraverse(svglContext, glinfo);
    for(dom::Node *n = getFirstChild(); n; n=n->getNextSibling()) {
      _currentPCDATA = dynamic_cast<dom::Text*>(n);
      if (_currentPCDATA) {
	SVGTransformable::glTraverse(svglContext, glinfo);
	SVGStylable::DOIT;
	//glTraverseBoundingBoxPCDATA(svglContext,glinfo,pcdata);
      }
      else {
	SVGTextContentElement *e = dynamic_cast<SVGTextContentElement*>(n);
	if(e) {
	  //std::cerr << "text may not be at the right place since transfors are not traverse " << __FL__;
	  _currentTextContent = e;
	  svg::SVGLength xl = GETVAL(X);
	  svg::SVGLength yl = GETVAL(Y);

	  float x = svglContext->computeWidthLength(xl);
	  float y = svglContext->computeHeightLength(yl);
	  
	  textinfo.dx=x;
	  textinfo.dy=y;

	  SVGElement *e2 = dynamic_cast<SVGElement*>(e);
	  e2->DOIT;
	}
      }
    }
#undef DOIT


    glPopMatrix();
    _currentPCDATA = 0;
    _currentTextContent = 0;

  }

  void
  SVGTextElement::glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    //STDDBG;
    svgl::TextInfo textinfo;
    _textinfo=&textinfo;

    svg::SVGLength xl = GETVAL(X);
    svg::SVGLength yl = GETVAL(Y);

    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);
	  
    textinfo.dx=x;
    textinfo.dy=y;

    //textinfo.dx=0;
    //textinfo.dy=0;

    glPushMatrix();
    for(dom::Node *n = getFirstChild(); n; n=n->getNextSibling()) {
      _currentTextContent = 0;
      _currentPCDATA = dynamic_cast<dom::Text*>(n);
      if (_currentPCDATA) {
	SVGTransformable::glTraverse(svglContext, glinfo);
	glTraverseBoundingBoxPCDATA(svglContext, glinfo, _currentPCDATA);
      }
      else {
	SVGTextContentElement *e = dynamic_cast<SVGTextContentElement*>(n);
	if(e) {
	  //std::cerr << "text may not be at the right place since transfors are not traverse " << __FL__;
	  _currentTextContent = e;
	  SVGTransformable::glTraverse(svglContext, glinfo);
	  svg::SVGLength xl = GETVAL(X);
	  svg::SVGLength yl = GETVAL(Y);

	  float x = svglContext->computeWidthLength(xl);
	  float y = svglContext->computeHeightLength(yl);
	  
	  textinfo.dx=x;
	  textinfo.dy=y;

	  SVGElement *e2 = dynamic_cast<SVGElement*>(e);
	  e2->glTraverseBoundingBox(svglContext, glinfo);
	}
      }
    }
    glPopMatrix();
  }

  void
  SVGTextElement::getBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo, float* bbx, float* bby, float* bbwidth, float* bbheight)
  {
    // std::cerr << "getBoundingBox is here only for SVGStylable algorithms. To get Text Bounding Box, use glTraverseBoundingBox... " __FL__;
    //STDDBG;  
    setFont(svglContext, glinfo, 0);

    svg::SVGLength xl = GETVAL(X);
    svg::SVGLength yl = GETVAL(Y);
    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);

    if(_currentPCDATA) {
      int x2,y2,width,height;
			double currentScale = glinfo->getCurrentScale();
      svglContext->fontManager->getBoundingBoxWithScale(GLFT_TEXT(_currentPCDATA->getData()), currentScale, &x2, &y2, &width, &height);
			//svglContext->fontManager->getBoundingBox(GLFT_TEXT(_currentPCDATA->getData()), &x2, &y2, &width, &height);
      y2=-(height+y2);
			//std::cerr << _currentPCDATA->getData() << DBGVAR(x) << DBGVAR(y) << DBGVAR(x2) << DBGVAR(y2) << __FL__;
      *bbx=x2+x;
			*bby=y2+y;
      *bbwidth=width;
      *bbheight=height;
    }
    //std::cerr << DBGVAR(*bbx) << DBGVAR(*bby) << DBGVAR(*bbwidth) << DBGVAR(*bbheight) << __FL__;
  }

  void
  SVGTextElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, BezierPath* bezier)
  {
    STDDBG;
  }

	void
	SVGTextElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage* ppath)
  {
  }		

	
  void
  SVGTextElement::glTraverseFill(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    if(_currentPCDATA) {
      setFont(svglContext, glinfo, 0);
      glTextTraversePCDATA(svglContext, glinfo, _textinfo, _currentPCDATA);
    }
  }

  void
  SVGTextElement::glTraverseStroke(svgl::Context * svglContext, svgl::GLInfo* glinfo, float sw)
  {
  }

  void
  SVGTextElement::glTextTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo*)
  {
    STDDBG;
  }

  void
  SVGTextElement::glTextTraversePCDATA(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo, dom::Text* pcdata)
  {
#if 1
    int bbx,bby,width,height;
    svglContext->fontManager->getBoundingBox(GLFT_TEXT(pcdata->getData()), &bbx, &bby, &width, &height);
    bby=-(height+bby);
    //std::cerr << DBGVAR(bbx) << DBGVAR(bby) << DBGVAR(width) << DBGVAR(height) << " " << pcdata->content.c_str() << std::endl;
#endif
    glPushMatrix();
    //SVGTransformable::glTraverse(svglContext, glinfo);
    //glDisable(GL_STENCIL_TEST);
    //
    float dx=0;
    css::TextAnchor anchor = getStyle().getValue().getTextAnchor();

    switch(anchor.value) {
    case css::TextAnchor::CSS_TEXTANCHOR_MIDDLE:
      dx=-width/2.;
      break;
    case css::TextAnchor::CSS_TEXTANCHOR_END:
      dx=-width;
      break;
    default:
      break;
    }
				
    glTranslatef(textinfo->x + dx, textinfo->y,0);
    //svglContext->fontManager->render(pcdata->getData());
    double currentScale = glinfo->getCurrentScale();
    glft::FontManager::font_type ft;
		float exactApparentFontSize;
		svglContext->fontManager->getFontTypeWithScale(currentScale,ft,exactApparentFontSize);
    if(ft==glft::FontManager::texture && glinfo->doAntiAliasing()) {
      glinfo->disableAntiAliasing();
    }

    svglContext->fontManager->renderWithScale(GLFT_TEXT(pcdata->getData()), currentScale);
    //svglContext->fontManager->renderAutoScale(GLFT_TEXT(pcdata->getData()));

    if(ft==glft::FontManager::texture && glinfo->doAntiAliasing()) {
      glinfo->enableAntiAliasing();
    }

		glPopMatrix();
		
		bbx += int(textinfo->x+dx);
		bby += int(textinfo->y);

#if 0
    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(bbx,bby);
    glVertex2f(bbx,bby+height);
    glVertex2f(bbx+width,bby+height);
    glVertex2f(bbx+width,bby);
    glEnd();
#endif

    textinfo->x+=width;
  }


  void
  SVGTextElement::glTraverseBoundingBoxPCDATA(svgl::Context * svglContext, svgl::GLInfo* glinfo, dom::Text* pcdata)
  {

    svg::SVGLength xl = GETVAL(X);
    svg::SVGLength yl = GETVAL(Y);
    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);

    glPushMatrix();
    glTranslated(x,y,0);

    if(_currentPCDATA) {
      int x,y,width,height;
      //svglContext->fontManager->getBoundingBox(_currentPCDATA->getData(), &x, &y, &width, &height);
      svglContext->fontManager->getBoundingBoxAutoScale(GLFT_TEXT(_currentPCDATA->getData()), &x, &y, &width, &height);
      y=-(height+y);
      //std::cerr << DBGVAR(x) << DBGVAR(y) << DBGVAR(width) << DBGVAR(height) << " " << pcdata->getData()->transcode().c_str() << __FL__;

      glBegin(GL_LINE_LOOP);

      glVertex2f(x,y);
      glVertex2f(x+width,y);
      glVertex2f(x+width,y+height);
      glVertex2f(x,y+height);

      glVertex2f(x,y);
      glVertex2f(x+width,y+height);
      glVertex2f(x+width,y);
      glVertex2f(x,y+height);

      glEnd();  

    }

    glPopMatrix();

  }

  void
  SVGTextElement::setText(const char* s)
  {
    
    for(dom::Node *n = getFirstChild(); n; n = getFirstChild())
      removeChild(n);

    dom::Text * t = new dom::Text(getOwnerDocument(), unicode::String::internString(s));
    insertBefore(t,0);
  }

// /* [rb] inherited from SVGTextContentElement

	long SVGTextElement::getNumberOfChars() {
		long res = 0;
		
		for(dom::Node *n = getFirstChild(); n != 0; n = n->getNextSibling()) {
			dom::Text *pcdata = dynamic_cast< dom::Text * >(n);
			if(pcdata != 0) {
				const unicode::String *content = pcdata->getData();
				unicode::String::size_type len = content->getLength();
				res += len;

			} else {
				SVGTextContentElement *e = dynamic_cast< SVGTextContentElement * >(n);
				if(e != 0) {
					res += e->getNumberOfChars();
				}
			}
		}
   	return res;
	}

	float	SVGTextElement::getComputedTextLength() {
		return getSubStringLength(0, (unsigned long)(-1));
	}

	float SVGTextElement::getSubStringLength(unsigned long charnum, unsigned long nchars) {
		// not compliant with recommandation because don't throw exception
		// and works only if TextElement contains only dom::Text
		float res = 0.;		
		
		const css::CSSStyleDeclaration& style = getStyle().getValue();
		css::CSSStyle::FontFamilyType fontfamily = style.getFontFamily();
		css::CSSStyle::FontStyleType fontstyle = style.getFontStyle();
		css::CSSStyle::FontWeightType fontweight = style.getFontWeight();

		glft::FontInfo f;
		f.family = fontfamily->transcode();
		f.style = fontstyle;
		f.weight = fontweight;
		for(int i = 0; i < f.family.length(); ++i) {
			f.family[i] = tolower(f.family[i]);
		}
		
		svgl::Context *svglContext = svgl::InitHelper::get()->context;

		css::CSSStyle::FontSizeType sizel = style.getFontSize();
		unsigned int size = (unsigned int)(svglContext->computeWidthLength(sizel));
		
		svglContext->fontManager->setCurrentFont(f, size);
		svglContext->fontSize = size;
		
		unsigned long charindex = 0;
		for(dom::Node *n = getFirstChild(); n != 0; n = n->getNextSibling()) {
			dom::Text *pcdata = dynamic_cast< dom::Text * >(n);
			if(pcdata != 0) {
				const unicode::String *content = pcdata->getData();
				unicode::String::size_type len = content->getLength();
			
				for(unicode::String::size_type c = 0; c < len; ++c) {
					if(charindex >= charnum + nchars) {
						return res;
					}
					if(charindex >= charnum) {
						unicode::String::char_type domchar = (*content)[c];
						res += svglContext->fontManager->getXAdvance(domchar);
					}
					charindex++;
				}
			}
		}

		return res;
	}

	SVGPoint *SVGTextElement::getStartPositionOfChar(unsigned long charnum) {
		throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
	}

	SVGPoint *SVGTextElement::getEndPositionOfChar(unsigned long charnum) {
		throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
	}
	
	SVGRect *SVGTextElement::getExtentOfChar(unsigned long charnum) {
		throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
	}

	float SVGTextElement::getRotationOfChar(unsigned long charnum) {
		throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
	}

	long SVGTextElement::getCharNumAtPosition(const SVGPoint& point) {
		throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
	}

// [rb] */
}
