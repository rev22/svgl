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

#include <svgl/TextureSnapshot.hpp>
#include <svgl/debug.hpp>
#if defined (__APPLE__)
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

namespace svgl {

  TextureSnapshot::TextureSnapshot(int x, int y, int width, int height, GLenum format)
    : _x(x), _y(y), _width(width), _height(height), _format(format)
  {
#if 1
	_target = GL_TEXTURE_RECTANGLE_ARB;
	_texwidth=width;
	_texheight=height;
#else
	_target = GL_TEXTURE_2D;
    _texwidth=_texheight=1;
    while(_texwidth<_width)
      _texwidth<<=1;
    while(_texheight<_height)
      _texheight<<=1
#endif
		  
    //std::cerr << DBGVAL(_texwidth) << DBGVAL(_texheight) << std::endl;

    glGenTextures(1, &_texnum);
    glBindTexture(_target, _texnum);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // glitches appear otherwise...
    glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    //glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    // std::cerr << "snapshot at " << DBGVAL(_x) << DBGVAL(_y) << DBGVAL(_texwidth) << DBGVAL(_texheight) << std::endl;
	//glReadBuffer(GL_FRONT);
	glTexParameteri(_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glCopyTexImage2D(_target, 0, _format, _x, _y, _texwidth, _texheight, 0);

#if 0
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glPushMatrix();
    glTranslatef(30, -30, 0);
#if 1
    glRasterPos2i(_x,_y);
    //glCopyPixels(_x,_y, _texwidth, _texheight, GL_COLOR);
    glCopyPixels(_x,_y, _width, _height, GL_COLOR);
    glTranslatef(0, -_texheight, 0);
#endif
#if 0
    pop();
#endif
    glPopMatrix();
    glPopAttrib();
#endif
    // #define UNPROJECT /* only for debugging purpose with gray rectangle and glCopy... */
    /*
#ifdef UNPROJECT
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,450,0,450,-1,1);
    glMatrixMode(GL_MODELVIEW);
#endif

#if 0
    unsigned char *pixels = new unsigned char[_texwidth*_texheight*3];
    memset(pixels, 128, _texwidth*_texheight*3);
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(_x+_texwidth+100,_y);
    glDrawPixels(_texwidth,_texheight, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glPopMatrix();
    delete [] pixels;
#endif
#if 0
    glRasterPos2i(_x+200,_y);
    //    glDisable(GL_BLEND);
    glCopyPixels(_x,_y, _texwidth, _texheight, GL_COLOR);
    //glEnable(GL_BLEND);
#endif

#ifdef UNPROJECT
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
#endif
    */
  }

  TextureSnapshot::~TextureSnapshot()
  {
    glDeleteTextures(1, &_texnum);
  }

  void
  TextureSnapshot::pop(bool translate)
  {
    glBindTexture(_target, _texnum);
    glEnable(_target);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

    glMatrixMode(GL_TEXTURE);
	glPushMatrix();
    //glLoadIdentity();
	if(_target==GL_TEXTURE_2D)
		glScaled(1./_texwidth, 1./_texheight, 1); 
	
	if(translate)
		glRecti(_x,_y,_x+_width,_y+_height);
	else
		glRecti(0,0,_width,_height);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	
    glDisable(_target);
	
#if 0
	glColor3d(0,0,1);
	glBegin(GL_LINE_LOOP);
    glVertex2f(0,0);
	glVertex2f(0, _height);
    glVertex2f(_width, _height);
    glVertex2f(_width, 0);	
    glEnd();
#endif

  }

  TextureSnapshot*
  TextureSnapshot::copy()
  {
    pop();
    TextureSnapshot * clone = new TextureSnapshot(_x,_y,_width,_height,_format);
    return clone;
  }


}
