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
#include <w3c/svg/SVGFilterElement.hpp>
#include <w3c/svg/SVGFilterPrimitiveStandardAttributes.hpp>
#include <svgl/GLInfo.hpp>

#include <svgl/Context.hpp>
#include <svgl/BoundingBoxManager.hpp>
#include <svgl/MultiPassManager.hpp>
#include <svgl/TextureSnapshot.hpp>
#include <svgl/FilterContext.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>
#include <svgl/screenshot.hpp>


namespace svg {

  void
  SVGFilterElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, SVGElement * elem)
  {
    //std::cerr << "--> SVGFilterElement::glPreTraverse" << __FL__;
    _nothingToDo = !glinfo->has_GL_ARB_imaging();
    if(_nothingToDo) {
      std::cerr << "GL_ARB_imaging not supported, filters will not work " __FL__;
      return;
    }

    svg::SVGLength xl = GETVAL(X);
    svg::SVGLength yl = GETVAL(Y);
    svg::SVGLength widthl = GETVAL(Width);
    svg::SVGLength heightl = GETVAL(Height);
  
#define SETDEFAULT(x,v) \
    if(x##l.GET_SIMPLE_VAL(UnitType)==SVGLength::SVG_LENGTHTYPE_UNKNOWN) { \
      x##l.newValueSpecifiedUnits(SVGLength::SVG_LENGTHTYPE_PERCENTAGE, v); \
    }
  
    SETDEFAULT(x,-10);
    SETDEFAULT(x,-10);
    SETDEFAULT(width,120);
    SETDEFAULT(height,120);
  
    _nothingToDo=true;

#undef SETDEFAULT

    float &x=_x,&y=_y,&width=_width,&height=_height;
    float bbx,bby,bbwidth=0,bbheight=0;
    if(GETVAL(FilterUnits)==SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
      svgl::BoundingBoxManager bboxManager;
      bboxManager.glPreTraverse(glinfo);
      glPushMatrix();
      elem->glTraverseBoundingBox(svglContext, glinfo);
      glPopMatrix();
      float xmaxf,ymaxf;
      bool ok = bboxManager.glPostTraverse(glinfo, &bbx, &bby, &xmaxf, &ymaxf);
      if(!ok) {
	STDDBG;
	return;
      }
      bbwidth = xmaxf-bbx;
      bbheight = ymaxf-bby;

      /*
	filter region computation seems very strange to me.
	According to the spec (15.5), if x="-10%" then x = bbx*0.9.
	This scheme does not fulfill what it is meant for i.e.
	define a padding space to filters application.
	In the following code, if x="-10%", then it's -10% of
	width, so x = bbx - .1*bbwidth and y = bby - .1*bbheight 
       */

#define COMPUTEVAR(x, X, rel) \
      if(x##l.isAbsolute()) { \
	x = svglContext->compute##X##Length(x##l); \
      } \
      else { \
	x = x##l.GET_SIMPLE_VAL(Value); \
	x = bb##x + x*bb##rel; \
      }

      COMPUTEVAR(x,Width,width)
      COMPUTEVAR(y,Height,height)
#undef COMPUTEVAR

#define COMPUTEVAR(x, X, rel) \
      if(x##l.isAbsolute()) { \
	x = svglContext->compute##X##Length(x##l); \
      } \
      else { \
	x = x##l.GET_SIMPLE_VAL(Value); \
	x = x*bb##x; \
      }

      COMPUTEVAR(width,Width,width)
      COMPUTEVAR(height,Height,height)

#undef COMPUTEVAR

	//std::cerr << DBGVAR(bbx) << DBGVAR(bby) << DBGVAR(bbwidth) << DBGVAR(bbheight) << __FL__;
	//std::cerr << DBGVAR(x) << DBGVAR(y) << DBGVAR(width) << DBGVAR(height) << __FL__;
    }
    else {
      x = svglContext->computeWidthLength(xl);
      y = svglContext->computeHeightLength(yl);
      width = svglContext->computeWidthLength(widthl);
      height = svglContext->computeHeightLength(heightl);
    }

    //svgl::screenshot("1");
    _multiPassManager.push(glinfo, x, y, x+width, y+height);
    _nothingToDo=false;
#if 1
    //svgl::screenshot("2");

    glinfo->pushWindowMode();
    glBlendFunc(GL_ONE, GL_ZERO);
    glColor4f(1,1,1,0);
    glRectf(x,y,x+width,y+height);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glinfo->popWindowMode();
    //svgl::screenshot("3");
#endif
    //_x=x;_y=y;_width=width;_height=height;
    _x=bbx;_y=bby;_width=bbwidth;_height=bbheight;

    //std::cerr << "<-- SVGFilterElement::glPreTraverse" << __FL__;

    // FIXME we should clip to the bounding box
  }


  void
  SVGFilterElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, SVGElement * elem)
  {
    //std::cerr << "--> SVGFilterElement::glPostTraverse" << __FL__;
    if(_nothingToDo)
      return;

#define STRDEF(x) static unicode::String * x = unicode::String::internString(#x);
      STRDEF(SourceGraphic)
      STRDEF(SourceAlpha)
      STRDEF(BackgroundImage)
      STRDEF(BackgroundAlpha)
	//STRDEF(FillPaint)
	//STRDEF(StrokePaint)
#undef STRDEF

#if 1
    float &x=_x,&y=_y,&width=_width,&height=_height;
    //std::cerr << DBGVAR(x) << DBGVAR(y) << DBGVAR(width) << DBGVAR(height) << __FL__;

    svgl::FilterContext filterContext;
    filterContext.lastResult = SourceGraphic;

    int& xi = filterContext.x = (int)x;
    int& yi = filterContext.y = (int)(y);
    int& widthi = filterContext.width = (int)(width+.5);
    int& heighti = filterContext.height = (int)(height+.5);
    filterContext.dx = 0;
    filterContext.dy = 0;

    // first, fill up results with SourceGraphic, BackgroundImage... if necessary.
    bool
      useSourceGraphic=false,
      useSourceAlpha=false,
      useBackgroundImage=false,
      useBackgroundAlpha=false,
      useSavedResult=false
      ;

#if 1

    #define USE_ALL (useSourceGraphic && useSourceAlpha && useBackgroundImage && useBackgroundAlpha)

    // first effect may have a result that is automatically used by next filter
    SVGFilterPrimitiveStandardAttributes * firstFE=0;
    for(dom::Node *domchild = getFirstChild(); domchild && firstFE==0; domchild = domchild->getNextSibling() ) {
      firstFE = dynamic_cast<SVGFilterPrimitiveStandardAttributes*>(getFirstChild());
    }

    unicode::String * lastResult = 0;
    if(firstFE) { // else give up ... IMPROVEME
      lastResult = firstFE->getOptionalResult();
    }
    if (!lastResult || lastResult->getLength()==0)
      lastResult = SourceGraphic;

    for(dom::Node *domchild = getFirstChild(); domchild && !USE_ALL; domchild = domchild->getNextSibling() ) {
      SVGFilterPrimitiveStandardAttributes * fe = dynamic_cast<SVGFilterPrimitiveStandardAttributes*>(domchild);
      if(fe) {
	unicode::String * in1 = fe->getOptionalIn1();
	unicode::String * in2 = fe->getOptionalIn2();
	unicode::String * result = fe->getOptionalResult();
	// FIXME
	// won't work with feMergeNode...

	if(!in1 || !in1->getLength())
	  in1 = lastResult;
	if(!in2 || !in2->getLength())
	  in2 = lastResult;
	if(!result || !result->getLength())
	  result = SourceGraphic;
	lastResult=result;

	//std::cerr << DBGVAR(in1) << DBGVAR(in2) << DBGVAR(result) << DBGVAR(lastResult) << __FL__;

	if(!useSourceGraphic && ((*in1==*SourceGraphic) || (*in2==*SourceGraphic)) ) {
	    useSourceGraphic=true;
	}

	if(!useSourceAlpha && ((*in1==*SourceAlpha) || (*in2==*SourceAlpha)) ) {
	  glPixelTransferf(GL_RED_SCALE, 0);
	  glPixelTransferf(GL_GREEN_SCALE, 0);
	  glPixelTransferf(GL_BLUE_SCALE, 0);
	  svgl::TextureSnapshot * snapshot = new svgl::TextureSnapshot(xi,yi,widthi,heighti);
	  filterContext.results[SourceAlpha] = snapshot;	  
	  glPixelTransferf(GL_RED_SCALE, 1);
	  glPixelTransferf(GL_GREEN_SCALE, 1);
	  glPixelTransferf(GL_BLUE_SCALE, 1);
	  useSourceAlpha=true;
	}

	if(!useBackgroundImage && ((*in1==*BackgroundImage) || (*in2==*BackgroundImage)) ) {
	  svgl::TextureSnapshot * snapshot = _multiPassManager.getSaveUnder();
	  filterContext.results[BackgroundImage] = snapshot;
	  useBackgroundImage=true;
	}
	
	else if(!useBackgroundAlpha && ((*in1==*BackgroundAlpha) || (*in2==*BackgroundAlpha)) ) {
	  // FIXME
	  std::cerr << "Background Alpha not correctly supported in filters" << __FL__;
	  svgl::TextureSnapshot * snapshot = _multiPassManager.getSaveUnder();
	  filterContext.results[BackgroundAlpha] = snapshot;
	  useBackgroundAlpha=true;
	}

	else {
	  useSavedResult=true;
	}

      }
    }
    #undef USE_ALL

    //std::cerr << DBGVAR(useSourceGraphic) << DBGVAR(useSourceAlpha) << DBGVAR(useBackgroundImage) << DBGVAR(useBackgroundAlpha) << DBGVAR(useSavedResult) << __FL__;

    // if it can't be done using only glCopy, save the SourceGraphic...
    if(useSourceGraphic && useSavedResult ) {
      svgl::TextureSnapshot * snapshot = new svgl::TextureSnapshot(xi,yi,widthi,heighti);
      filterContext.results[SourceGraphic] = snapshot;
    }
#endif

    for(dom::Node *domchild = getFirstChild(); domchild; domchild = domchild->getNextSibling() ) {
      SVGFilterPrimitiveStandardAttributes * fe = dynamic_cast<SVGFilterPrimitiveStandardAttributes*>(domchild);
      if(fe) {
	glinfo->pushWindowMode();
	fe->glPreTraverse(svglContext, glinfo, elem, &filterContext);
	fe->glPostTraverse(svglContext, glinfo, elem, &filterContext);
	glinfo->popWindowMode();
      }
    }
#endif
    _multiPassManager.pop(glinfo);

#if 0
    // frame around the box
    glinfo->pushWindowMode();
    glColor3f(0.5,0.5,0.5);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y);
    glVertex2f(x,y+height);
    glVertex2f(x+width,y+height);
    glVertex2f(x+width,y);
    glEnd();
    glinfo->popWindowMode();
#endif

    //std::cerr << "<-- SVGFilterElement::glPostTraverse" << __FL__;
  }


  void
  SVGFilterElement::setFilterRes ( unsigned long filterResX, unsigned long filterResY )
  {
    throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
  }

}
