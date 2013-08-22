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
#ifndef __svgl_FontManager__
#define __svgl_FontManager__

//#define ___SVGL

#ifdef ___SVGL
#include <svgl/config.hpp>
#include <dom/string.hpp>
#endif

#include <glft/glft.hpp>
#include <map>
#include <vector>
#include <string>
#include <glft/FontInfo.hpp>
#include <GL/gl.h>

namespace glft {

  struct FontFileInfo {
    FontInfo fontInfo;
    std::string path;
    std::string fileName;
  };

  class FontManager {
  public:
    typedef int size_type;
    typedef std::string system_string;
    FontManager(const system_string& font_path, const system_string& cache_path, bool yUp=false, bool autoEnableTexture=false);
    ~FontManager();

    enum font_type {
      vector,
      texture
    };

#ifndef ___SVGL
    typedef const std::string string;
    typedef std::string::value_type char_type;
#else
    typedef const unicode::String string;
    typedef const unicode::String::char_type char_type;
#endif

    // --------------------------------

    void setFontType(font_type);
    font_type getFontType() { return _font_type ; }
    void setCurrentFont(const FontInfo&, size_type size);

    // --------
    // string*

    void render(string* msg);
    void renderAndAdvance(string* msg);
    void renderAutoScale(string* msg);
    void renderAndAdvanceAutoScale(string* msg);
    void renderWithScale(string* msg, double scale);
    void renderAndAdvanceWithScale(string* msg, double scale);

    void getBoundingBox(string* msg, int* x, int* y, int* width, int* height);
    void getBoundingBoxWithScale(string* msg, double scale, int* x, int* y, int* width, int* height);
    void getBoundingBoxAutoScale(string* msg, int* x, int* y, int* width, int* height);

    float getXAdvance(string* msg);

    // --------
    // const char*

    void render(const char* msg);
    void renderAndAdvance(const char* msg);
    void renderAutoScale(const char* msg);
    void renderAndAdvanceAutoScale(const char* msg);
    void renderWithScale(const char* msg, double scale);
    void renderAndAdvanceWithScale(const char* msg, double scale);

    void getBoundingBox(const char* msg, int* x, int* y, int* width, int* height);
    void getBoundingBoxWithScale(const char* msg, double scale, int* x, int* y, int* width, int* height);
    void getBoundingBoxAutoScale(const char* msg, int* x, int* y, int* width, int* height);

    float getXAdvance(const char* msg);

    // --------
    // char_type

    void render(const char_type& domchar);
  	void renderAndAdvance(const char_type& domchar);
    void renderAutoScale(const char_type& domchar);
		void renderAndAdvanceAutoScale(const char_type& domchar);
	  void renderWithScale(const char_type& domchar, double scale);
    void renderAndAdvanceWithScale(const char_type& domchar, double scale);

    void getBoundingBox(const char_type&, int* x, int* y, int* width, int* height);
    float getXAdvance(const char_type& c);

    void getFontTypeWithScale(double s, font_type& chosenType, float& exactApparentFontSize);
    void getFontTypeAutoScale(font_type& chosenType, float& exactApparentFontSize);

    // --------------------------------

    void updateFontList();
    //void setAutoScale(bool onoff);
    void setMaxTexturedFontSize(size_type s) { _maxTexturedFontSize = s; }
    size_type getMaxTexturedFontSize() const { return _maxTexturedFontSize; }

    static FontManager *getDefaultManager(void) ;

  private:

    static FontManager *_defaultManager ;

    //void setCurrentTextureFont(string* font_name, size_type size);
    //void setCurrentVectorFont(string* font_name, size_type size);

		
		
    void setCurrentTextureFont(const FontFileInfo& f, size_type size);
    void setCurrentVectorFont(const FontFileInfo& f, size_type size);

    void renderTexture(string* msg);
    void renderTexture(const char_type& domchar);
    void renderVector(string* msg);
    void renderVector(const char_type& domchar);
    void getTextureBoundingBox(string* msg, int* x, int* y, int* width, int* height);
    void getVectorBoundingBox(string* msg, int* x, int* y, int* width, int* height);
    void getTextureBoundingBox(const char_type& msg, int* x, int* y, int* width, int* height);
    void getVectorBoundingBox(const char_type& msg, int* x, int* y, int* width, int* height);
    float getTextureXAdvance(string* msg);
    float getVectorXAdvance(string* msg);
    float getTextureXAdvance(const char_type& c);
    float getVectorXAdvance(const char_type& c);

// Bounding Box
//
// _____________________P2
// | |                  |
// | |                  |
// | |O                 |
// |-|------------------|
// |____________________|
// P1

// O = (0,0)
// P1 = (x,y)
// P2 = (w-x,h-y)





    font_type _font_type;
    size_type _current_size;
        
  private:
    system_string _font_path;
    system_string _cache_path;

    struct PixmapFaceInfo {
      glft_pixmap_font* _glft_face;
      PixmapFaceInfo(glft_pixmap_font* f=0) : _glft_face(f) {}
      GLuint _texnum;
    };

    struct PixmapFontInfo {
      typedef std::map<size_type, PixmapFaceInfo> PixmapFaceInfos;
      PixmapFaceInfos _pixmapFaceInfos;
    };

    typedef std::map<system_string, PixmapFontInfo> PixmapFontInfos;
    PixmapFontInfos _pixmapFontInfos;

    PixmapFaceInfo * _currentPixmapFaceInfo;

    struct VectorFaceInfo {
      glft_vector_font* _glft_face;
      VectorFaceInfo(glft_vector_font* f=0) : _glft_face(f) {}
    };

    struct VectorFontInfo {
      typedef std::map<size_type, VectorFaceInfo> VectorFaceInfos;
      VectorFaceInfos _vectorFaceInfos;
    };

    typedef std::map<system_string, VectorFontInfo> VectorFontInfos;
    VectorFontInfos _vectorFontInfos;

    VectorFaceInfo * _currentVectorFaceInfo;

    void glCreateTextureFont( PixmapFaceInfo& );
    void glSetCurrentTextureFont( PixmapFaceInfo& );
    void glSetCurrentVectorFont( VectorFaceInfo& );

    typedef std::vector<const std::string*> DirectoriesPath;
    void updateCacheDirectories(DirectoriesPath& path);

    typedef std::vector<FontFileInfo> FontFileInfos;
    FontFileInfos _fontFileInfos;
    const FontFileInfo* _defaultFileInfo;
    FontFileInfo _current_fontinfo;

    struct NoFont {
      const char* what() const throw() { return "No Font"; }
    };
    // f is set with fontFamily, weight and style
    // result has the right path
    const FontFileInfo&
    getFontFileInfo(const FontFileInfo& f) throw(NoFont);
		void setupFontsForScale(double scale, PixmapFaceInfo *&, float& exactApparentFontSize);

    bool _autoScale;
    size_type _maxTexturedFontSize;
    font_type _user_font_type;
    size_type _user_size;
    //    void setFontAccordingToScale(double scale);
    bool _yUp, _autoEnableTexture;

  };

}

#endif // __svgl_FontManager__
