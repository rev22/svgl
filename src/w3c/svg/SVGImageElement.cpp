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
#include <w3c/svg/SVGImageElement.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/bezierpath.hpp>
#include <svgl/SimpleShapeTraversal.hpp>
#include <svgl/ImageManager.hpp>
#include <svgl/ExternalEntityManager.hpp>
#include <agg/agg_path_storage.h>
#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

namespace svg {

  SIMPLE_TRAVERSE(Image)

  void
  SVGImageElement::getBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo, float* bbx, float* bby, float* bbwidth, float* bbheight)
  {
    svg::SVGLength xl = GETVAL(X);
    svg::SVGLength yl = GETVAL(Y);
    svg::SVGLength widthl = GETVAL(Width);
    svg::SVGLength heightl = GETVAL(Height);


    *bbx = svglContext->computeWidthLength(xl);
    *bby = svglContext->computeHeightLength(yl);
    *bbwidth = svglContext->computeWidthLength(widthl);
    *bbheight = svglContext->computeHeightLength(heightl);
  }

  void
  SVGImageElement::asBezier(svgl::Context * svglContext, svgl::GLInfo* glinfo, BezierPath* bezier)
  {
    svg::SVGLength xl = GETVAL(X);
    svg::SVGLength yl = GETVAL(Y);
    svg::SVGLength widthl = GETVAL(Width);
    svg::SVGLength heightl = GETVAL(Height);

    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);
    float width = svglContext->computeWidthLength(widthl);
    float height = svglContext->computeHeightLength(heightl);

    bezier->move_to(x,y);
    bezier->line_to(x+width,y);
    bezier->line_to(x+width,y+height);
    bezier->line_to(x,y+height);
    bezier->close_path();
  }
	
	void
	SVGImageElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage* ppath)
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


  void
  SVGImageElement::glTraverseFill(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    svg::SVGLength xl = GETVAL(X);
    svg::SVGLength yl = GETVAL(Y);
    svg::SVGLength widthl = GETVAL(Width);
    svg::SVGLength heightl = GETVAL(Height);

    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);

    bool widthb = (widthl.GET_SIMPLE_VAL(UnitType) != SVGLength::SVG_LENGTHTYPE_UNKNOWN);
    bool heightb = (heightl.GET_SIMPLE_VAL(UnitType) != SVGLength::SVG_LENGTHTYPE_UNKNOWN);

    float width = svglContext->computeWidthLength(widthl);
    float height = svglContext->computeHeightLength(heightl);

    dom::string ref = GETVAL(Href);
    dom::string tmp  = svglContext->externalEntityManager->getFileName(ref, getOwnerDocument());
    svgl::Image* image = svglContext->imageManager->get(tmp);

    if(!widthb)
      width = image->getWidth();
    if(!heightb)
      height = image->getHeight();

    //std::cerr << DBGVAR(x) << DBGVAR(y) << DBGVAR(width) << DBGVAR(height) << __FL__;

    glPushMatrix();

    if(image) {
#if 0
      image->glEnableTexture();
      float iw=image->getTexCoordWidth(), ih=image->getTexCoordHeight();
      glEnable(GL_TEXTURE_2D);
      glBegin(GL_QUADS);
      glTexCoord2f(0,0);   glVertex2f(x,y);
      glTexCoord2f(iw,0);  glVertex2f(x+width,y);
      glTexCoord2f(iw,ih); glVertex2f(x+width,y+height);
      glTexCoord2f(0,ih);  glVertex2f(x,y+height);
      glEnd();
      glDisable(GL_TEXTURE_2D);
#else
      image->glDisplay(x,y,width,height);
#endif
    }

    glPopMatrix();

  }

  void
  SVGImageElement::glTraverseStroke(svgl::Context * svglContext, svgl::GLInfo* glinfo, float sw)
  {
  }


}



