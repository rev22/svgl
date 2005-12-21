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
#include <svgl/GLInfo.hpp>
#if defined (__APPLE__)
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif
#include <w3c/svg/SVGPoint.hpp>
#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

#include <cmath>

#ifndef CALLBACK
#define CALLBACK
#endif

#define CLIP_WITH_STENCIL

namespace svg {
  class SVGElement;
}

namespace svgl {

  static void CALLBACK
  tessBegin(GLenum type)
  {
    glBegin(type);
  }

  static void CALLBACK
  tessEnd()
  {
    glEnd();
  }

  static void CALLBACK
  tessVertexPolygon(void* data)
  {
    typedef GLfloat ftype;
    ftype * d = reinterpret_cast<ftype*>(data);
    //std::cerr << data << " " << d << " " << d[0] << " " << d[1] << " " << d[2] << " " << d[3] << __FL__;
    ftype e[2];
    e[0] = d[0];
    e[1] = d[2];
    glVertex2fv(e);
  }

#if 0
  static void CALLBACK
  tessVertexPath(void* data)
  {
    double * d = reinterpret_cast<double*>(data);
#if 0
    if (tessdbg)
      std::cerr << d[0] << " " << d[1] << " " << data << std::endl;
#endif
    glVertex2dv(d);
  }
#endif

  static void CALLBACK
  tessError(GLenum err)
  {
    std::cerr << gluErrorString(err) << std::endl;
  }
  
  static void CALLBACK
  tessCombine (GLdouble c[3], void *d[4], GLfloat w[4], void **out)
  {
    // memory leak...
    *out = new char[sizeof(double)*3];
    GLdouble *nv = reinterpret_cast<GLdouble*>(*out);
    nv[0] = c[0];
    nv[1] = c[1];
    nv[2] = c[2];
#if 0
    GLfloat nvf[3];

    nvf[0] = c[0];
    nvf[1] = c[1];
    nvf[2] = c[2];

    std::cerr << "combine " << nv[0] << " " << nv[1] << " " << nv[2] << ", " << nvf[0] << " " << nvf[1] << " " << nvf[2] << __FL__;
#endif
  }

  static void CALLBACK
  tessVertexCombine (GLdouble c[3], void *d[4], GLfloat w[4], void **out)
  {
    // memory leak...
    typedef GLdouble ftype;
    svg::SVGPoint *p = new svg::SVGPoint;
    //*out =&(p->getX().getValue());
    *out =&(p->GET_SIMPLE_VAL(X));
    p->dom_getX().setValue(c[0]);
    p->dom_getY().setValue(c[1]);

#if 0
    GLfloat nvf[3];

    nvf[0] = c[0];
    nvf[1] = c[1];
    nvf[2] = c[2];

    std::cerr << "combine " << *out << " " << nvf[0] << " " << nvf[1] << " " << nvf[2] << __FL__;
#endif
  }


  GLInfo::GLInfo()
  {
#ifdef CLIP_WITH_STENCIL
    stencil_max_depth = 0;
#else
    stencil_max_depth = 255;
#endif
    gluQuadric = gluNewQuadric();
    gluTriangulator = gluNewTess();
    setTessContext();
    _has_GL_ARB_imaging=false;
    reset();
  }



  void
  GLInfo::reset()
  {
    dpi=90.;
    //scaleX=1;
    //scaleY=1;
    zoom=1;
    xpan=ypan=0;
    alpha=1.;
    winWidth=450;
    winHeight=450;
    stencil=0;
    doPick=pick_no;
    _redisplay=false;

    antialiasing=true;
    //aa_primitive = GL_POLYGON_SMOOTH;
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    aa_primitive = GL_MULTISAMPLE_ARB;
#if 0
    glDisable(GL_MULTISAMPLE_ARB); // disable it by default
#endif
    disableAntiAliasing();
  }

