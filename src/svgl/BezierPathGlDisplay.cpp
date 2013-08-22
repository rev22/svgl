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
#include <svgl/bezierpath.hpp>
#ifdef WIN32
#include <windows.h>
#endif
#if defined (__APPLE__) || defined (OS_IS_MACOSX) 
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <svgl/GLInfo.hpp>
#include <svgl/Context.hpp>
#include <svgl/debug.hpp>
#include <w3c/css/RGBColor.hpp>

#include <svgl/BoundingBoxManager.hpp>
#include <svgl/MultiPassManager.hpp>


namespace svg {
  class SVGElement;
}

namespace svgl {

  void
  glTraverseStrokeOne(svgl::GLInfo* glinfo, const BezierPath& bezier)
  {
    bool begun=false;      
    float x0=0, y0=0;
    STDDBG;
    for (BezierPath::const_iterator i = bezier.begin();
	 i!=bezier.end();
	 ++i) {
      switch ( (*i).type() ) {

      case BezierPath::move: {
	if (begun) {
	  glEnd();
	}
	begun=false;
      }
	break;
	
      case BezierPath::line: {
	const BezierPath::Point& p1 = (*i)[1];
	
	if(!begun) {
	  begun=true;
	  const BezierPath::Point& p0 = (*i)[0];
	  glBegin(GL_LINE_STRIP);
	  x0 = p0.x();
	  y0 = p0.y();
	  glVertex2f(p0.x(), p0.y());
	}
	
	glVertex2f(p1.x(), p1.y());
      }
	break;
      case BezierPath::close: {
	const BezierPath::Point& p1 = (*i)[1];
	glVertex2f(p1.x(), p1.y());
	glVertex2f(x0, y0);
      }
	break;
      default:
	std::cerr << __FILE__ ":" << __LINE__ << std::endl;
	break;
      }
    }
    if (begun)
      glEnd();
  }


  void
  glTraverseStroke(svgl::GLInfo* glinfo, const BezierPath& bezier, float strokeWidth)
  {
    bool begun=false;      
    float x0=0, y0=0, swx0=0, swy0=0;
    float sw = strokeWidth/2.;
#if 0
    double currentScale = glinfo->getCurrentScale();
    if(currentScale*strokeWidth<=1) {
      glTraverseStrokeOne(glinfo, bezier);
    }
#endif
    for (BezierPath::const_iterator i = bezier.begin();
	 i!=bezier.end();
	 ++i) {
      switch ( (*i).type() ) {

      case BezierPath::move: {
	if (begun) {
	  glEnd();
	}
	begun=false;
      }
      break;

      case BezierPath::line: {
	const BezierPath::Point& p0 = (*i)[0];
	const BezierPath::Point& p1 = (*i)[1];
	BezierPath::Coord dx = p1.x()-p0.x();
	BezierPath::Coord dy = p1.y()-p0.y();

	BezierPath::Coord l = ::hypot(dx, dy);
	if(l==0) {
	  continue;
	}

	BezierPath::Coord xw = dy*sw/l;
	BezierPath::Coord yw = dx*sw/l;

	if(!begun) {
	  begun=true;
	  glBegin(GL_TRIANGLE_STRIP);
	  x0=p0.x();
	  y0=p0.y();
	  swx0=xw;
	  swy0=yw;
	}

	//glEdgeFlag(0);
	glVertex2f(p0.x()-xw, p0.y()+yw);
	//glEdgeFlag(0);
	glVertex2f(p0.x()+xw, p0.y()-yw);
	//glEdgeFlag(0);
	glVertex2f(p1.x()-xw, p1.y()+yw);
	//glEdgeFlag(0);
	glVertex2f(p1.x()+xw, p1.y()-yw);
      }
      break;
      case BezierPath::close: {
	const BezierPath::Point& p0 = (*i)[0];
	const BezierPath::Point& p1 = (*i)[1];

	BezierPath::Coord dx = p1.x()-p0.x();
	BezierPath::Coord dy = p1.y()-p0.y();

	BezierPath::Coord l = ::hypot(dx, dy);
	if(l!=0) {
	  BezierPath::Coord xw = dy*sw/l;
	  BezierPath::Coord yw = dx*sw/l;

	  glVertex2f(p0.x()-xw, p0.y()+yw);
	  glVertex2f(p0.x()+xw, p0.y()-yw);
	  glVertex2f(p1.x()-xw, p1.y()+yw);
	  glVertex2f(p1.x()+xw, p1.y()-yw);
	}
	// last triangle
	if(begun) {
	  glVertex2f(x0-swx0,y0+swy0);
	  glVertex2f(x0+swx0,y0-swy0);
	}

      }
      break;
      default:
	std::cerr << __FILE__ ":" << __LINE__ << std::endl;
	break;
      }
    }
    if (begun)
      glEnd();
  }


  void
  glTraverseFill(svgl::GLInfo* glinfo, const BezierPath& bezier)
  {
    if (is_rectangle(bezier)) {
      BezierPath::Coord min_x, min_y, max_x, max_y;
      bezier.bounds(min_x, min_y, max_x, max_y);
#if 1
      glRectd(min_x, min_y, max_x, max_y);
#else
      glBegin(GL_QUADS);
      glVertex2f(min_x,min_y);
      glVertex2f(min_x,max_y);
      glVertex2f(max_x,max_y);
      glVertex2f(max_x,min_y);
      glEnd();
#endif
      return;
    }
    glinfo->setTessPathContext();

    bool begun=false;
    gluTessBeginPolygon(glinfo->gluTriangulator, 0);
    for (BezierPath::const_iterator i = bezier.begin();
	 i!=bezier.end();
	 ++i) {
      switch ( (*i).type() ) {

      case BezierPath::close: {
	const BezierPath::Point& p = (*i)[1];
	double v[3];
	v[0]=p[0];
	v[1]=p[1];
	v[2]=0;
	BezierPath::Coord *pv = const_cast<BezierPath::Coord*>(&p[0]);
	gluTessVertex(glinfo->gluTriangulator, v, pv);
      }
      break;

      case BezierPath::move: {
	if(begun)
	  gluTessEndContour(glinfo->gluTriangulator);
	gluTessBeginContour(glinfo->gluTriangulator);
	begun=true;

	const BezierPath::Point& p = (*i)[0];
	double v[3];
	v[0]=p[0];
	v[1]=p[1];
	v[2]=0;
	BezierPath::Coord *pv = const_cast<BezierPath::Coord*>(&p[0]);
	gluTessVertex(glinfo->gluTriangulator, v, pv);
      }
      break;

      case BezierPath::line: {
	const BezierPath::Point& p = (*i)[1];
	double v[3];
	v[0]=p[0];
	v[1]=p[1];
	v[2]=0;
	BezierPath::Coord *pv = const_cast<BezierPath::Coord*>(&p[0]);
	gluTessVertex(glinfo->gluTriangulator, v, pv);
      }
      break;
      default:
	std::cerr << __FILE__ ":" << __LINE__ << std::endl;
	break;
      }
    }
    if(begun)
      gluTessEndContour(glinfo->gluTriangulator);
    gluTessEndPolygon(glinfo->gluTriangulator);
  }

}
