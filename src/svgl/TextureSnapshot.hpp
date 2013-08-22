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
#ifndef __svgl_TextureSnapshot__
#define __svgl_TextureSnapshot__

#include <svgl/config.hpp>
#ifdef WIN32
#include <windows.h>
#endif
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#if SVGL_USE_GC
#include <gc_cpp.h>
#endif

namespace svgl {

  class TextureSnapshot
#if SVGL_USE_GC
    : public gc_cleanup
#endif
  {
  public:
    TextureSnapshot(int x, int y, int width, int height, GLenum format=GL_RGBA);
    ~TextureSnapshot();
    void pop(bool translate=true);

    TextureSnapshot* copy();
    GLenum getFormat() const { return _format; }

  private:
    int _x,_y,_width,_height;
    int _texwidth, _texheight;
	GLenum _target; // GL_TEXTURE_2D or GL_TEXTURE_RECTANGLE_EXT
    GLenum _format;
    GLuint _texnum;
  };


}

#endif // __svgl_TextureSnapshot__
