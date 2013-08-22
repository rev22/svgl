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
#include <w3c/svg/SVGPathElement.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/SimpleShapeTraversal.hpp>
#include <svgl/bezierpath.hpp>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

namespace svg {

  SVGPathElement::SVGPathElement(dom::Document * doc)
    : SVGElement(doc, SVGPathElement_element_string), _bezier(0), _glFillList(0), _glStrokeList(0)
  {
  }

  void
  SVGPathElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    const SVGPathSegList& pl = GET_SIMPLE_VAL(PathSegList);
    if (pl.numberOfItems()<2)
      return;

    svgl::glTraverseSimpleShape(this, svglContext, glinfo);
  }

  void
  SVGPathElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    const SVGPathSegList& pl = GET_SIMPLE_VAL(PathSegList);
    if (pl.numberOfItems()<2)
      return;

    svgl::pickTraverseSimpleShape(this, svglContext, glinfo);
  }

  void
  SVGPathElement::glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    const SVGPathSegList& pl = GET_SIMPLE_VAL(PathSegList);
    if (pl.numberOfItems()<2)
      return;

    svgl::glTraverseBoundingBoxSimpleShape(this, svglContext, glinfo);
  }

  void
  SVGPathElement::getBoundingBox(svgl::Context* svglContext, svgl::GLInfo* glinfo, float* bbx, float* bby, float* bbwidth, float* bbheight)
  {
    const SVGPathSegList& pl = GET_SIMPLE_VAL(PathSegList);
    if (pl.numberOfItems()<2)
      return;

    if(!_bezier) {
      BezierPath bezier;
      for(unsigned int i=0; i<pl.numberOfItems(); ++i) {
	const SVGPathSeg* p = pl.getItem(i);
	p->updateBezier(bezier);
      }
      _bezier  = new BezierPath;
      bezier.flatten(*_bezier, 1.0);
    }

    BezierPath::Coord cbbx,cbby,cbbwidth,cbbheight;
    {
      BezierPath::Coord x1,y1;
      if (!_bezier->bounds(cbbx,cbby,x1,y1)) {
	std::cerr << "unable to comput bezier bounds" << std::endl;
	return;
      }
      cbbwidth=x1-cbbx;
      cbbheight=y1-cbby;
    }
    
    *bbx = cbbx;
    *bby = cbby;
    *bbwidth = cbbwidth;
    *bbheight = cbbheight;
  }

  void
  SVGPathElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, BezierPath* bezier)
  {
    const SVGPathSegList& pl = GET_SIMPLE_VAL(PathSegList);
    if (pl.numberOfItems()<2)
      return;

    for(unsigned int i=0; i<pl.numberOfItems(); ++i) {
      const SVGPathSeg* p = pl.getItem(i);
      p->updateBezier(*bezier);
    }
  }
	
	void
	SVGPathElement::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage* ppath)
  {
    const SVGPathSegList& pl = GET_SIMPLE_VAL(PathSegList);
    if (pl.numberOfItems()<2)
      return;
		
    for(unsigned int i=0; i<pl.numberOfItems(); ++i) {
      const SVGPathSeg* p = pl.getItem(i);
      p->updateBezier(*ppath);
    }
  }		
	
//++FIXME
float
SVGPathElement::getTotalLength (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPoint*
SVGPathElement::getPointAtLength ( float distance )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

unsigned long
SVGPathElement::getPathSegAtLength ( float distance )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegClosePath*
SVGPathElement::createSVGPathSegClosePath (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegMovetoAbs*
SVGPathElement::createSVGPathSegMovetoAbs ( float x, float y )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegMovetoRel*
SVGPathElement::createSVGPathSegMovetoRel ( float x, float y )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegLinetoAbs*
SVGPathElement::createSVGPathSegLinetoAbs ( float x, float y )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegLinetoRel*
SVGPathElement::createSVGPathSegLinetoRel ( float x, float y )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegCurvetoCubicAbs*
SVGPathElement::createSVGPathSegCurvetoCubicAbs ( float x, float y, float x1, float y1, float x2, float y2 )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegCurvetoCubicRel*
SVGPathElement::createSVGPathSegCurvetoCubicRel ( float x, float y, float x1, float y1, float x2, float y2 )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegCurvetoQuadraticAbs*
SVGPathElement::createSVGPathSegCurvetoQuadraticAbs ( float x, float y, float x1, float y1 )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegCurvetoQuadraticRel*
SVGPathElement::createSVGPathSegCurvetoQuadraticRel ( float x, float y, float x1, float y1 )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegArcAbs*
SVGPathElement::createSVGPathSegArcAbs ( float x, float y, float r1, float r2, float angle, bool largeArcFlag, bool sweepFlag )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegArcRel*
SVGPathElement::createSVGPathSegArcRel ( float x, float y, float r1, float r2, float angle, bool largeArcFlag, bool sweepFlag )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegLinetoHorizontalAbs*
SVGPathElement::createSVGPathSegLinetoHorizontalAbs ( float x )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegLinetoHorizontalRel*
SVGPathElement::createSVGPathSegLinetoHorizontalRel ( float x )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegLinetoVerticalAbs*
SVGPathElement::createSVGPathSegLinetoVerticalAbs ( float y )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegLinetoVerticalRel*
SVGPathElement::createSVGPathSegLinetoVerticalRel ( float y )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegCurvetoCubicSmoothAbs*
SVGPathElement::createSVGPathSegCurvetoCubicSmoothAbs ( float x, float y, float x2, float y2 )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegCurvetoCubicSmoothRel*
SVGPathElement::createSVGPathSegCurvetoCubicSmoothRel ( float x, float y, float x2, float y2 )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegCurvetoQuadraticSmoothAbs*
SVGPathElement::createSVGPathSegCurvetoQuadraticSmoothAbs ( float x, float y )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPathSegCurvetoQuadraticSmoothRel*
SVGPathElement::createSVGPathSegCurvetoQuadraticSmoothRel ( float x, float y )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}
}
