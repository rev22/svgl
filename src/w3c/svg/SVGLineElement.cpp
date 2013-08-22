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
#include <w3c/svg/SVGLineElement.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/bezierpath.hpp>
#include <svgl/SimpleShapeTraversal.hpp>
#include <agg/agg_path_storage.h>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <math.h>

namespace svg {

  SIMPLE_TRAVERSE(Line)

  void
  SVGLineElement::getBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo, float* bbx, float* bby, float* bbwidth, float* bbheight)
  {
    svg::SVGLength x1l = GETVAL(X1);
    svg::SVGLength y1l = GETVAL(Y1);
    svg::SVGLength x2l = GETVAL(X2);
    svg::SVGLength y2l = GETVAL(Y2);


    *bbx = svglContext->computeWidthLength(x1l);
    *bby = svglContext->computeHeightLength(y1l);
    *bbwidth = svglContext->computeWidthLength(x2l)-*bbx;
    *bbheight = svglContext->computeHeightLength(y2l)-*bby;
  }

  void
  SVGLineElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, BezierPath* bezier)
  {
    svg::SVGLength x1l = GETVAL(X1);
    svg::SVGLength y1l = GETVAL(Y1);
    svg::SVGLength x2l = GETVAL(X2);
    svg::SVGLength y2l = GETVAL(Y2);


    float x1 = svglContext->computeWidthLength(x1l);
    float y1 = svglContext->computeHeightLength(y1l);
    float x2 = svglContext->computeWidthLength(x2l);
    float y2 = svglContext->computeHeightLength(y2l);

    bezier->move_to(x1,y1);
    bezier->line_to(x2,y2);
  }

	void
	SVGLineElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage* ppath)
  {
		svg::SVGLength x1l = GETVAL(X1);
    svg::SVGLength y1l = GETVAL(Y1);
    svg::SVGLength x2l = GETVAL(X2);
    svg::SVGLength y2l = GETVAL(Y2);
		
    float x1 = svglContext->computeWidthLength(x1l);
    float y1 = svglContext->computeHeightLength(y1l);
    float x2 = svglContext->computeWidthLength(x2l);
    float y2 = svglContext->computeHeightLength(y2l);
		
		agg::path_storage& path = *ppath;
		
		path.move_to(x1,y1);
		path.line_to(x2,y2);		
  }		

  void
  SVGLineElement::glTraverseFill(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
		//SVGStylable::glTraverseFill(svglContext, glinfo);
  }

  void
  SVGLineElement::glTraverseStroke(svgl::Context * svglContext, svgl::GLInfo* glinfo, float sw)
  {
		SVGStylable::glTraverseStroke(svglContext, glinfo, sw);
  }
}
