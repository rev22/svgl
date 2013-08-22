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
#include <svgl/MultiPassManager.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/TextureSnapshot.hpp>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#ifndef  GL_GLEXT_PROTOTYPES
#define  GL_GLEXT_PROTOTYPES
#endif

namespace svgl {

  void
  MultiPassManager::push(svgl::GLInfo* glinfo, float  xminf, float  yminf, float  xmaxf, float  ymaxf)
  {
    xmin=static_cast<int>(xminf-.5);
    xmax=static_cast<int>(xmaxf+.5);
    ymin=static_cast<int>(yminf-.5);
    ymax=static_cast<int>(ymaxf+.5);
    snap_saveunder = new svgl::TextureSnapshot(xmin,ymin,xmax-xmin,ymax-ymin);

    glinfo->pushWindowMode();

#if 1
    // clear bounding box
    glColor4f(1.0,1.0,1.0, 0);
    glBegin(GL_QUADS);
    glVertex2f(xmin,ymin);
    glVertex2f(xmin,ymax);
    glVertex2f(xmax,ymax);
    glVertex2f(xmax,ymin);
    glEnd();
#endif

#if 0
    // frame around the box
    glColor3f(1,.5,.5);
    glBegin(GL_LINE_LOOP);
    glVertex2f(xmin,ymin);
    glVertex2f(xmin,ymax);
    glVertex2f(xmax,ymax);
    glVertex2f(xmax,ymin);
    glEnd();
#endif

#if 1
#if 0
    if(!glinfo->has_GL_ARB_imaging())
#endif
    {
      // push to know where pixels have been written.
      //glinfo->stencilEnableTopAndPush();
      glinfo->stencilPush();
#if 1
      glBegin(GL_QUADS);
      glVertex2f(xmin,ymin);
      glVertex2f(xmin,ymax);
      glVertex2f(xmax,ymax);
      glVertex2f(xmax,ymin);
      glEnd();
#endif
      glinfo->stencilEnableTop();
      
      //glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
#endif

    glinfo->popWindowMode();

  }

  void
  MultiPassManager::pop(svgl::GLInfo* glinfo)
  {
    glinfo->pushWindowMode();
#if 1
    glinfo->stencilEnableTop();
#if 1
    // ignore alpha values between 0.0 and 1.0
    // for written pixels only (hence the above stencilPush)
    //glinfo->stencilDisableTest();
    //glColor4f(1,1,1,glinfo->alpha);
    glColor4f(1,1,1, glinfo->alpha);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
    glBegin(GL_QUADS);
    glVertex2f(xmin,ymin);
    glVertex2f(xmin,ymax);
    glVertex2f(xmax,ymax);
    glVertex2f(xmax,ymin);
    glEnd();
    glEnable(GL_BLEND);
    //glinfo->stencilEnableTest();
#endif

#if 1
    glColor4f(0,1,1,1);
    glinfo->stencilPop();
    //glColorMask(GL_TRUE,GL_TRUE,GL_TRUE, GL_TRUE);
    glBegin(GL_QUADS);
    glVertex2f(xmin,ymin);
    glVertex2f(xmin,ymax);
    glVertex2f(xmax,ymax);
    glVertex2f(xmax,ymin);
    glEnd();

    glinfo->stencilEnableTop();
#endif
#endif
		
#if 1
    if(snap_saveunder) {
      //glinfo->stencilDisableTest();
      glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      snap_saveunder->pop();
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      //glinfo->stencilEnableTest();
    }
#endif
    glinfo->popWindowMode();
     
  }

}

