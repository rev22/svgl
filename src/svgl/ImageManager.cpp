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
#include <svgl/ImageManager.hpp>
#include <utils/String.hpp>
#include <png.h>

#include <svgl/debug.hpp>
#include <svgl/Parser.hpp>
#include <w3c/svg/SVGDocument.hpp>

namespace svgl {

  Image::Image()
    : _width(0), _height(0)
  {
  }
  
  Image::~Image()
  {
  }

  PixmapImage::PixmapImage()
    : _texwidth(0), _texheight(0), _pixmap(0)
  {
  }
  
  PixmapImage::~PixmapImage()
  {
    glDeleteTextures(1, &_texnum);
    delete [] _pixmap;
  }

  float
  PixmapImage::getTexCoordWidth() const
  {
    if(_texheight)
      return _width/(float)_texwidth;
    else
      return 0;
  }

  float
  PixmapImage::getTexCoordHeight() const
  {
    if(_texheight)
      return _height/(float)_texheight;
    else
      return 0;
  }

  void
  PixmapImage::glEnableTexture()
  {
    glBindTexture(GL_TEXTURE_2D, _texnum);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

  }

  void
  PNGImage::glCreateTexture()
  {
    glGenTextures(1, &_texnum);
    glBindTexture(GL_TEXTURE_2D, _texnum);

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

    //    std::cerr << DBGVAL(_width) << DBGVAL(_texwidth) << DBGVAL(_height) << DBGVAL(_texheight) << " pixmap:" << (void*)_pixmap << std::endl;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
#if 0
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glRasterPos2i(0,0);
    glDrawPixels(_texwidth, _texheight, GL_RGB, GL_UNSIGNED_BYTE, _pixmap);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
#else
    glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _texwidth, _texheight, 0, _format, _type, _pixmap);
#endif
  }

  void
  PixmapImage::glDisplay(double x, double y, double width, double height)
  {
      glEnableTexture();
      float iw=getTexCoordWidth(), ih=getTexCoordHeight();
      glEnable(GL_TEXTURE_2D);
      glBegin(GL_QUADS);
      glTexCoord2f(0,0);   glVertex2f(x,y);
      glTexCoord2f(iw,0);  glVertex2f(x+width,y);
      glTexCoord2f(iw,ih); glVertex2f(x+width,y+height);
      glTexCoord2f(0,ih);  glVertex2f(x,y+height);
      glEnd();
      glDisable(GL_TEXTURE_2D);
  }

  Image*
  ImageManager::get(const unicode::String* filename)
  {
    //Image* res = _images[filename];
    Images::const_iterator i = _images.find(filename);
    if(i != _images.end())
      return i->second;

    Image * res = load(filename);

    _images.insert(Images::value_type(filename, res));

    return res;
  }

  Image*
  ImageManager::load(const unicode::String* filename)
  {
    Image * res=0;
    {
      PNGImage* respng = loadPNG(filename);
 
      if(respng) {
	respng->glCreateTexture();
	res = respng;
      }
    }

    if(!res) {
      SVGImage * ressvg = SVGImage::load(filename);
      if(ressvg) {
	res = ressvg;
      }
    }

    if(res) {
      _images.insert(Images::value_type(filename,res));
    } else {
      std::cerr << "unknown format for image " << filename << __FL__;
    }
    return res;
  }

  PNGImage*
  ImageManager::loadPNG(const unicode::String* filename)
  {
    png_structp png_ptr;
    png_infop info_ptr;
    png_infop end_info;

    if(!filename)
      return 0;

    const char* f_c_str = filename->transcode();
    FILE * file = ::fopen(f_c_str, "r");
    if(!file) {
      std::cerr << "image \"" << filename << "\" is not a png image" << __FL__;
      return 0;
    }

#if 1
    {
      png_byte header[9];
      fread(header, 1, 8, file);
#if 0
      header[8]=0;
      fprintf(stderr, "%s\n", header);
#endif
      if(png_sig_cmp(header, 0, 8)) {
	//STDDBG;
	return 0;
      }
    }
#endif
    png_ptr = png_create_read_struct
      (PNG_LIBPNG_VER_STRING, 0,
       0, 0);
    if (!png_ptr) {
      STDDBG;
      return 0;
    }
    
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
      STDDBG;
      png_destroy_read_struct(&png_ptr,
			      (png_infopp)NULL, (png_infopp)NULL);
      return 0;
    }
    
    end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
      STDDBG;
      png_destroy_read_struct(&png_ptr, &info_ptr,
			      (png_infopp)NULL);
      return 0;
    }
    
    if (setjmp(png_ptr->jmpbuf)) {
      STDDBG;
      png_destroy_read_struct(&png_ptr, &info_ptr,
			      &end_info);
      return 0;
    }
    
    png_init_io(png_ptr, file);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    PNGImage *res = new PNGImage;

    Image::size_type width,height;
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    /*  fprintf(stderr, "%d %d \n", thetex->width, thetex->height);*/
    Image::size_type texwidth=8,texheight=1;

    while(texwidth<=width)
      texwidth<<=1;

    while(texheight<=height)
      texheight<<=1;

    res->_width=width;
    res->_height=height;

    res->_texwidth=texwidth;
    res->_texheight=texheight;
    
    int depth = png_get_bit_depth(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);
    int bpp;

    bool ok=false;
    switch (color_type) {
    case PNG_COLOR_TYPE_RGB:
      //STDDBG;
      res->_format = GL_RGB;
      switch(depth) {
      case 8: res->_internalFormat = GL_RGB; res->_type = GL_UNSIGNED_BYTE; bpp=3; ok=true; break;
      case 16: res->_internalFormat = GL_RGB16; res->_type = GL_UNSIGNED_INT; bpp=6; ok=true; break;
      }
      break;

    case PNG_COLOR_TYPE_RGB_ALPHA:
      //STDDBG;
      res->_format  = GL_RGBA;
      switch(depth) {
      case 8: res->_internalFormat = GL_RGBA; res->_type = GL_UNSIGNED_BYTE; bpp=4; ok=true; break;
      case 16: res->_internalFormat = GL_RGBA16; res->_type = GL_UNSIGNED_INT; bpp=8; ok=true; break;
      }
      break;

    case PNG_COLOR_TYPE_GRAY:
      //STDDBG;
      res->_format = GL_LUMINANCE;
      switch(depth) {
      case 1:
      case 2:
      case 4:
	break;
      case 8: res->_internalFormat = GL_LUMINANCE; res->_type = GL_UNSIGNED_BYTE; bpp=1; ok=true; break;
      case 16: res->_internalFormat = GL_LUMINANCE16; res->_type = GL_UNSIGNED_INT; bpp=2; ok=true; break;
      }
      break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
      //STDDBG;
      res->_format = GL_LUMINANCE_ALPHA;
      switch(depth) {
      case 8: res->_internalFormat = GL_LUMINANCE_ALPHA; res->_type = GL_UNSIGNED_BYTE; bpp=1; ok=true; break;
      case 16: res->_internalFormat = GL_LUMINANCE16_ALPHA16; res->_type = GL_UNSIGNED_INT; bpp=2; ok=true; break;
      }
      break;

    default:
      break;

    }

    //std::cerr << "PNG format type:" << color_type << "depth: " << depth << " "__FILE__":" << __LINE__ << std::endl;

    if(!ok) {
      std::cerr << "unsupported PNG format type:" << color_type << "depth: " << depth << " "__FILE__":" << __LINE__ << std::endl;
      png_destroy_read_struct(&png_ptr, &info_ptr,
			      &end_info);
      return 0;
    }


    const unsigned int texsize=texwidth*texheight*bpp;
    PixmapImage::value_type* pixmap = new PixmapImage::value_type[texsize];
    {
      memset(pixmap,255,texsize);
    }
