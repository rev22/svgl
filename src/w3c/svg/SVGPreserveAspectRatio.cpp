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
#include <w3c/svg/SVGPreserveAspectRatio.hpp>
#include <w3c/svg/SVGRect.hpp>
#include <svgl/GLInfo.hpp>

#include <cmath>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>


namespace svg {

  void
  SVGPreserveAspectRatio::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo * glinfo, const SVGRect& viewbox, float width, float height)
  {
#define GETVAL1(x,X)   int x = static_cast<int>(std::floor(viewbox.GET_SIMPLE_VAL(X)));
    GETVAL1(vbx,X);
    GETVAL1(vby,Y);
    GETVAL1(vbwidth,Width);
    GETVAL1(vbheight,Height);
#undef GETVAL1
    _dostencil=false;

    SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO align_ = GET_SIMPLE_VAL(Align);
    SVGPreserveAspectRatio::SVG_MEETORSLICE meetOrSlice_ = GET_SIMPLE_VAL(MeetOrSlice);

    float sx=1,sy=1;

    glPushMatrix();

    if(vbwidth!=0 && vbheight!=0) {

      float pretx=0, prety=0;
      float posttx=-vbx, postty=-vby;

      float wratio = width/vbwidth;
      float hratio = height/vbheight;
      float maxratio = wratio;
      float minratio = hratio;
      if(maxratio<minratio) {
	maxratio = hratio;
	minratio = wratio;
      }

      switch(meetOrSlice_) {
      default:
      case SVGPreserveAspectRatio::SVG_MEETORSLICE_MEET:
	sx = sy = minratio;
	break;
      case SVGPreserveAspectRatio::SVG_MEETORSLICE_SLICE:
	sx = sy = maxratio;
	_dostencil=true;
	break;
      }

#define XMIN pretx += 0; posttx += 0;
#define XMID pretx += width/2; posttx += (-vbwidth)/2;
#define XMAX pretx += width; posttx += -vbwidth;

#define YMIN prety += 0; postty += 0;
#define YMID prety += height/2; postty += (-vbheight)/2;
#define YMAX prety += height; postty += -vbheight;


      switch(align_) {
      default:
      case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_NONE:
	sx = wratio;
	sy = hratio;
	break;
      case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMINYMIN:
	XMIN;
	YMIN;
	break;
      case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMIN:
	XMID;
	YMIN;
	break;
      case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMIN:
	XMAX;
	YMIN;
	break;
      case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMINYMID:
	XMIN;
	YMID;
	break;
      case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMID:
	XMID;
	YMID;
	break;
      case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMID:
	XMAX;
	YMID;
	break;
      case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMINYMAX:
	XMIN;
	YMAX;
	break;
      case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMAX:
	XMID;
	YMAX;
	break;
      case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMAX:
	XMAX;
	YMAX;
	break;
#if 0
      default:
	std::cerr << DBGVAR(align_) << DBGVAR(meetOrSlice_) << __FL__;
	break;
#endif
    }

#undef XMIN
#undef XMID
#undef XMAX

#undef YMIN
#undef YMID
#undef YMAX

      if(_dostencil) {
	glinfo->stencilPush();
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, height);
	glVertex2f(width, height);
	glVertex2f(width, 0);
	glEnd();
	glinfo->stencilEnableTop();
      }

      //std::cerr << DBGVAR(pretx) << DBGVAR(prety) << DBGVAR(sx) << DBGVAR(sy) << DBGVAR(posttx) << DBGVAR(postty) << __FL__;
      //      glPushMatrix(); // at the top
      glTranslatef( pretx, prety, 0);
      glScalef( sx, sy, 1);
      glTranslatef( posttx, postty, 0);
      
    }
    //glinfo->scaleX *= sx;
    //glinfo->scaleY *= sy;
    //_sx=sx;
    //_sy=sy;

  }

  void
  SVGPreserveAspectRatio::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo * glinfo, const SVGRect& viewbox, float width, float height)
  {
    glPopMatrix();

    if(_dostencil) {
       glinfo->stencilPop();
       glBegin(GL_QUADS);
       glVertex2f(0, 0);
       glVertex2f(0, height);
       glVertex2f(width, height);
       glVertex2f(width, 0);
       glEnd();
       glinfo->stencilEnableTop();
    }

    //glinfo->scaleX /= _sx;
    //glinfo->scaleY /= _sy;
  }

#if 0
  void
  cullingPreTraverse(svgl::Context * svglContext, svgl::GLInfo * glinfo, const SVGRect& viewbox, float width, float height)
  {
#define GETVAL1(x,X)   int x = static_cast<int>(std::floor(viewbox.get##X().getValue()));
    GETVAL(vbx,X);
    GETVAL(vby,Y);
    GETVAL(vbwidth,Width);
    GETVAL(vbheight,Height);
#undef GETVAL1

    svgl::BoundingBoxManager bboxManager;
    bboxManager.glPreTraverse(glinfo);
    glPushMatrix();
    glTraverseCullingChildren(svglContext, glinfo, );
    glPopMatrix();
    float xminf,yminf,xmaxf,ymaxf;
    snap_done = bboxManager.glPostTraverse(glinfo, &xminf, &yminf, &xmaxf, &ymaxf);
  }

  void
  cullingPostTraverse(svgl::Context * svglContext, svgl::GLInfo * glinfo, const SVGRect& viewbox, float width, float height)
  {
  }
#endif

}
