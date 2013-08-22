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
#include <w3c/svg/SVGTextPathElement.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/svg/SVGFilterElement.hpp>
#include <w3c/svg/SVGPathElement.hpp>

#include <svgl/bezierpath.hpp>
#include <svgl/TextInfo.hpp>
#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/ExternalEntityManager.hpp>
#include <glft/FontManager.hpp>
#include <svgl/BezierPathDistanceIterator.hpp>
#include <svgl/BezierPathLength.hpp>

#include <w3c/dom/Text.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>


namespace svg {

  SVGTextPathElement::SVGTextPathElement(dom::Document * doc)
    : SVGTextContentElement(doc, SVGTextPathElement_element_string), _bezier(0)
  {
  }

  void
  SVGTextPathElement::glTextTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo)
  {
    _textinfo = textinfo;
    buildBezier(svglContext, glinfo, textinfo);

    for(dom::Node *n = getFirstChild(); n; n=n->getNextSibling()) {
      _currentPCDATA = 0;
      dom::Text * pcdata = dynamic_cast<dom::Text*>(n);
      if(pcdata) {
	_currentPCDATA = pcdata;
	//SVGTransformable::glTraverse(svglContext, glinfo);
	SVGStylable::glTraverse(svglContext, glinfo, false, true);
      }
      else {
	SVGTextContentElement *e = dynamic_cast<SVGTextContentElement*>(n);
	if(e) {
	  //std::cerr << "text may not be at the right place since transforms are not traversed" __FL__;
	  e->glTextTraverse(svglContext, glinfo, _textinfo);
	  //textinfo->dx = textinfo->distanceIterator->x;
	  //textinfo->dy = textinfo->distanceIterator->y;
	}
      }
    }

    delete _textinfo->distanceIterator;
    _textinfo->distanceIterator=0;

  }

  void
  SVGTextPathElement::glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
  }

  void
  SVGTextPathElement::getBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo, float* bbx, float* bby, float* bbwidth, float* bbheight)
  {
    *bbx=*bby=*bbwidth=*bbheight;
  }

  void
  SVGTextPathElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, BezierPath* bezier)
  {
    STDDBG;
  }

	void
	SVGTextPathElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage* ppath)
  {
  }		


  void
  SVGTextPathElement::glTraverseFill(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    if(_currentPCDATA) {
      //setFont(svglContext, glinfo, _textinfo);
      glTextTraversePCDATA(svglContext, glinfo, _textinfo, _currentPCDATA);
    }
  }
  
  void
  SVGTextPathElement::glTraverseStroke(svgl::Context * svglContext, svgl::GLInfo* glinfo, float sw)
  {
  }

  void
  SVGTextPathElement::buildBezier(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo)
  {

    if(!_bezier) {
      unicode::String* ref = GETVAL(Href);
      SVGPathElement* path=0;
    
      if (ref) {
	dom::Element* de = svglContext->externalEntityManager->get(ref, getOwnerDocument());
	path = dynamic_cast<SVGPathElement*>(de);
      }
      if(!path)
	//std::cerr << "textPath " << ref << " " __FILE__ ":" << __LINE__ << std::endl;
	return;

      const SVGPathSegList& pl = path->GET_SIMPLE_VAL(PathSegList);
      if (pl.numberOfItems()<2)
	return;

      _bezier = new BezierPath;
      
      for(unsigned int i=0; i<pl.numberOfItems(); ++i) {
	const SVGPathSeg* p = pl.getItem(i);
	p->updateBezier(*_bezier);
      }
    }

    svg::SVGLength startOffsetl = GETVAL(StartOffset);

    float startOffset_ = 0;
    {
      float sav = svglContext->viewport_width;
      svglContext->viewport_width = svgl::length(*_bezier);
      startOffset_ = svglContext->computeWidthLength(startOffsetl);
      //std::cerr << DBGVAR(startOffsetl.getValueInSpecifiedUnits().getValue()) << DBGVAR(svglContext->viewport_width) << __FL__;
      svglContext->viewport_width = sav;
    }
    //std::cerr << DBGVAR(startOffset_) << __FL__;
    textinfo->distanceIterator= new svgl::BezierPathDistanceIterator(*_bezier);
    textinfo->distanceIterator->advance(startOffset_);
    textinfo->x = textinfo->distanceIterator->x /*+ textinfo->distanceIterator->dx*/;
    textinfo->y = textinfo->distanceIterator->y /*+ textinfo->distanceIterator->dy*/;

    //std::cerr << DBGVAR(startOffset_) << DBGVAR(textinfo->distanceIterator->x) << DBGVAR(textinfo->distanceIterator->y) << DBGVAR(textinfo->distanceIterator->dx) << DBGVAR(textinfo->distanceIterator->dy) << __FL__;
  }



  void
  SVGTextPathElement::glTextTraversePCDATA(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo, dom::Text* pcdata)
  {
    // std::cerr << pcdata->content.c_str() << std::endl;
    setFont(svglContext, glinfo, textinfo);

    glPushMatrix();
    glTranslatef(textinfo->x, textinfo->y, 0);    
    //std::cerr << DBGVAR(textinfo->dx) << DBGVAR(textinfo->dy) << __FL__;
    //int bbx,bby,width,height;
    float prevadvance=0;//startOffset_*2; // it's divided by 2 below...
    const unicode::String* content = pcdata->getData();
    unicode::String::size_type len = content->getLength();

    //std::cerr << DBGVAR(content) << __FL__;

    for (unicode::String::size_type c=0; c<len; ++c) {
      unicode::String::char_type domchar = (*content)[c];
      float xadvance = svglContext->fontManager->getXAdvance(domchar);
      //std::cerr << bbx << " " << bby << " " << advance << " " << height << " " << tmp.c_str() << std::endl;
      float advance = prevadvance/2.+xadvance/2.;
      //std::cerr << DBGVAR((char) domchar) << DBGVAR(advance) << __FL__;
      textinfo->distanceIterator->advance(advance);
      //std::cerr << DBGVAR((char) domchar) << DBGVAR(xadvance) << DBGVAR(textinfo->distanceIterator->finished()) << __FL__;
      //std::cerr << DBGVAR(textinfo->distanceIterator->x) << DBGVAR(textinfo->distanceIterator->y) << DBGVAR(textinfo->distanceIterator->dx) << DBGVAR(textinfo->distanceIterator->dy) << __FL__;

      glTranslatef(textinfo->distanceIterator->dx, textinfo->distanceIterator->dy,0);
      textinfo->x += textinfo->distanceIterator->dx;
      textinfo->y += textinfo->distanceIterator->dy;
      float orientation = textinfo->distanceIterator->getOrientation();
      glPushMatrix();
      glRotatef(-orientation, 0,0,1);
      glTranslatef(-xadvance/2.,0,0);
      svglContext->fontManager->render(domchar);
      glPopMatrix();
      prevadvance=xadvance;
    }

#if 0
    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(bbx,bby);
    glVertex2f(bbx,bby+height);
    glVertex2f(bbx+width,bby+height);
    glVertex2f(bbx+width,bby);
    glEnd();
#endif


    glPopMatrix();

  }

