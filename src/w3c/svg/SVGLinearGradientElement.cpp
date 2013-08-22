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
#include <w3c/svg/SVGLinearGradientElement.hpp>
//#include <w3c/svg/SVGGradientElement.hpp>
#include <w3c/svg/SVGStopElement.hpp>
#include <w3c/svg/SVGLength.hpp>
#include <w3c/css/RGBColor.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/Context.hpp>
#include <svgl/ExternalEntityManager.hpp>
#include <svgl/GradientInfo.hpp>


#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <cmath>

#include <assert.h>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

#define DBGCO(n) std::cerr << "x_"#n"=" << x_##n << " y_"#n"=" << y_##n << " " << std::endl;
// #define DBGVERBOSE

static inline void myswap(float& x, float& y) { float tmp=x;x=y;y=tmp;}

namespace svg {

  inline
  void
  removeStencil(svgl::GLInfo* glinfo, float x, float y, float width, float height)
  {
    glinfo->stencilPop();
    
    glBegin(GL_QUAD_STRIP);
    glVertex2f(x, y);
    glVertex2f(x+width, y);
    glVertex2f(x, y+height);
    glVertex2f(x+width, y+height);
    glEnd();

    glinfo->stencilEnableTop();
  }

  SVGLinearGradientElement::SVGLinearGradientElement(dom::Document * doc)
    : SVGGradientElement(doc, SVGLinearGradientElement_element_string)
  {
    SETVAL_AS_STRING(x1, "0%");
    SETVAL_AS_STRING(y1, "0%");
    SETVAL_AS_STRING(x2, "100%");
    SETVAL_AS_STRING(y2, "0%");
  }

  void
  SVGLinearGradientElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
  }

  void
  SVGLinearGradientElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    glinfo->stencilPush();
    //glinfo->stencilDisableTest();
  }

	
	struct LinearGradientInfo : svgl::GradientInfo {
		const SVGLength *x1, *y1, *x2, *y2;
		LinearGradientInfo() {
			x1=y1=x2=y2=0;
		}
	};
	
	void buildGradientInfo(svgl::Context* svglContext, svg::SVGLinearGradientElement *gradient, LinearGradientInfo& gradientInfo)
	{
		if(gradientInfo.units == SVGUnitTypes::SVG_UNIT_TYPE_UNKNOWN)
			gradientInfo.units = gradient->GETVAL(GradientUnits);
		if(gradientInfo.spreadMethod == SVGGradientElement::SVG_SPREADMETHOD_UNKNOWN)
			gradientInfo.spreadMethod = gradient->GETVAL(SpreadMethod);
		
		if(!gradientInfo.transforms)
			gradientInfo.transforms = &gradient->GETVAL(GradientTransform);
		
		if(!gradientInfo.x1) gradientInfo.x1 = &gradient->GET_BASE_VAL(X1);
		if(!gradientInfo.y1) gradientInfo.y1 = &gradient->GET_BASE_VAL(Y1);
		if(!gradientInfo.x2) gradientInfo.x2 = &gradient->GET_BASE_VAL(X2);
		if(!gradientInfo.y2) gradientInfo.y2 = &gradient->GET_BASE_VAL(Y2);
		
		if(gradientInfo.stops.empty())
			gradient->getStopElements(&gradientInfo.stops);
		
		dom::string ref = gradient->GETVAL(Href);
		if (ref) {
			dom::Element* de = svglContext->externalEntityManager->get(ref, gradient->getOwnerDocument());
			//std::cerr << this << " " << de << __FL__;
			SVGLinearGradientElement *e = dynamic_cast<SVGLinearGradientElement*>(de);
			if(e) {
				buildGradientInfo(svglContext, e, gradientInfo);
			}
			SVGGradientElement *g = dynamic_cast<SVGGradientElement*>(de);
			if(g) {
				buildGradientInfo(svglContext, g, gradientInfo);
			}
		}
	}
	
