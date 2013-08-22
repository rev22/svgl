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
#include <w3c/svg/SVGCircleElement.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/SimpleShapeTraversal.hpp>
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <agg/agg_path_storage.h>
#include <agg/agg_ellipse.h>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

namespace svg {

#if 1
  
  SIMPLE_TRAVERSE(Circle)

#else
  void
  SVGCircleElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    svgl::glTraverseSimpleShape(this, svglContext, glinfo);
  }

  void
  SVGCircleElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    svgl::pickTraverseSimpleShape(this, svglContext, glinfo);
  }

  void
  SVGCircleElement::glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    svgl::glTraverseBoundingBoxSimpleShape(this, svglContext, glinfo);
  }
#endif

  void
  SVGCircleElement::getBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo, float* bbx, float* bby, float* bbwidth, float* bbheight)
  {
    svg::SVGLength cxl = GETVAL(Cx);
    svg::SVGLength cyl = GETVAL(Cy);
    svg::SVGLength rl = GETVAL(R);

    float cx = svglContext->computeWidthLength(cxl);
    float cy = svglContext->computeHeightLength(cyl);
    float r = svglContext->computeHypotLength(rl);

    *bbx = cx-r;
    *bby = cy-r;
    *bbwidth = *bbheight = 2*r;
  }


  void
  SVGCircleElement::asBezier(svgl::Context * svglContext, svgl::GLInfo* glinfo, BezierPath* bezier)
  {
  }
	
	void
	SVGCircleElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage* ppath)
  {
    svg::SVGLength cxl = GETVAL(Cx);
    svg::SVGLength cyl = GETVAL(Cy);
    svg::SVGLength rl = GETVAL(R);
		
    double cx = svglContext->computeWidthLength(cxl);
    double cy = svglContext->computeHeightLength(cyl);
    double r = svglContext->computeHypotLength(rl);
		
		agg::ellipse e(cx,cy,r,r,1);
		e.approximation_scale(1);
		ppath->add_path(e);
  }		
}
