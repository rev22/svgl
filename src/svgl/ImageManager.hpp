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
#ifndef __ImageManager__
#define __ImageManager__

#include <svgl/config.hpp>
#include <utils/String.hpp>

#if SVGL_USE_GC
#include <utils/gc_alloc.hpp>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <map>

namespace svg {
  class SVGDocument;
}

namespace svgl {

  class ImageManager;

  class Image {
  public:
    Image();
    virtual ~Image();

    typedef unsigned int size_type;
    size_type getWidth() const { return _width; }
    size_type getHeight() const { return _height; }

    virtual void glDisplay(double x, double y, double width, double height) = 0;

  protected:
    size_type _width, _height;

    friend class ImageManager;
  };

  class PixmapImage : public Image {
  public:
    PixmapImage();
    virtual ~PixmapImage();

    virtual void glEnableTexture();
    virtual void glCreateTexture()=0;
    typedef unsigned char value_type;

    float getTexCoordWidth() const;
    float getTexCoordHeight() const;

    virtual void glDisplay(double x, double y, double width, double height);

  protected:
    size_type _texwidth, _texheight;
    value_type* _pixmap;
    GLuint _texnum;
    
  };


  class PNGImage : public PixmapImage {
  public:
  private:
    virtual void glCreateTexture();
    GLenum _internalFormat, _format, _type;
    friend class ImageManager;
  };

#if 0
  class JPEGImage : public PixmapImage {
  };
#endif

  class SVGImage : public Image {
  public:
    SVGImage();
    SVGImage(svg::SVGDocument*);
    ~SVGImage();

    virtual void glDisplay(double x, double y, double width, double height);
    static SVGImage* load(const unicode::String* filename);
  protected:

    svg::SVGDocument * _doc;

  };


  class ImageManager {
  public:
    Image* load(const unicode::String* filename);
    Image* get(const unicode::String* filename);

#if SVGL_USE_GC
    typedef std::map<const unicode::String*, Image*, string_less, gc_alloc<std::pair<const unicode::String*, Image*> > > Images;
#else
    typedef std::map<const unicode::String*, Image*, string_less> Images;
#endif

  private:
    PNGImage* loadPNG(const unicode::String* filename);
    //SVGImage* loadSVG(const unicode::String* filename);
    
    Images _images;
  };

}


#endif // __ImageManager__
