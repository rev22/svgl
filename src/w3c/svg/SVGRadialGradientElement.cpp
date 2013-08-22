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
#include <w3c/svg/SVGRadialGradientElement.hpp>
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
#include <svgl/gluShadedDisk.hpp>

#include <cmath>

#include <assert.h>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

#define DBGCO(n) std::cerr << "x_"#n"=" << x_##n << " y_"#n"=" << y_##n << " " << __FILE__;

// #define DBGVERBOSE

namespace svg {

  static
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


  SVGRadialGradientElement::SVGRadialGradientElement(dom::Document * doc)
    : SVGGradientElement(doc, SVGRadialGradientElement_element_string)
  {
    SETVAL_AS_STRING(cx, "50%");
    SETVAL_AS_STRING(cy, "50%");
    SETVAL_AS_STRING(r, "50%");
    // fx=cx and fy=cy if not specified !
    // They should have their unit set to unknown
  }

  void
  SVGRadialGradientElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    glinfo->stencilPush();
    //glinfo->stencilDisableTest();
  }
    
	struct RadialGradientInfo : svgl::GradientInfo {
		const SVGLength *cx, *cy, *r, *fx, *fy;
		RadialGradientInfo() {
			cx=cy=r=fx=fy=0;
		}
	};
	
	void buildGradientInfo(svgl::Context* svglContext, svg::SVGRadialGradientElement *gradient, RadialGradientInfo& gradientInfo)
	{
		if(gradientInfo.units == SVGUnitTypes::SVG_UNIT_TYPE_UNKNOWN)
			gradientInfo.units = gradient->GETVAL(GradientUnits);
		if(gradientInfo.spreadMethod == SVGGradientElement::SVG_SPREADMETHOD_UNKNOWN)
			gradientInfo.spreadMethod = gradient->GETVAL(SpreadMethod);
		
		if(!gradientInfo.transforms)
			gradientInfo.transforms = &gradient->GETVAL(GradientTransform);

		if(!gradientInfo.cx) gradientInfo.cx = &gradient->GET_BASE_VAL(Cx);
		if(!gradientInfo.cy) gradientInfo.cy = &gradient->GET_BASE_VAL(Cy);
		if(!gradientInfo.r) gradientInfo.r = &gradient->GET_BASE_VAL(R);
		if(!gradientInfo.fx) gradientInfo.fx = &gradient->GET_BASE_VAL(Fx);
		if(!gradientInfo.fy) gradientInfo.fy = &gradient->GET_BASE_VAL(Fy);
		
		if(gradientInfo.stops.empty())
			gradient->getStopElements(&gradientInfo.stops);
		
		dom::string ref = gradient->GETVAL(Href);
		if (ref) {
			dom::Element* de = svglContext->externalEntityManager->get(ref, gradient->getOwnerDocument());
			//std::cerr << this << " " << de << __FL__;
			SVGRadialGradientElement *e = dynamic_cast<SVGRadialGradientElement*>(de);
			if(e) {
				buildGradientInfo(svglContext, e, gradientInfo);
			}
			SVGGradientElement *g = dynamic_cast<SVGGradientElement*>(de);
			if(g) {
				buildGradientInfo(svglContext, g, gradientInfo);
			}
		}
	}
    
  void
  SVGRadialGradientElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, float x_R0, float y_R0, float bbwidth, float bbheight)
  {
		RadialGradientInfo gradientInfo;
		buildGradientInfo(svglContext, this, gradientInfo);
    float percentage_width, percentage_height;
    if(gradientInfo.units==SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
        // value will eventually be multiplied by the correct value a few lines below...
      percentage_width = 1;
      percentage_height = 1;
    }
    else {
      percentage_width = bbwidth;
      percentage_height = bbheight;
    }

    // gx1-> gradient x1...
		assert(gradientInfo.cx);
		assert(gradientInfo.cy);
		assert(gradientInfo.r);
		assert(gradientInfo.fx);
		assert(gradientInfo.fy);
		svg::SVGLength cxl = *gradientInfo.cx;
		svg::SVGLength cyl = *gradientInfo.cy;
		svg::SVGLength rl = *gradientInfo.r;
		svg::SVGLength fxl = *gradientInfo.fx;
		svg::SVGLength fyl = *gradientInfo.fy;
        
    float sqw2r2 = ::hypot(percentage_width,percentage_height)/sqrt(2);

#define GETVAL1(x, l, relative) float x = l.computeValue(glinfo->dpi, relative);
    GETVAL1(cx, cxl, percentage_width);
    GETVAL1(cy, cyl, percentage_height);
    GETVAL1(r, rl, sqw2r2);

    float sx=1, sy=1;
		float tx=cx, ty=cy;
		
		//std::cerr << DBGVAR(cx) << DBGVAR(cy) << DBGVAR(r) << DBGVAR(x_R0) << DBGVAR(y_R0) << std::endl;
#if 1
    // SVG_UNIT_TYPE_USERSPACEONUSE
    if(gradientInfo.units==SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
      cx*=bbwidth;
      cy*=bbheight;
      /*
      float sqw2r2 = ::hypot(bbwidth,bbheight)/std::sqrt(2);
      r*=sqw2r2;
      */
      sx = (float)bbwidth;
      sy = (float)bbheight;
			cx+=x_R0;
      cy+=y_R0;
			tx = cx;
			ty = cy;
      // transforms
      //glinfo->pushAndResetView();
    }
#endif
			
#if 0
    double tmp_x0=cx, tmp_y0=cy; //, tmp_x1=x_G1, tmp_y1=y_G1;
    const SVGTransformList& transforms = GETVAL(GradientTransform);
    for(unsigned int i=0; i<transforms.numberOfItems(); ++i) {
      transforms.getItem(i)->apply(cx, cy, &tmp_x0, &tmp_y0);
      //transforms.getItem(i)->apply(x_G1, y_G1, &tmp_x1, &tmp_y1);
    }

    cx = tmp_x0;
    cy = tmp_y0;
    //x_G1 = tmp_x1;
    //y_G1 = tmp_y1;
#endif

    //std::cerr << "------" << std::endl;
    
    // get gradient successive values
    svgl::StopElements& stops = gradientInfo.stops;

    if(stops.size()==0) {
      std::cerr << "no <stop>  for gradient " << GET_SIMPLE_VAL(Id) << " " __FILE__":" << __LINE__ << std::endl;
      removeStencil(glinfo, x_R0, y_R0, bbwidth, bbheight);
      return;
    }

    //std::cerr << offsets.size() << std::endl;
    
    if(stops.size()==1) {
      // could be more efficient if detected sooner...

      std::cerr << "don't know what to do with a one stop radial gradient " <<  __FILE__ ":" << __LINE__ << std::endl;
      glinfo->stencilEnableTop();
      {
				float alpha_save = glinfo->alpha;
				glinfo->alpha *= stops[0].opacity;
				stops[0].color->glPreTraverse(svglContext, glinfo, this, x_R0,y_R0,bbwidth,bbheight);
				glinfo->alpha = alpha_save;
      }
      //      glColor4f(stops[0].color->r, stop[0].color->g, stops[0].color->b, stops[0].opacity);
			
      removeStencil(glinfo, x_R0, y_R0, bbwidth, bbheight);      
      return;
    }

    /*
      Rx = bbox

        R1 ------------------- R2
	   |                  |
           |    C             |
           |                  |
           |                  |
     	R0 -------------------- R3

     */

    // if it doesn't fill up the bbox
    float d_R0C = ::hypot(cx-x_R0, cy-y_R0);
    float d_R1C = ::hypot(cx-x_R0, cy-y_R0-bbheight);
    float d_R2C = ::hypot(cx-x_R0-bbwidth, cy-y_R0-bbheight);
    float d_R3C = ::hypot(cx-x_R0-bbheight, cy-y_R0);
    float d_max=d_R0C;
    if(d_R1C > d_max) d_max=d_R1C;
    if(d_R2C > d_max) d_max=d_R2C;
    if(d_R3C > d_max) d_max=d_R3C;
    // std::cerr << DBGVAR(d_max) << DBGVAR(r) << std::endl;

    glinfo->stencilEnableTop();
    // glDisable(GL_STENCIL_TEST);
    glShadeModel(GL_SMOOTH);
    glPushMatrix();

#if 0
    float incx = x_G0G1;
    float incy = y_G0G1;
#endif

    //transforms
    glPushMatrix();		
		glTranslatef(tx, ty, 0);
		glScalef(sx,sy,1);
		const SVGTransformList& transforms = GETVAL(GradientTransform);
    for(unsigned int i=0; i<transforms.numberOfItems(); ++i) {
      transforms.getItem(i)->glTraverse(svglContext, glinfo);
    }
				
    SVGGradientElement::SVG_SPREADMETHOD spread = gradientInfo.spreadMethod;

    // since we are using SGI's glu, we have to use their quadric,
    // which may be different from other's quadric...
    static svgl::GLUquadric *quadric = new svgl::GLUquadric;

    switch(spread) {

      //    case SVGGradientElement::SVG_SPREADMETHOD_UNKNOWN:
    case SVGGradientElement::SVG_SPREADMETHOD_PAD: {
#if 1
      // std::cerr << DBGVAR(sx) << DBGVAR(sy) << DBGVAR(d_max) << __FL__;
      //glPushMatrix();
#if 1			
      if(d_max>r) {
				// pad
				unsigned int i = stops.size()-1;
				{
					float alpha_save = glinfo->alpha;
					glinfo->alpha *= stops[i].opacity;
					stops[i].color->glPreTraverse(svglContext, glinfo, this, x_R0,y_R0,bbwidth,bbheight);
					glinfo->alpha = alpha_save;
				}
#if 1
				// pad with a holed external circle
				gluDisk(glinfo->gluQuadric, r, d_max, 40, 1);
#else
				// or just fill with a regular rectangle for testing purpose (doesn't work with opacity)
				glBegin(GL_QUADS);
#if 1
				glVertex2f(-d_max, -d_max);
				glVertex2f(+d_max, -d_max);
				glVertex2f(+d_max, +d_max);
				glVertex2f(-d_max, +d_max);
								
#else
				glVertex2f(x_R0, y_R0);
				glVertex2f(x_R0+bbwidth, y_R0);
				glVertex2f(x_R0+bbwidth, y_R0+bbheight);
				glVertex2f(x_R0, y_R0+bbheight);
#endif
				glEnd();
#endif
      }
#endif      
      for(unsigned int i=0; i<stops.size()-1; ++i) {
				float innerradius = r*stops[i].offset;
				float outerradius = r*stops[i+1].offset;
				// std::cerr << DBGVAR(innerradius) << DBGVAR(outerradius) << __FL__;
				GLdouble innerColor[4];
				innerColor[0] = stops[i].color->r;
				innerColor[1] = stops[i].color->g;
				innerColor[2] = stops[i].color->b;
				innerColor[3] = stops[i].opacity;
				GLdouble outerColor[4];
				outerColor[0] = stops[i+1].color->r;
				outerColor[1] = stops[i+1].color->g;
				outerColor[2] = stops[i+1].color->b;
				outerColor[3] = stops[i+1].opacity;
				svgl::gluShadedDisk(quadric, innerradius, innerColor, outerradius, outerColor, 40, 1);
      }
      //glPopMatrix();
#endif
    }
      break;


    case SVGGradientElement::SVG_SPREADMETHOD_REPEAT: {
#if 1
      std::cerr << "spreadmethod 'repeat' for radial gradient not implemented" __FILE__ ":" << __LINE__ <<std::endl;
#else
      while(t_FG<t_LG) {
	for (unsigned int i=0; i!=colors.size(); ++i) {
	  //	  glColor4f(colors[i].r, colors[i].g, colors[i].b, opacities[i]);
	  {
	    float alpha_save = glinfo->alpha;
	    glinfo->alpha *= opacities[i];
	    colors[i]->glPreTraverse(svglContext, glinfo,this,x_R0,y_R0,bbwidth,bbheight);
	    glinfo->alpha = alpha_save;
	  }

	  float x,y;
	  float offset = offsets[i];

	  x = x_R1p+offset*incx;
	  y = y_R1p+offset*incy;
	  //std::cerr << DBGVAR(x) << DBGVAR(y) << std::endl;
	  glVertex2f(x, y);
	  
	  x = x_R3p+offset*incx;
	  y = y_R3p+offset*incy;
	  //std::cerr << DBGVAR(x) << DBGVAR(y) << std::endl;
	  glVertex2f(x, y);
	}
	x_R1p+=incx;
	y_R1p+=incy;
	x_R3p+=incx;
	y_R3p+=incy;

	t_FG+=1;
      }
#endif
    }
      break;
    case SVGGradientElement::SVG_SPREADMETHOD_REFLECT: {
#if 1
      std::cerr << "spreadmethod 'reflect' for radial gradient not implemented" __FILE__ ":" << __LINE__ <<std::endl;
#else
      bool reverse=false;
      if ( static_cast<int>(t_FG)%2!=0 ) {
	reverse = !reverse;
      }
      while(t_FG<t_LG) {
	for (unsigned int i=0; i!=colors.size(); ++i) {
	  int cindex = i;
	  float offset;
	  if (reverse) {
	    cindex = colors.size()-i;
	  }

	  {
	    float alpha_save = glinfo->alpha;
	    glinfo->alpha *= opacities[cindex];
	    colors[cindex]->glPreTraverse(svglContext, glinfo,this,x_R0,y_R0,bbwidth,bbheight);
	    glinfo->alpha = alpha_save;
	  }
	  //glColor4f(colors[cindex].r, colors[cindex].g, colors[cindex].b, opacities[cindex]);

	  float x,y;
	  if (reverse) {
	    offset = 1 - offsets[cindex];
	  }
	  else
	    offset = offsets[cindex];

	  x = x_R1p+offset*incx;
	  y = y_R1p+offset*incy;
	  //std::cerr << DBGVAR(x) << DBGVAR(y) << std::endl;
	  glVertex2f(x, y);
	  
	  x = x_R3p+offset*incx;
	  y = y_R3p+offset*incy;
	  //std::cerr << DBGVAR(x) << DBGVAR(y) << std::endl;
	  glVertex2f(x, y);
	}
	x_R1p+=incx;
	y_R1p+=incy;
	x_R3p+=incx;
	y_R3p+=incy;

	reverse = !reverse;
	t_FG+=1;
      }
#endif
      break;
    }
    case SVGGradientElement::SVG_SPREADMETHOD_UNKNOWN:
    default:
      STDDBG;
      break;
    }

    glPopMatrix();    
    glShadeModel(GL_FLAT);

    //transforms
    glPopMatrix();
    
		glinfo->stencilPop();

#if 0
    //    glinfo->stencilEnableTop();
    //glDisable(GL_STENCIL_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glColor3f(.5,.5,.5);
#endif
    glBegin(GL_QUADS);
#if 0
		glVertex2f(-d_max, -d_max);
		glVertex2f(+d_max, -d_max);
		glVertex2f(+d_max, +d_max);
		glVertex2f(-d_max, +d_max);
				
#else
    glVertex2f(x_R0, y_R0);
    glVertex2f(x_R0+bbwidth, y_R0);
    glVertex2f(x_R0+bbwidth, y_R0+bbheight);
    glVertex2f(x_R0, y_R0+bbheight);
#endif
    glEnd();

    glinfo->stencilEnableTop();

    if(gradientInfo.units==SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
      //glinfo->popAndResetView();
    }

		//glinfo->stencilEnableTest();


  }


}














