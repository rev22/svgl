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
#include <w3c/svg/SVGTSpanElement.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/TextInfo.hpp>
#define ___SVGL
#include <glft/FontManager.hpp>

#include <w3c/dom/Text.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

#define GLFT_TEXT(x) (new std::string((x)->transcode()))


namespace svg {

  void
  glCharPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo, unicode::String::char_type domchar);

  void
  glCharPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo, unicode::String::char_type domchar);


  void
  SVGTSpanElement::glTextTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo)
  {
    _textinfo = textinfo;

    for(dom::Node *n = getFirstChild(); n; n=n->getNextSibling()) {
      _currentPCDATA = 0;
      _currentTextContent = 0;
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
	  _currentTextContent = e;
	  e->glTextTraverse(svglContext, glinfo, _textinfo);
	  //SVGElement *e2 = dynamic_cast<SVGElement*>(e);
	  //e2->DOIT;
	}
      }
    }
  }

  void
  SVGTSpanElement::glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
  }

  void
  SVGTSpanElement::getBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo, float* bbx, float* bby, float* bbwidth, float* bbheight)
  {
    *bbx=*bby=*bbwidth=*bbheight;
  }

  void
  SVGTSpanElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, BezierPath* bezier)
  {
    STDDBG;
  }

	void
	SVGTSpanElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage* ppath)
  {
  }		
	
  void
  SVGTSpanElement::glTraverseFill(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    if(_currentPCDATA) {
      //setFont(svglContext, glinfo, _textinfo);
      glTextTraversePCDATA(svglContext, glinfo, _textinfo, _currentPCDATA);
    }
  }
  
  void
  SVGTSpanElement::glTraverseStroke(svgl::Context * svglContext, svgl::GLInfo* glinfo, float sw)
  {
  }

  void
  SVGTSpanElement::glTextTraversePCDATA(svgl::Context * svglContext, svgl::GLInfo* glinfo, svgl::TextInfo* textinfo, dom::Text* pcdata)
  {
    svg::SVGLengthList xlist = GETVAL(X);
    svg::SVGLengthList ylist = GETVAL(Y);
    svg::SVGLengthList dxlist = GETVAL(Dx);
    svg::SVGLengthList dylist = GETVAL(Dy);
    svg::SVGNumberList rotatelist = GETVAL(Rotate);

    //    bool underline = true;
    //    bool lineThrough = true;

    bool doCharManip = false;
    unsigned int maxCharManipSize = xlist.numberOfItems();
    {
      unsigned int tmp = xlist.numberOfItems();
      if(maxCharManipSize<tmp) maxCharManipSize=tmp;
      tmp = ylist.numberOfItems();
      if(maxCharManipSize<tmp) maxCharManipSize=tmp;
      tmp = dxlist.numberOfItems();
      if(maxCharManipSize<tmp) maxCharManipSize=tmp;
      tmp = dylist.numberOfItems();
      if(maxCharManipSize<tmp) maxCharManipSize=tmp;
      tmp = rotatelist.numberOfItems();
      if(maxCharManipSize<tmp) maxCharManipSize=tmp;
    }

    if(maxCharManipSize || textinfo->distanceIterator) {
      doCharManip=true;
    }
    unsigned int charIndex=0;

    float oldFontSize = svglContext->fontSize;
    setFont(svglContext, glinfo, textinfo);

    glPushMatrix();

    float &x=textinfo->x, &y=textinfo->y, &dx=textinfo->dx, &dy=textinfo->dy;
    float rotate=0;
		double currentScale = glinfo->getCurrentScale();

    dom::string content = pcdata->getData();
    //std::cerr << content << __FL__;
    if(doCharManip) {

      SVGLength xl,yl,dxl,dyl;

      while(charIndex<maxCharManipSize || (textinfo->distanceIterator!=0 && charIndex<content->getLength())) {

	if(charIndex<xlist.numberOfItems()) {
	  xl=xlist.getItem(charIndex);
	  x = svglContext->computeWidthLength(xl);
	}

	if(charIndex<ylist.numberOfItems()) {
	  yl=ylist.getItem(charIndex);
	  y = svglContext->computeHeightLength(yl);
	}
	  
	if(charIndex<dxlist.numberOfItems()) {
	  dxl=dxlist.getItem(charIndex);
	  dx += svglContext->computeWidthLength(dxl);
	}
	if(charIndex<dylist.numberOfItems()) {
	  dyl=dylist.getItem(charIndex);
	  dy += svglContext->computeHeightLength(dyl);
	}
	  
	if(charIndex<rotatelist.numberOfItems()) {
	  SVGNumber rotaten=rotatelist.getItem(charIndex);
	  rotate = rotaten.GET_SIMPLE_VAL(Value);
	}
	  
	//std::cerr << DBGVAR((char) ((*content)[charIndex])) << DBGVAR(x) << DBGVAR(y) << DBGVAR(dx) << DBGVAR(dy) << DBGVAR(rotate) << std::endl;

	glPushMatrix();	      

	// first position the base line
	glTranslatef(x,y,0);

	//std::cerr << (char) ((*content)[charIndex]) << __FL__;

	if(textinfo->distanceIterator) {
	  glPushMatrix();
	  glCharPreTraverse(svglContext, glinfo, textinfo, (*content)[charIndex]);
	}

	// then apply deltas
	glTranslatef(dx,dy,0);
	glRotatef(rotate, 0,0,1);

	float xadvance = svglContext->fontManager->getXAdvance((*content)[charIndex]);
#if 0
	if(underline) {
	  const float yshift = oldFontSize * .1;
	  const float size = oldFontSize * .1;
	  glRectf( 0,  yshift, xadvance, yshift+size );
	}
#endif
	{
		svglContext->fontManager->renderWithScale((*content)[charIndex], currentScale);
	}
#if 0
	if(lineThrough) {
	  const float yshift = -oldFontSize * .3;
	  const float size = oldFontSize * .1;
	  glRectf( 0,  -yshift, xadvance, -(yshift+size) );
	}
#endif
	if(textinfo->distanceIterator) {
	  glCharPostTraverse(svglContext, glinfo,textinfo,(*content)[charIndex]);
	  glPopMatrix();
	}
	else {
	  x+=xadvance;
	}

	glPopMatrix();
	++charIndex; 
      }	    
    }

    if(charIndex<content->getLength()) {
      int donewidth, remainingwidth;
      unicode::String* done = content->substring(0,charIndex);
      if(done) {
	//std::cerr << DBGVAR(done) << __FL__;
	int bbx,bby,height;
	svglContext->fontManager->getBoundingBox(GLFT_TEXT(done), &bbx, &bby, &donewidth, &height);
      }
      else {
	donewidth=0;
      }
      unicode::String* remaining = content->substring(charIndex);
      if(remaining) {
	//std::cerr << DBGVAR(remaining) << __FL__;
	int bbx,bby,height;
	svglContext->fontManager->getBoundingBox(GLFT_TEXT(remaining), &bbx, &bby, &remainingwidth, &height);
      }
      else {
	remainingwidth=0;
      }

      glTranslated(x+dx,y+dy,0);
      //glRotatef(rotate, 0,0,1); // or not ?
      glPushMatrix();
      //svglContext->fontManager->render(remaining);
      //svglContext->fontManager->renderAutoScale(GLFT_TEXT(remaining));
			svglContext->fontManager->renderWithScale(GLFT_TEXT(remaining), currentScale);
      glPopMatrix();
      textinfo->x += remainingwidth;
    }

    glPopMatrix();

    svglContext->fontSize=oldFontSize;

  }

}