#define NEWGRADIENTCODE 1
#define TEST_GLLIST 0 // not so good... the stencil-based algorithm is really slow.
    
void
SVGLinearGradientElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, float x_R0, float y_R0, float bbwidth, float bbheight)
{
	
#if TEST_GLLIST
	// test with gllist
	if(!_damaged) {
		//STDDBG;
		glCallList(_glList);
	} else {
		_glList = glGenLists(1);
	}
#endif
	
	
#if NEWGRADIENTCODE
    LinearGradientInfo gradientInfo;
    buildGradientInfo(svglContext, this, gradientInfo);
#endif
    
    float percentage_width, percentage_height;
#if NEWGRADIENTCODE
    if(gradientInfo.units==SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
#else
    if(GETVAL(GradientUnits)==SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
#endif
        // value will eventually be multiplied by the correct value a few lines below...
      percentage_width = 1;
      percentage_height = 1;
    }
    else {
      percentage_width = bbwidth;
      percentage_height = bbheight;
    }

    // gx1-> gradient x1...
#if NEWGRADIENTCODE
    assert(gradientInfo.x1);
    assert(gradientInfo.y1);
    assert(gradientInfo.x2);
    assert(gradientInfo.y2);
    svg::SVGLength gx1l = *gradientInfo.x1;
    svg::SVGLength gy1l = *gradientInfo.y1;
    svg::SVGLength gx2l = *gradientInfo.x2;
    svg::SVGLength gy2l = *gradientInfo.y2;    
#else
    svg::SVGLength gx1l = GET_BASE_VAL(X1);
    svg::SVGLength gy1l = GET_BASE_VAL(Y1);
    svg::SVGLength gx2l = GET_BASE_VAL(X2);
    svg::SVGLength gy2l = GET_BASE_VAL(Y2);    
#endif
    
#define GETVAL1(x, l, relative) float x = l.computeValue(glinfo->dpi, relative);
    GETVAL1(x_G0, gx1l, percentage_width);
    GETVAL1(y_G0, gy1l, percentage_height);
    GETVAL1(x_G1, gx2l, percentage_width);
    GETVAL1(y_G1, gy2l, percentage_height);
#undef GETVAL1

    // transforms

    double tmp_x0=x_G0, tmp_y0=y_G0, tmp_x1=x_G1, tmp_y1=y_G1;
#if NEWGRADIENTCODE
    assert(gradientInfo.transforms);
    const SVGTransformList& transforms = *gradientInfo.transforms;
#else
    const SVGTransformList& transforms = GETVAL(GradientTransform);
#endif    
    for(unsigned int i=0; i<transforms.numberOfItems(); ++i) {
      transforms.getItem(i)->apply(x_G0, y_G0, &tmp_x0, &tmp_y0);
      transforms.getItem(i)->apply(x_G1, y_G1, &tmp_x1, &tmp_y1);
    }

    x_G0 = tmp_x0;
    y_G0 = tmp_y0;
    x_G1 = tmp_x1;
    y_G1 = tmp_y1;

    // Normal to GOG1 -> HOH1
    float x_H0 = x_G0;
    float y_H0 = y_G0;
    float x_H1 = x_G0 + (y_G0-y_G1);
    float y_H1 = y_G0 + (x_G1-x_G0);

#ifdef DBGVERBOSE
    std::cerr << "------" << std::endl;
    DBGCO(G0);
    DBGCO(G1);
    DBGCO(H0);
    DBGCO(H1);
#endif

    // SVG_UNIT_TYPE_USERSPACEONUSE

#if NEWGRADIENTCODE
    if(gradientInfo.units==SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
#else
    if(GETVAL(GradientUnits)==SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
#endif
    
      x_G0*=bbwidth;
      x_G1*=bbwidth;
      x_H0*=bbwidth;
      x_H1*=bbwidth;
      y_G0*=bbheight;
      y_G1*=bbheight;
      y_H0*=bbheight;
      y_H1*=bbheight;

      x_G0+=x_R0;
      x_G1+=x_R0;
      x_H0+=x_R0;
      x_H1+=x_R0;
      y_G0+=y_R0;
      y_G1+=y_R0;
      y_H0+=y_R0;
      y_H1+=y_R0;
    }

    float x_R1 = x_R0;
    float y_R1 = y_R0+bbheight;
#ifdef DBGVERBOSE
    DBGCO(R1)
#endif

    float x_R2 = x_R0+bbwidth;
    float y_R2 = y_R0+bbheight;
#ifdef DBGVERBOSE
    DBGCO(R2);
#endif

    float x_R3 = x_R0+bbwidth;
    float y_R3 = y_R0;
#ifdef DBGVERBOSE
    DBGCO(R3);
#endif


    //std::cerr << "------" << std::endl;

    //    std::cerr << DBGVAR(gx1) << DBGVAR(gy1) << DBGVAR(gx2) << DBGVAR(gy2) << std::endl;

    // get gradient successive values
#if NEWGRADIENTCODE
    svgl::StopElements& stops = gradientInfo.stops;
#else
    svgl::StopElements stops;
    getStopElements(&stops);
#endif
    
    if(stops.size()==0) {
      std::cerr << "no <stop>  for gradient " << GET_SIMPLE_VAL(Id) << " " __FILE__":" << __LINE__ << std::endl;
      removeStencil(glinfo, x_R0, y_R0, bbwidth, bbheight);
      return;
    }
#if 0
    else {
      svgl::StopElements::iterator first = stops.begin();
      if(first->offset!=0) {
	STDDBG;
	svgl::StopElementInfo newfirst = *first;
	newfirst.offset=0;
	stops.insert(first, newfirst);
      }

      svgl::StopElements::reverse_iterator last = stops.rbegin();
      if(last->offset!=1) {
	STDDBG;
	svgl::StopElementInfo newlast = *last;
	newlast.offset=1;
	stops.push_back(newlast);
      }
    }
#endif
    //std::cerr << stops.size() << std::endl;
    
    if(stops.size()==1) {
#if TEST_GLLIST
				glNewList(_glList, GL_COMPILE_AND_EXECUTE);
#endif
      // could be more efficient if detected sooner...
      //glinfo->stencilEnableTopAndPop();
      glinfo->stencilEnableTop();
      {
	float alpha_save = glinfo->alpha;
	glinfo->alpha *= stops[0].opacity;
	stops[0].color->glPreTraverse(svglContext, glinfo, this, x_R0,y_R0,bbwidth,bbheight);
	glinfo->alpha = alpha_save;
      }
      
      glBegin(GL_QUAD_STRIP);
      glVertex2f(x_R0, y_R0);
      glVertex2f(x_R0+bbwidth, y_R0);
      glVertex2f(x_R0, y_R0+bbheight);
      glVertex2f(x_R0+bbwidth, y_R0+bbheight);
      glEnd();

      removeStencil(glinfo, x_R0, y_R0, bbwidth, bbheight);
      //glShadeModel(GL_FLAT);
#if TEST_GLLIST
			glEndList();
#endif
      return;
    }

    /*
      Rx = bbox

                      / G1p
                     /          G1
                    / G0p      /
		   /          /
               R2p/          G0
                 /     
        R1 ------------------- R2
	/  |   /              |
       /   |  /               |
      /    | /                |
     /     |/                 |
R1p /	R0 -------------------- R3
         /                   /
        FG                  /
                           /
                          R3p

     */


    // get gradient orientation

#ifdef DBGVERBOSE
    DBGCO(G0);
    DBGCO(G1);
    DBGCO(H0);
    DBGCO(H1);
#endif

    float x_G0G1 = x_G1-x_G0;
    float y_G0G1 = y_G1-y_G0;
    float h_G0G1 = ::hypot(x_G0G1, y_G0G1);
    if(h_G0G1==0) {
      std::cerr << "wrong angle for gradient " << GET_SIMPLE_VAL(Id) << " " __FILE__":" << __LINE__ << std::endl;
      removeStencil(glinfo, x_R0, y_R0, bbwidth, bbheight);
      return;
    }

#ifdef DBGVERBOSE
    DBGCO(G0G1);
#endif

    float x_H0H1 = x_H1-x_H0;
    float y_H0H1 = y_H1-y_H0;
    //    float& h_G0H1 = h_G0G1;
    float h_H0H1 = ::hypot(x_H0H1, y_H0H1);

    // u is the unit vector with the gradient direction
    float ux = x_G0G1/h_G0G1;
    float uy = y_G0G1/h_G0G1;

#ifdef DBGVERBOSE
    std::cerr << DBGVAR(ux) << DBGVAR(uy) << std::endl;
#endif
    // v is the unit vector with the direction of the normal of the gradient
    float vx = x_H0H1/h_H0H1;
    float vy = y_H0H1/h_H0H1;

#ifdef DBGVERBOSE
    std::cerr << DBGVAR(vx) << DBGVAR(vy) << std::endl;
#endif

    if(x_G0G1<0) {
      myswap(x_R0,x_R3);
      myswap(y_R0,y_R3);
      myswap(x_R1,x_R2);
      myswap(y_R1,y_R2);
    }

    if(y_G0G1<0) {
      myswap(x_R0,x_R1);
      myswap(y_R0,y_R1);
      myswap(x_R2,x_R3);
      myswap(y_R2,y_R3);
    }


    // R0 is the corner of the bbox
    // in the following, we get G0p (G0 prime) which is the projection of G0 on the line
    // of direction u and passing by R0
#ifdef DBGVERBOSE
    DBGCO(R0);
#endif
    float x_R0G0 = x_G0-x_R0;
    float y_R0G0 = y_G0-y_R0;
#ifdef DBGVERBOSE
    DBGCO(R0G0)
#endif
    float x_G0p;
    float y_G0p;
    float d_R0G0p;
    float h_R0G0 = ::hypot(x_R0G0, y_R0G0);
    if(h_R0G0!=0) {
      d_R0G0p = ( x_R0G0*ux + y_R0G0*uy);
      //std::cerr << DBGVAR(d_R0G0p) << std::endl;
      /*      if(d_R0G0p<0)
	      d_R0G0p=-d_R0G0p;*/
      x_G0p = x_R0 + d_R0G0p * ux;
      y_G0p = y_R0 + d_R0G0p * uy;
    }
    else {
      x_G0p = x_G0;
      y_G0p = y_G0;
      d_R0G0p=0;
    }
#ifdef DBGVERBOSE
    DBGCO(G0p)
#endif
    // t_FG is the number of times the gradient vector is from the first point of a slided
    // gradient vector to "enter" the bbox
    float t_FG = -::ceil(d_R0G0p/h_G0G1);
    // std::cerr << DBGVAR(t_FG) << std::endl;

    // FG is the point from which the first gradient enters the bbox
    float x_FG;
    float y_FG;

    x_FG = x_G0p + t_FG*x_G0G1;
    y_FG = y_G0p + t_FG*y_G0G1;
#ifdef DBGVERBOSE
    DBGCO(FG);
#endif
    // LG is the projection of the other corner (R2), to know where to stop
    if(bbwidth==0 && bbheight==0) {
      std::cerr << "wrong bounding box " << bbwidth << " " << bbheight << " " __FILE__":" << __LINE__ << std::endl;
      removeStencil(glinfo, x_R0, y_R0, bbwidth, bbheight);
      return;
    }

    float x_R0R2 = x_R2-x_R0;
    float y_R0R2 = y_R2-y_R0;
    //    float h_R0R2 = std::hypot(x_R0R2, y_R0R2);
    float d_R0LG = (x_R0R2*ux + y_R0R2*uy);
    float t_LG = (d_R0LG-d_R0G0p) / h_G0G1;
        
    if(d_R0LG<0)
      t_LG=-t_LG;
    
#ifdef DBGVERBOSE
    std::cerr << DBGVAR(t_LG) << std::endl;
#endif
    float den = (ux*vy-vx*uy);
    if(den==0) {
      std::cerr << "wrong gradient " << DBGVAR(x_G0) << DBGVAR(y_G0) << DBGVAR(x_G1) <<  DBGVAR(y_G1) << " " __FILE__":" << __LINE__ << std::endl;
      removeStencil(glinfo, x_R0, y_R0, bbwidth, bbheight);
      return;
    }
    
    // R1p projection of R1 on FG.v (v is the normal to u)
    float x_FGR1 = x_R1-x_FG;
    float y_FGR1 = y_R1-y_FG;
#ifdef DBGVERBOSE
    DBGCO(FGR1)
#endif
    float x_R1p;
    float y_R1p;
    {
      float t = (vx*y_FGR1-vy*x_FGR1) / den;
      //std::cerr << DBGVAR(t) << std::endl;
      x_R1p = x_R1 + t*ux;
      y_R1p = y_R1 + t*uy;
    }
#ifdef DBGVERBOSE
    DBGCO(R1p)
#endif
    // R3p projection of R3 on FG.v (v is the normal to u)
    float x_FGR3 = x_R3-x_FG;
    float y_FGR3 = y_R3-y_FG;
#ifdef DBGVERBOSE
    DBGCO(FGR3)
#endif
    float x_R3p;
    float y_R3p;

    {
      float t = (vx*y_FGR3-vy*x_FGR3) / den;
      //std::cerr << DBGVAR(t) << std::endl;
      x_R3p = x_R3 + t*ux;
      y_R3p = y_R3 + t*uy;
    }
#ifdef DBGVERBOSE
    DBGCO(R3p)
#endif

#if TEST_GLLIST
			glNewList(_glList, GL_COMPILE_AND_EXECUTE);
#endif
      //glinfo->stencilEnableTopAndPop();
      glinfo->stencilEnableTop();
    //glDisable(GL_STENCIL_TEST);

    glShadeModel(GL_SMOOTH);
    glBegin(GL_QUAD_STRIP);

    float incx = x_G0G1;
    float incy = y_G0G1;

#define DBGGSQ 0
#if DBGGSQ
    // R1 R3 R1end R3end gradient rectangle for debugging purpose
    float R1x=x_R1p;
    float R1y=y_R1p;
    float R3x=x_R3p;
    float R3y=y_R3p;
    float R1endx=x_R1p;
    float R1endy=y_R1p;
    float R3endx=x_R3p;
    float R3endy=y_R3p;
#endif

#if NEWGRADIENTCODE
    SVGGradientElement::SVG_SPREADMETHOD spread = gradientInfo.spreadMethod;
#else
    SVGGradientElement::SVG_SPREADMETHOD spread = GETVAL(SpreadMethod);
#endif
    
    switch(spread) {
      //    case SVGGradientElement::SVG_SPREADMETHOD_UNKNOWN:
    case SVGGradientElement::SVG_SPREADMETHOD_UNKNOWN:
      STDDBG;
    case SVGGradientElement::SVG_SPREADMETHOD_PAD: {
      //      std::cerr << "pad" << std::endl
      
      if(t_FG<0) {
	{
	  float alpha_save = glinfo->alpha;
	  glinfo->alpha *= stops[0].opacity;
	  stops[0].color->glPreTraverse(svglContext, glinfo,this,x_R0,y_R0,bbwidth,bbheight);
	  glinfo->alpha = alpha_save;
	}

	glVertex2f(x_R1p, y_R1p);
	glVertex2f(x_R3p, y_R3p);

	x_R1p+=-t_FG*incx;
	y_R1p+=-t_FG*incy;
	x_R3p+=-t_FG*incx;
	y_R3p+=-t_FG*incy;
      }
      if(t_LG<0) {
	glVertex2f(x_R1p-t_LG*ux, y_R1p-t_LG*uy);
	glVertex2f(x_R3p-t_LG*ux, y_R3p-t_LG*uy);
#if DBGGSQ
	R1endx = x_R1p-t_LG*ux;
	R1endy = y_R1p-t_LG*uy;
	R3endx = x_R3p-t_LG*ux;
	R3endy = y_R3p-t_LG*uy;
#endif
      }
      else {
	for (unsigned int i=0; i<stops.size(); ++i) {
	  {
	    float alpha_save = glinfo->alpha;
	    glinfo->alpha *= stops[i].opacity;
	    stops[i].color->glPreTraverse(svglContext, glinfo,this,x_R0,y_R0,bbwidth,bbheight);
	    glinfo->alpha = alpha_save;
	  }

	  float x,y;
	  x = x_R1p + stops[i].offset*incx;
	  y = y_R1p + stops[i].offset*incy;
	  //	  std::cerr << DBGVAR(x) << DBGVAR(y) << DBGVAR(stops[i].offset) << std::endl;
	  glVertex2f(x, y);
#if DBGGSQ
	  R1endx=x;
	  R1endy=y;
#endif

	  x = x_R3p + stops[i].offset*incx;
	  y = y_R3p + stops[i].offset*incy;
	  //std::cerr << DBGVAR(x) << DBGVAR(y) << DBGVAR(stops[i].offset) << std::endl;
	  glVertex2f(x, y);
#if DBGGSQ
	  R3endx=x;
	  R3endy=y;
#endif 
	}
	x_R1p+=incx;
	y_R1p+=incy;
	x_R3p+=incx;
	y_R3p+=incy;

	if(t_LG>1) {
	  float t=t_LG-1;
	  glVertex2f(x_R1p+t*incx, y_R1p+t*incy);
	  glVertex2f(x_R3p+t*incx, y_R3p+t*incy);
#if DBGGSQ
	  R1endx = x_R1p+t*incx;
	  R1endy = y_R1p+t*incy;
	  R3endx = x_R3p+t*incx;
	  R3endy = y_R3p+t*incy;
#endif
	}

      }

    }
      break;
    case SVGGradientElement::SVG_SPREADMETHOD_REPEAT: {
      //      std::cerr << "repeat" << std::endl;
      while(t_FG<t_LG) {
	for (unsigned int i=0; i!=stops.size(); ++i) {
	  {
	    float alpha_save = glinfo->alpha;
	    glinfo->alpha *= stops[i].opacity;
	    stops[i].color->glPreTraverse(svglContext, glinfo,this,x_R0,y_R0,bbwidth,bbheight);
	    glinfo->alpha = alpha_save;
	  }

	  float x,y;
	  float offset = stops[i].offset;

	  x = x_R1p+offset*incx;
	  y = y_R1p+offset*incy;
	  //std::cerr << DBGVAR(x) << DBGVAR(y) << std::endl;
	  glVertex2f(x, y);
#if DBGGSQ
	  R1endx=x;
	  R1endy=y;
#endif
	  
	  x = x_R3p+offset*incx;
	  y = y_R3p+offset*incy;
	  //std::cerr << DBGVAR(x) << DBGVAR(y) << std::endl;
	  glVertex2f(x, y);
#if DBGGSQ
	  R3endx=x;
	  R3endy=y;
#endif

	}

	x_R1p+=incx;
	y_R1p+=incy;
	x_R3p+=incx;
	y_R3p+=incy;

	t_FG+=1;
      }

    }
      break;
    case SVGGradientElement::SVG_SPREADMETHOD_REFLECT: {
      //      std::cerr << "reflect" << std::endl;
      bool reverse=false;
      if ( static_cast<int>(t_FG)%2!=0 ) {
	reverse = !reverse;
      }
      while(t_FG<t_LG) {
	for (unsigned int i=0; i!=stops.size(); ++i) {
	  int cindex = i;
	  float offset;
	  if (reverse) {
	    cindex = stops.size()-i-1;
	  }

	  {
	    float alpha_save = glinfo->alpha;
	    glinfo->alpha *= stops[cindex].opacity;
	    stops[cindex].color->glPreTraverse(svglContext, glinfo,this,x_R0,y_R0,bbwidth,bbheight);
	    glinfo->alpha = alpha_save;
	  }

	  float x,y;
	  if (reverse) {
	    offset = 1 - stops[cindex].offset;
	  }
	  else
	    offset = stops[cindex].offset;

	  x = x_R1p+offset*incx;
	  y = y_R1p+offset*incy;
	  //std::cerr << DBGVAR(x) << DBGVAR(y) << std::endl;
	  glVertex2f(x, y);

#if DBGGSQ
	  R1endx=x;
	  R1endy=y;
#endif

	  x = x_R3p+offset*incx;
	  y = y_R3p+offset*incy;
	  //std::cerr << DBGVAR(x) << DBGVAR(y) << std::endl;
	  glVertex2f(x, y);
#if DBGGSQ
	  R3endx=x;
	  R3endy=y;
#endif

	}

	x_R1p+=incx;
	y_R1p+=incy;
	x_R3p+=incx;
	y_R3p+=incy;

	reverse = !reverse;
	t_FG+=1;
      }
      break;
    }
    }
    
    glEnd();
    glShadeModel(GL_FLAT);

    if(x_G0G1<0) {
      myswap(x_R0,x_R3);
      myswap(y_R0,y_R3);
      myswap(x_R1,x_R2);
      myswap(y_R1,y_R2);
    }

    if(y_G0G1<0) {
      myswap(x_R0,x_R1);
      myswap(y_R0,y_R1);
      myswap(x_R2,x_R3);
      myswap(y_R2,y_R3);
    }


#if 1 // stencilEnableTopAndPop()
    glinfo->stencilPop();

    glBegin(GL_QUAD_STRIP);
    glVertex2f(x_R0, y_R0);
    glVertex2f(x_R0+bbwidth, y_R0);
    glVertex2f(x_R0, y_R0+bbheight);
    glVertex2f(x_R0+bbwidth, y_R0+bbheight);
    glEnd();
    glinfo->stencilEnableTop();
#endif

#if 0
    //float bbx,bby,bbwidth,bbheight;
    float bbx=x_R0,bby=y_R0;
    //getBoundingBox(svglContext, glinfo, &bbx, &bby,&bbwidth,&bbheight);
    std::cerr << DBGVAR(bbx) << DBGVAR(bby) << DBGVAR(bbwidth) << DBGVAR(bbheight) << __FL__;
    glColor4f(0,0,1,1);
    glBegin(GL_LINE_LOOP);
    glVertex2d(bbx,bby);
    glVertex2d(bbx,bby+bbheight);
    glVertex2d(bbx+bbwidth,bby+bbheight);
    glVertex2d(bbx+bbwidth,bby);
    glEnd();
#if DBGGSQ

    glColor4f(1,0,0,1);
    glBegin(GL_LINE_LOOP);
    glVertex2d(R1x,R1y);
    glVertex2d(R1endx,R1endy);
    glVertex2d(R3endx,R3endy);
    glVertex2d(R3x,R3y);
    glEnd();

    glColor4f(0,1,0,1);
    glBegin(GL_LINES);
    glVertex2d(x_G0, y_G0);
    glVertex2d(x_G1, y_G1);

    glVertex2d(x_H0, y_H0);
    glVertex2d(x_H1, y_H1);

    glVertex2d(x_G0p, y_G0p);
    glVertex2d(x_G0p+(x_G1-x_G0), y_G0p+(y_G1-y_G0));

    glVertex2d(x_R1, y_R1);
    glVertex2d(x_R1p, y_R1p);

    glEnd();

    glColor4f(1,1,0,1);
    glBegin(GL_POINTS);
    glVertex2d(x_G0p, y_G0p);
    glEnd();

#endif

#endif

#if TEST_GLLIST
		glEndList();
		_damaged=false;
#endif		

  }
}




