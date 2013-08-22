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
#include <w3c/svg/SVGRectElement.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/bezierpath.hpp>
#include <svgl/SimpleShapeTraversal.hpp>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

#include <agg/agg_path_storage.h>

//#include <dyglc/block.hpp>

namespace svg {

  SIMPLE_TRAVERSE(Rect)

#if 0
  class RectFill : public dyglc::block {
  public:
    double x,y,width,height;
    RectFill();
    RectFill(double x_, double y_, double width_, double height_) : x(x_), y(y_), width(width_), height(height_) {}
    virtual void inner_execute();
    virtual void inner_declare();
  };

  void
  RectFill::inner_execute()
  {
    glRectf(x,y,x+width,y+height);
  }

  void
  RectFill::inner_declare()
  {
    //glRectf(x,y,x+width,y+height);
  }

  class RectStroke : public dyglc::block {
  public:
    double x,y,width,height,stroke_width;
    RectStroke();
    RectStroke(double x_, double y_, double width_, double height_, double stroke_width_) : x(x_), y(y_), width(width_), height(height_), stroke_width(stroke_width_) {}

    virtual void inner_execute();
    virtual void inner_declare();
  };

  void
  RectStroke::inner_execute()
  {
    double sw = stroke_width/2.0;
    if (sw == 0)
      return;

    glBegin(GL_QUAD_STRIP);
    // top
    glVertex2f(x-sw,y-sw);
    glVertex2f(x+sw,y+sw);

    glVertex2f(x+sw+width,y-sw);
    glVertex2f(x-sw+width,y+sw);

    glVertex2f(x+sw+width,y+sw+height);
    glVertex2f(x-sw+width,y-sw+height);

    glVertex2f(x-sw,y+sw+height);
    glVertex2f(x+sw,y-sw+height);

    glVertex2f(x-sw,y-sw);
    glVertex2f(x+sw,y+sw);

    glEnd();
  }

  void
  RectStroke::inner_declare()
  {
  }
#endif
	
  void
  SVGRectElement::getBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo, float* bbx, float* bby, float* bbwidth, float* bbheight)
  {
    svg::SVGLength& xl = MYGETVAL(x);
    svg::SVGLength& yl = MYGETVAL(y);
    svg::SVGLength& widthl = MYGETVAL(width);
    svg::SVGLength& heightl = MYGETVAL(height);

    *bbx = svglContext->computeWidthLength(xl);
    *bby = svglContext->computeHeightLength(yl);
    *bbwidth = svglContext->computeWidthLength(widthl);
    *bbheight = svglContext->computeHeightLength(heightl);
  }

  void
  SVGRectElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, BezierPath* bezier)
  {
    svg::SVGLength& xl = MYGETVAL(x);
    svg::SVGLength& yl = MYGETVAL(y);
    svg::SVGLength& widthl = MYGETVAL(width);
    svg::SVGLength& heightl = MYGETVAL(height);

    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);
    float width = svglContext->computeWidthLength(widthl);
    float height = svglContext->computeHeightLength(heightl);

    add_rectangle(*bezier, x, y, width, height);
  }

  void
	SVGRectElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage* ppath)
	{
    svg::SVGLength& xl = MYGETVAL(x);
    svg::SVGLength& yl = MYGETVAL(y);
    svg::SVGLength& widthl = MYGETVAL(width);
    svg::SVGLength& heightl = MYGETVAL(height);
		
    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);
    float width = svglContext->computeWidthLength(widthl);
    float height = svglContext->computeHeightLength(heightl);
		
		agg::path_storage& path = *ppath;
		
		path.move_to(x,y);
		path.line_to(x+width,y);
		path.line_to(x+width,y+height);
		path.line_to(x,y+height);
		path.close_polygon();		
	}

#if 0
  void
  SVGRectElement::glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    svg::SVGLength xl = GETVAL(X);
    svg::SVGLength yl = GETVAL(Y);
    svg::SVGLength widthl = GETVAL(Width);
    svg::SVGLength heightl = GETVAL(Height);

    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);
    float width = svglContext->computeWidthLength(widthl);
    float height = svglContext->computeHeightLength(heightl);

    glPushMatrix();
    svgl::GLInfo glinfonew = *glinfo;
    SVGTransformable::glTraverse(svglContext, &glinfonew);

    const css::CSSStyleDeclaration* style = &(getStyle().getValue());
    if (style) {    
      const css::RGBColor * fill = style->getFill();
      const css::RGBColor * stroke = style->getStroke();
      if(fill || stroke) {
	float sw=0;
	if(stroke) {
	  svg::SVGLength strokeWidth = style->getStrokeWidth();
	  sw = svglContext->computeWidthLength(strokeWidth)/2.;
	}

	glBegin(GL_LINE_LOOP);

	glVertex2f(x-sw,y-sw);
	glVertex2f(x+sw+width,y-sw);
	glVertex2f(x+sw+width,y+sw+height);
	glVertex2f(x-sw,y+sw+height);

	glVertex2f(x-sw,y-sw);
	glVertex2f(x+sw+width,y+sw+height);
	glVertex2f(x+sw+width,y-sw);
	glVertex2f(x-sw,y+sw+height);

	glEnd();  
      }
    }

    glPopMatrix();

  }
#endif
}
