#if 1
  void
  glCharPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo, unicode::String::char_type domchar)
  {
    float advance = svglContext->fontManager->getXAdvance(domchar);
    textinfo->distanceIterator->advance(advance/2.);
    glTranslatef(textinfo->distanceIterator->dx, textinfo->distanceIterator->dy,0);
    textinfo->x += textinfo->distanceIterator->dx;
    textinfo->y += textinfo->distanceIterator->dy;
    //textinfo->dx = textinfo->distanceIterator->dx;
    //textinfo->dy = textinfo->distanceIterator->dy;

    float orientation = textinfo->distanceIterator->getOrientation();

    glPushMatrix();
    glRotatef(-orientation, 0,0,1);
    glTranslatef(-advance/2.,0,0);
  }

  void
  glCharPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo, unicode::String::char_type domchar)
  {
    glPopMatrix();
    float advance = svglContext->fontManager->getXAdvance(domchar);
    textinfo->distanceIterator->advance(advance/2.);
    //glTranslatef(textinfo->distanceIterator->dx, textinfo->distanceIterator->dy,0);
    textinfo->x += textinfo->distanceIterator->dx;
    textinfo->y += textinfo->distanceIterator->dy;
    //textinfo->dx = textinfo->distanceIterator->dx;
    //textinfo->dy = textinfo->distanceIterator->dy;
  }
#endif
    
}