#if 1
    {
      unsigned int i;
      for(i=0; i<height;++i) {
	png_read_row(png_ptr, &pixmap[i*texwidth*bpp], 0);
      }
    }
#endif
    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

    //    std::cerr << DBGVAL(width) << DBGVAL(texwidth) << DBGVAL(height) << DBGVAL(texheight) << " pixmap:" << (void*)pixmap << std::endl;

    res->_pixmap=pixmap;
    
    return res;
  }


  SVGImage::SVGImage()
    : _doc(0)
  {
  }

  SVGImage::SVGImage(svg::SVGDocument * doc)
    : _doc(doc)
  {
  }

  SVGImage::~SVGImage()
  {
  }

  SVGImage*
  SVGImage::load(const unicode::String* filename)
  {
    svgl::Parser parser;
    svg::SVGDocument *thesvgdoc=0;
    try {
      const char* f_c_str = filename->transcode();
      thesvgdoc = parser.parse(f_c_str);
    }
    catch (const svgl::Parser::exception& e) {
      std::cerr << e.what() << __FL__;
      return 0;
    }
    catch (std::exception& e) {
      std::cerr << e.what() << __FL__;
      return 0;
    }
    catch (...) {
      std::cerr << "unknown exception" << __FL__;
      return 0;
    }

    if(!thesvgdoc)
      return 0;


#if 0
    {
      std::cerr << "writing document..." << __FL__;
      dom::DOMWriter writer(std::cerr);
      //std::cout << std::flush;
      writer.visitDocument(thesvgdoc);
      std::cerr << std::endl << "document written." << __FL__;
      
    }
#endif

    thesvgdoc->updateStyle();

#if 0
    animinfo->animationManager->stop();
    animinfo->animationManager->unsubscribe_all();
    
    //gc_dbg_save.push_back(thesvgelt);
    
    svg::SVGSVGElement * thesvgelt = thesvgdoc->getRootElement().getValue();
    if(!thesvgelt) {
      std::cerr << "no svg element in " << filename << __FL__;
      return 0;
    }
    thesvgelt->animationTraverse(animinfo);
    
    //std::cerr << DBGVAR(thesvgelt) << __FL__;
    
    const svg::SVGLength& widthl = thesvgelt->GETVAL(Width);
    const svg::SVGLength& heightl = thesvgelt->GETVAL(Height);
    
    //std::cerr << DBGVAR(&widthl) << DBGVAR(&heightl) << __FL__
#if 0
    float width = svglContext.computeWidthLength(widthl);
    float height = svglContext.computeHeightLength(heightl);
#else
    float width = widthl.getValue().getValue();
    float height = heightl.getValue().getValue();
#endif
    
    //std::cerr << DBGVAR(width) << DBGVAR(height) << __FL__
    //std::cerr << DBGVAR(widthl.getValueAsString()) << DBGVAR(heightl.getValueAsString()) << __FL__
    
    svglContext->setViewportWidthHeight(width, height);
    svglContext->externalEntityManager->setDocumentName(unicode::String::createString(filename));
    svglContext->svgOwner = thesvgelt;
    
#endif

    SVGImage * im = new SVGImage(thesvgdoc);
    return im;
  }

  void
  SVGImage::glDisplay(double x, double y, double width, double height)
  {
  } 
}

