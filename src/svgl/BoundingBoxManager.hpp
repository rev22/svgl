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
#ifndef __svgl__BoundingBoxManager__
#define __svgl__BoundingBoxManager__

#include <svgl/config.hpp>
#ifdef WIN32
#include <windows.h>
#endif
#if defined (__APPLE__) 
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace svgl {

  class GLInfo;

  class BoundingBoxManager {
  public:
    BoundingBoxManager();
    ~BoundingBoxManager();
    void glPreTraverse(GLInfo*);
    bool glPostTraverse(GLInfo*, float * xmin, float * ymin, float * xmax, float * ymax);
  private:
    GLfloat* buf;
    //static const int bbox_size = (1+2*2)*4;
    //static const int bufsize = bbox_size*15;
    enum { bbox_size = (1+2*2)*4,
	   bufsize = bbox_size*15};


  };

 
}

#endif // __svgl__BoundingBoxManager__
