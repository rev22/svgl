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
#include <svgl/GLInfo.hpp>
#include <svgl/BoundingBoxManager.hpp>
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <svgl/debug.hpp>

namespace svgl {
 
  BoundingBoxManager::BoundingBoxManager()
  {
    buf = new GLfloat[bufsize];
  }

  
  BoundingBoxManager::~BoundingBoxManager()
  {
    delete [] buf;
  }


  void
  BoundingBoxManager::glPreTraverse(GLInfo* glinfo)
  {
    glFeedbackBuffer(bufsize, GL_2D, buf);
    glRenderMode(GL_FEEDBACK);
  }

  bool
  BoundingBoxManager::glPostTraverse(GLInfo*, float * xminf, float * yminf, float * xmaxf, float * ymaxf)
  {
    int size = glRenderMode(GL_RENDER);
    bool ok=false;
#if 0
    std::cerr << "size " << size << std::endl;
    for (int i=0; i<size; ++i) {
      std::cerr << i << ": " << buf[i] << std::endl;
    }
#endif

    if(size) {
      //snap_done=true;
      ok=true;
      *xminf = *xmaxf = buf[1];
      *yminf = *ymaxf = buf[2];
	
	// each bbox is a rectangular drawing using diagonals:
	// if the bbox is ABCD (NW, NE, SE, SW) then the drawing is ABCD (NW,NE,SE,SW)

      for(int index=2; index<size; index+=5) {
	if(buf[index+1]<*xminf)
	  *xminf=buf[index+1];
	else if(buf[index+1]>*xmaxf)
	  *xmaxf=buf[index+1];
	if(buf[index+2]<*yminf)
	  *yminf=buf[index+2];
	else if(buf[index+2]>*ymaxf)
	  *ymaxf=buf[index+2];
      }
    }
    return ok;
  }

}
