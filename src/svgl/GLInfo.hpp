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
#ifndef __svg_GLInfo__
#define __svg_GLInfo__

#include <svgl/config.hpp>
#if defined (__APPLE__)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

namespace svg {
  class SVGElement;
}

namespace svgl {

  class FontManager;
  class ImageManager;

  class GLInfo {
  public:
    float dpi;
    //float scaleX, scaleY;
    float zoom, xpan, ypan;
    float alpha;
    int stencil;
    int stencil_max_depth;
    int winWidth, winHeight;
    bool _redisplay;
    bool _has_GL_ARB_imaging;
    bool antialiasing;
    GLuint aa_primitive;

    enum Pick {
      pick_no=0,
      pick_this
    };

    Pick doPick;
    // GLuint pickName;
    void pushPickInfo(svg::SVGElement*);
    void popPickInfo();

    GLUquadricObj* gluQuadric;
    GLUtriangulatorObj* gluTriangulator;

    GLInfo();

    // can be called only with a valid GL context
    void glInit();

    void reset();


    bool has_GL_ARB_imaging() const { return _has_GL_ARB_imaging; }
    bool doAntiAliasing() const { return antialiasing; }
    void enableAntiAliasing(bool opaque=false);
    void disableAntiAliasing();

#if 0
    void resetRedisplay() { _postRedisplay=false; }
    void postRedisplay() { _postRedisplay=true; }
    bool getRedisplay() { return _postRedisplay; }
#endif

    double getCurrentScale() const;
    void setZoomAndPan();
    void pushAndResetView();
    void popAndResetView();
    void pushWindowMode();
    void popWindowMode();

    void setTessContext();
    void setTessPolygonContext();
    void setTessPathContext();

    void stencilPush();
    void stencilPop();
    void stencilDisableTest();
    void stencilEnableTest();

    void stencilEnableTopAndPush();

    // needs a better name...
    // specify that if there is a top value in the stencil,
    // then we write only in the pixels with this stencil value
    // usually, one must call it after a stencilPush() and a stencilPop()
    // to set the stencil func...
    void stencilEnableTop();
    // same as above plus it pops the stencil
    // useful when applying a quad for example
    void stencilEnableTopAndPop();

    void setRedisplay(bool);

  };

}

#endif // __svg_GLInfo__

