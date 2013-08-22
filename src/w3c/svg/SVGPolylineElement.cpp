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
#include <w3c/svg/SVGPolylineElement.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/bezierpath.hpp>
#include <svgl/SimpleShapeTraversal.hpp>
#include <agg/agg_path_storage.h>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// ::hypot
#include <math.h>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

namespace svg {

  void
  SVGPolylineElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    const SVGPointList& pl = GET_SIMPLE_VAL(Points);
    if (pl.numberOfItems()<2)
      return;

    svgl::glTraverseSimpleShape(this, svglContext, glinfo);
  }

  void
  SVGPolylineElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    const SVGPointList& pl = GET_SIMPLE_VAL(Points);
    if (pl.numberOfItems()<2)
      return;

    svgl::pickTraverseSimpleShape(this, svglContext, glinfo);
  }

  void
  SVGPolylineElement::glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    const SVGPointList& pl = GET_SIMPLE_VAL(Points);
    if (pl.numberOfItems()<2)
      return;

    svgl::glTraverseBoundingBoxSimpleShape(this, svglContext, glinfo);
  }

  void
  SVGPolylineElement::getBoundingBox(svgl::Context* svglContext, svgl::GLInfo* glinfo, float* bbx, float* bby, float* bbwidth, float* bbheight)
  {
    *bbx=*bby=*bbwidth=*bbheight=0;
    const SVGPointList& pl = GET_SIMPLE_VAL(Points);
    if (pl.numberOfItems()<2) {
      return;
    }

    const SVGPoint& p = pl.getItem(0);
    float xmin=p.GET_SIMPLE_VAL(X), xmax=xmin;
    float ymin=p.GET_SIMPLE_VAL(Y), ymax=ymin;
    for (unsigned int i=1; i<pl.numberOfItems(); ++i) {
      const SVGPoint& p = pl.getItem(i);
      const float& x = p.GET_SIMPLE_VAL(X);
      const float& y = p.GET_SIMPLE_VAL(Y);
      if(x>xmax)
	xmax=x;
      else if(x<xmin)
	xmin=x;
      if(y>ymax)
	ymax=y;
      else if(y<ymin)
	ymin=y;
    }

    *bbx = xmin;
    *bby = ymin;
    *bbwidth = xmax-xmin;
    *bbheight = ymax-ymin;
  }

  void
  SVGPolylineElement::asBezier(svgl::Context * svglContext, svgl::GLInfo* glinfo, BezierPath* bezier)
  {
    const SVGPointList& pl = GET_SIMPLE_VAL(Points);

    if (pl.numberOfItems()<2)
      return;

    const SVGPoint& p = pl.getItem(0);
    bezier->move_to(p.GET_SIMPLE_VAL(X), p.GET_SIMPLE_VAL(Y));
    for (unsigned int i=1; i<pl.numberOfItems(); ++i) {
      const SVGPoint& p = pl.getItem(i);
      bezier->line_to(p.GET_SIMPLE_VAL(X), p.GET_SIMPLE_VAL(Y));
    }

  }

	void
	SVGPolylineElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage* ppath)
  {
    const SVGPointList& pl = GET_SIMPLE_VAL(Points);
		
    if (pl.numberOfItems()<2)
      return;
		
    const SVGPoint& p = pl.getItem(0);
    ppath->move_to(p.GET_SIMPLE_VAL(X), p.GET_SIMPLE_VAL(Y));
    for (unsigned int i=1; i<pl.numberOfItems(); ++i) {
      const SVGPoint& p = pl.getItem(i);
      ppath->line_to(p.GET_SIMPLE_VAL(X), p.GET_SIMPLE_VAL(Y));
    }

  }		
}