  void
  GLInfo::enableAntiAliasing(bool opaque)
  {
    //std::cerr << "enabling antialiasing" << __FL__;
#if 0
    if(opaque) {
      aa_primitive = GL_POLYGON_SMOOTH;
      glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
    }
    else {
      aa_primitive = GL_MULTISAMPLE_ARB;
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
#endif
#if 0
    glEnable(aa_primitive);
#endif
  }

  void
  GLInfo::disableAntiAliasing()
  {
    //std::cerr << "disabling antialiasing" << __FL__;
#if 0
    glDisable(GL_MULTISAMPLE_ARB);
#endif
    glDisable(GL_POLYGON_SMOOTH);
#if 0
    if(aa_primitive==GL_POLYGON_SMOOTH) {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
#endif
  }


/** Taken from GLUT */
int
extensionSupported(const char *extension)
{
  static const GLubyte *extensions = NULL;
  const GLubyte *start;
  GLubyte *where, *terminator;

  /* Extension names should not have spaces. */
  where = (GLubyte *) strchr(extension, ' ');
  if (where || *extension == '\0')
    return 0;

  if (!extensions) {
    extensions = glGetString(GL_EXTENSIONS);
  }
  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string.  Don't be fooled by sub-strings,
     etc. */
  start = extensions;
  for (;;) {
    /* If your application crashes in the strstr routine below,
       you are probably calling glutExtensionSupported without
       having a current window.  Calling glGetString without
       a current OpenGL context has unpredictable results.
       Please fix your program. */
    where = (GLubyte *) strstr((const char *) start, extension);
    if (!where)
      break;
    terminator = where + strlen(extension);
    if (where == start || *(where - 1) == ' ') {
      if (*terminator == ' ' || *terminator == '\0') {
        return 1;
      }
    }
    start = terminator;
  }
  return 0;
}

  void
  GLInfo::glInit()
  {
    _has_GL_ARB_imaging = extensionSupported("GL_ARB_imaging");

    GLint stencil_bits;
    glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);

#ifdef CLIP_WITH_STENCIL
    stencil_max_depth = (1 << stencil_bits)-1;
    // std::cerr << DBGVAR(stencil_max_depth) << __FL__;
#endif

    glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

  }


double
GLInfo::getCurrentScale() const
{
	double currentScale = 1.0;
	// guess the current scale
	{
		float m[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		float a = m[0];
		float b = m[5];
		float c = m[4];
		float d = m[1];
		currentScale = sqrt(a*b-c*d);
		//std::cerr << currentScale << std::endl;
	}
	return currentScale;
}


  void
  GLInfo::setZoomAndPan()
  {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

#ifdef CLIP_WITH_STENCIL
    glOrtho(0,winWidth,winHeight,0, -1,1);
#else
    glOrtho(0,winWidth,winHeight,0, 0, -255);
#endif

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(zoom, zoom, 1);
    //glTranslatef(-xpan+.3, -ypan+.3, 0);
    glTranslatef(-xpan, -ypan, 0);

    //glTranslated(0,0,-255);
  }

  void
  GLInfo::pushAndResetView()
  {
		// TO BE REMOVED
    //glMatrixMode(GL_PROJECTION);
    //glPushMatrix();
    //setZoomAndPan();
  }

  void
  GLInfo::popAndResetView()
  {
    //glMatrixMode(GL_PROJECTION);
    //glPopMatrix();
    //glMatrixMode(GL_MODELVIEW);
  }

  void
  GLInfo::setTessContext()
  {
    typedef GLvoid (CALLBACK *fn_type)(...);
    gluTessCallback(gluTriangulator, GLU_TESS_BEGIN, reinterpret_cast<fn_type>(tessBegin) );
    gluTessCallback(gluTriangulator, GLU_TESS_END, reinterpret_cast<fn_type>(tessEnd));
    gluTessCallback(gluTriangulator, GLU_TESS_ERROR, reinterpret_cast<fn_type>(tessError));
		gluTessCallback(gluTriangulator, GLU_TESS_BEGIN_DATA, reinterpret_cast<fn_type>(0) );
		gluTessCallback(gluTriangulator, GLU_TESS_END_DATA, reinterpret_cast<fn_type>(0));
		gluTessCallback(gluTriangulator, GLU_TESS_VERTEX_DATA, reinterpret_cast<fn_type>(0));				
  }

  void
  GLInfo::setTessPolygonContext()
  {
    setTessContext();
    typedef void (CALLBACK *fn_type)(...);
    gluTessCallback(gluTriangulator, GLU_TESS_VERTEX, reinterpret_cast<fn_type>(tessVertexPolygon));
    gluTessCallback(gluTriangulator, GLU_TESS_COMBINE, reinterpret_cast<fn_type>(tessVertexCombine));				
  }

  void
  GLInfo::setTessPathContext()
  {
    setTessContext();
    typedef void (CALLBACK *fn_type)(...);
    // gluTessCallback(gluTriangulator, GLU_TESS_VERTEX, reinterpret_cast<fn_type>(tessVertexPath));
    gluTessCallback(gluTriangulator, GLU_TESS_VERTEX, reinterpret_cast<fn_type>(glVertex2dv));
    gluTessCallback(gluTriangulator, GLU_TESS_COMBINE, reinterpret_cast<fn_type>(tessCombine));
  }

  //#define DBG_STENCIL

  void
  GLInfo::stencilEnableTest()
  {
#ifdef DBG_STENCIL
    std::cerr << "stencilEnableTest " << DBGVAR(stencil) << __FL__;
#endif
   if(stencil>0) {
#ifdef CLIP_WITH_STENCIL
      glEnable(GL_STENCIL_TEST);
#else
      glEnable(GL_DEPTH_TEST);
#endif
   }
  }

  void
  GLInfo::stencilDisableTest()
  {
#ifdef DBG_STENCIL
    std::cerr << "stencilDisableTest " << DBGVAR(stencil) << __FL__;
#endif
   if(stencil>0) {
#ifdef CLIP_WITH_STENCIL
      glDisable(GL_STENCIL_TEST);
#else
      glDisable(GL_DEPTH_TEST);
#endif
   }
  }

  void
  GLInfo::stencilPush()
  {
#ifdef DBG_STENCIL
    std::cerr << "stencilPush " << DBGVAR(stencil) << __FL__;
#endif
    if(stencil==stencil_max_depth) {
      std::cerr << "stencil push overflow " __FL__;
      return;
    }
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
#ifdef CLIP_WITH_STENCIL
    glStencilFunc(GL_EQUAL, stencil, static_cast<GLuint>(-1));
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
#else
    glTranslated(0,0,1);
#endif
    ++stencil;
    if(stencil==1) {
#ifdef CLIP_WITH_STENCIL
      glEnable(GL_STENCIL_TEST);
#else
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
#endif
    }
  }

  void
  GLInfo::stencilEnableTopAndPush()
  {
    static bool msggiven=false;
#ifdef DBG_STENCIL
    std::cerr << "stencilEnableTopAndPush " << DBGVAR(stencil) << __FL__;
#endif
    if(!msggiven) {
      //std::cerr << "GLInfo::stencilEnableTopAndPush() does not work" << __FL__;
      msggiven=true;
    }
    if(stencil==stencil_max_depth) {
      std::cerr << "stencil push overflow " __FL__;
      return;
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
#ifdef CLIP_WITH_STENCIL
    glStencilFunc(GL_EQUAL, stencil, static_cast<GLuint>(-1));
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
#else
    glTranslated(0,0,1);
#endif
		++stencil;

    if(stencil==1) {
#ifdef CLIP_WITH_STENCIL
      glEnable(GL_STENCIL_TEST);
#else
      glDepthFunc(GL_LESS);
      glEnable(GL_DEPTH_TEST);
#endif
    }
  }

  void
  GLInfo::stencilPop()
  {
#ifdef DBG_STENCIL
    std::cerr << "stencilPop " << DBGVAR(stencil) << __FL__;
#endif
    if(stencil==0) {
      std::cerr << "stencil pop underflow " __FL__;
      return;
    }
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
#ifdef CLIP_WITH_STENCIL
    glStencilFunc(GL_EQUAL, stencil, static_cast<GLuint>(-1));
    glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
#else
    glTranslated(0,0,-1);
    glDepthFunc(GL_GREATER);
#endif
    --stencil;
  }

  void
  GLInfo::stencilEnableTop()
  {
#ifdef DBG_STENCIL
    std::cerr << "stencilEnableTop " << DBGVAR(stencil) << __FL__;
#endif
    if(stencil==0) {
#ifdef CLIP_WITH_STENCIL
      glDisable(GL_STENCIL_TEST);
#else
      glDisable(GL_DEPTH_TEST);
#endif
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
#ifdef CLIP_WITH_STENCIL
    glStencilFunc(GL_EQUAL, stencil, static_cast<GLuint>(-1));
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
#else
    glDepthFunc(GL_EQUAL);
#endif
  }

  void
  GLInfo::stencilEnableTopAndPop()
  {
#ifdef DBG_STENCIL
    std::cerr << "stencilEnableTopAndPop " << DBGVAR(stencil) << __FL__;
#endif
    std::cerr << "GLInfo::stencilEnableTopAndPop() does not work" << __FL__;
    if(stencil==0) {
      std::cerr << "stencil pop underflow " __FL__;
      return;
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
#ifdef CLIP_WITH_STENCIL
    glStencilFunc(GL_EQUAL, stencil, static_cast<GLuint>(-1));
    glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
#else
    glTranslated(0,0,-1);
    glDepthFunc(GL_GEQUAL);
#endif
    --stencil;
  }

  void
  GLInfo::pushPickInfo(svg::SVGElement* e)
  {
    switch(doPick) {
    case pick_this:
      glPushName(reinterpret_cast<GLuint>(e));
      break;
    case pick_no:
    default:
      break;
    }
  }

  void
  GLInfo::popPickInfo()
  {
    switch(doPick) {
    case pick_this:
      glPopName();
      break;
    case pick_no:
    default:
      break;
    }
  }

  void
  GLInfo::pushWindowMode()
  {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,winWidth,0,winHeight,-1,1);
    //glTranslatef(.35,.35,0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(.35,.35,0);
  }

  void
  GLInfo::popWindowMode()
  {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }

  void
  GLInfo::setRedisplay(bool v)
  {
    _redisplay = v;
  }


}

