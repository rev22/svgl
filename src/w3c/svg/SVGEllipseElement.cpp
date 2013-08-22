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
#include <w3c/svg/SVGEllipseElement.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/SimpleShapeTraversal.hpp>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <svgl/gluEllipse.hpp>

#include <agg/agg_path_storage.h>
#include <agg/agg_ellipse.h>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

namespace svg {

  SIMPLE_TRAVERSE(Ellipse)

  void
  SVGEllipseElement::getBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo, float* bbx, float* bby, float* bbwidth, float* bbheight)
  {
    svg::SVGLength cxl = GETVAL(Cx);
    svg::SVGLength cyl = GETVAL(Cy);
    svg::SVGLength rxl = GETVAL(Rx);
    svg::SVGLength ryl = GETVAL(Ry);

    float cx = svglContext->computeWidthLength(cxl);
    float cy = svglContext->computeHeightLength(cyl);
    float rx = svglContext->computeHypotLength(rxl);
    float ry = svglContext->computeHypotLength(ryl);

    *bbx = cx-rx;
    *bby = cy-ry;
    *bbwidth = cx+rx;
    *bbheight = cy+ry;
  }


  void
  SVGEllipseElement::asBezier(svgl::Context * svglContext, svgl::GLInfo* glinfo, BezierPath* bezier)
  {
  }

	void
	SVGEllipseElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage* ppath)
  {
    svg::SVGLength cxl = GETVAL(Cx);
    svg::SVGLength cyl = GETVAL(Cy);
    svg::SVGLength rxl = GETVAL(Rx);
    svg::SVGLength ryl = GETVAL(Ry);
		
    float cx = svglContext->computeWidthLength(cxl);
    float cy = svglContext->computeHeightLength(cyl);
    float rx = svglContext->computeHypotLength(rxl);
    float ry = svglContext->computeHypotLength(ryl);
		
		agg::ellipse e(cx,cy,rx,ry,1);
		e.approximation_scale(1);
		ppath->add_path(e);		
  }		

}
