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

#include <glft/FontManager.hpp>

#ifndef ___SVGL
#include <glft/config.hpp>
#endif

#include <glft/glft.hpp>
#include <glft/debug.hpp>
#include <string>

#if OS_IS_MACOSX
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFBundle.h>
#endif

#ifndef ___SVGL
#define transcode c_str
#else
#define AUTO_ENABLE_TEXTURE
#define Y_UP
#endif

// stat(2)
#include <sys/stat.h>
#include <stdio.h>

// rint
#include <math.h>

#if defined(WIN32) && ! defined(__CYGWIN__)
#include <windows.h>
#include <direct.h>
#include <sys/stat.h>
#include <io.h>
#define DIRSEP "\\"
#else
#include <dirent.h>
#include <unistd.h>
#define DIRSEP "/"
#endif

#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H

//#include <glft/debug.hpp>

#if defined(WIN32) && ! defined(__CYGWIN__)
typedef struct {
  int handle;
  struct _finddata_t f_data;
} DIR;

struct dirent {
  char d_name[260];
};

DIR * opendir(const char * filename)
{
  static char slash_star [3] = { '/', '*', 0 };

  char dirname[2048];
  DIR * dir = new DIR;
  strcpy(dirname, filename);
  strcat(dirname, slash_star);
  dir->handle = _findfirst(dirname, &dir->f_data);
  if (dir->handle == -1) {
    delete dir;
    return 0;
  }
  return dir;
}

struct dirent *
readdir(DIR * dir)
{
  int code = _findnext(dir->handle, &dir->f_data);
  if (code == -1) {
    delete dir;
    return 0;
  }
  return (struct dirent*)(&dir->f_data.name);
}

#endif

namespace glft {

  void
  lower(std::string& s)
  {
    for(std::string::size_type i=0; i<s.size(); ++i) {
      s[i] = tolower(s[i]);
    }

  }

  const char* FontInfo::styleDesc[]={"regular", "normal", "italic", "oblique", "unknown"};
  const char* FontInfo::weightDesc[] = {"normal", "bold", "unknown"};

  FontInfo::Style
  FontInfo::getStyle(const std::string& sname)
  {
		std::cerr << sname << __FLendl__;
    FontInfo::Style res = FontInfo::fs_normal;
    std::string style_name = sname;
    lower(style_name);

    for (int n = 0; n < FontInfo::fs_unknown; ++n) {
	if(style_name.find(FontInfo::styleDesc [n])!=std::string::npos)
	    res=(FontInfo::Style)n;
    }
		std::cerr << res << __FLendl__;
    return res;
  }

  FontInfo::Weight
  FontInfo::getWeight(const std::string& sname)
  {
		std::cerr << sname << __FLendl__;
    FontInfo::Weight res = FontInfo::fw_normal;
    std::string style_name = sname;
    lower(style_name);

    for (int n = 0; n < FontInfo::fw_unknown; ++n) {
	if(style_name.find(FontInfo::weightDesc [n])!=std::string::npos)
	    res=(FontInfo::Weight)n;
    }
		std::cerr << res << __FLendl__;
    return res;
  }


  void
  setFontFileInfo(FontFileInfo& f, FT_Face ftface, const std::string& path_)
  {
    f.path = path_;

    std::string::size_type pos = path_.rfind('/');
    if(pos!=std::string::npos) {
      f.fileName = path_.substr(pos+1);
    }
    else {
      f.fileName = path_;
    }


    f.fontInfo.family = ftface->family_name;
    lower(f.fontInfo.family);

    std::string style_name = ftface->style_name;
    lower(style_name);

    for (int n = FontInfo::fs_regular; n <= FontInfo::fs_oblique; ++n) {
	if(style_name.find(FontInfo::styleDesc [n])!=std::string::npos)
	    f.fontInfo.style=(FontInfo::Style)n;
    }

    f.fontInfo.weight=FontInfo::fw_normal;
    if(style_name.find("bold")!=std::string::npos) {
      f.fontInfo.weight=FontInfo::fw_bold;
    }

    //std::cerr << f.fontInfo.family << " " << FontInfo::styleDesc[f.fontInfo.style] << " " << FontInfo::weightDesc[f.fontInfo.weight] << __FLendl__;

  }

  FontManager::FontManager(const system_string& font_path, const system_string& cache_path, bool yUp, bool autoEnableTexture)
    : _font_type(vector), _current_size(12), _font_path(font_path.c_str()), _cache_path(cache_path.c_str()), _currentPixmapFaceInfo(0), _currentVectorFaceInfo(0),
      _autoScale(false), _maxTexturedFontSize(30), _user_font_type(_font_type), _user_size(_current_size), _yUp(yUp), _autoEnableTexture(autoEnableTexture)
  {
    glft_init();
    struct stat buf;

    // create cache path
    if(::stat(cache_path.c_str(), &buf)) {
#if defined(WIN32) && ! defined(__CYGWIN__)
	 int fd=::mkdir(cache_path.c_str());
#else
	 int fd=::mkdir(cache_path.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
#endif

	 if(fd==-1) {
	   std::cerr << "couldn't create cache path \"" << cache_path << "\"" __FLendl__;
	   throw 1;
	 }
    }

    updateFontList();

  }

  void
  FontManager::updateFontList()
  {
    // synonyms
    typedef std::vector<std::string> synonym_t;
    typedef std::vector<synonym_t> Synonyms;

    synonym_t syn1;
    syn1.push_back("helvetica");
    syn1.push_back("arial");
    syn1.push_back("sans-serif");
    synonym_t syn2;
    syn2.push_back("times");
    syn2.push_back("times new roman");
    syn2.push_back("serif");
    synonym_t syn3;
    syn3.push_back("courrier");
    syn3.push_back("courrier new");
    syn3.push_back("typeface");

    Synonyms synonyms;
    synonyms.push_back(syn1);
    synonyms.push_back(syn2);
    synonyms.push_back(syn3);

    _fontFileInfos.clear();
    struct stat buf;
    
    // list font file
    if(!::stat(_font_path.c_str(), &buf) && (buf.st_mode & S_IFDIR) != 0) {
      DIR * fontdir = ::opendir(_font_path.c_str());
      if(fontdir==0) {
	   std::cerr << "couldn't open font directory \"" << _font_path << "\"" << __FLendl__;
      }
      else {
	   const unsigned int bufsize=2048;
	   char currentDir[bufsize];
	   ::getcwd(currentDir, bufsize);
	   ::chdir(_font_path.c_str());

	   FT_Library ftlib;
	   FT_Init_FreeType(&ftlib);
	   FT_Face ftface;

	   std::string fontdir_std = _font_path+'/';

	   for(dirent *dir=::readdir(fontdir); dir!=0; dir=::readdir(fontdir)) {
		int res = ::stat(dir->d_name, &buf);
		if(res==-1) {
		  perror(dir->d_name);
		} else {
		  if((buf.st_mode & S_IFREG)!= 0) {
		    //std::cerr << dir->d_name << __FLendl__;
		    int res = FT_New_Face(ftlib, dir->d_name, 0, &ftface);
		    if(!res) {		
					//std::cerr << ftface->family_name << " , " << ftface->style_name << __FLendl__;
			 FontFileInfo f;
			 setFontFileInfo(f, ftface, fontdir_std+dir->d_name);
			 FT_Done_Face(ftface);
			 bool has_synonym=false;
			 for (Synonyms::const_iterator i = synonyms.begin();
				 i != synonyms.end();
				 ++i) {		  

			   synonym_t::const_iterator j = i->begin();
			   for (;
				   j != i->end();
				   ++j) {
				if ((*j) == f.fontInfo.family) {
				  has_synonym=true;
				  break;
				}
			   }
			   if(has_synonym) {
				for (j = i->begin();
					j != i->end();
					++j) {
				  FontFileInfo g(f);
				  g.fontInfo.family = *j;
				  _fontFileInfos.push_back(g);
				}
				break;
			   }
			 }
			 if(!has_synonym) {
			   _fontFileInfos.push_back(f);
			 }
		    }
		    else {
			 //std::cerr << dir->d_name << " is not a font" << __FLendl__;
		    }
		  }
		}
	   }

	   FT_Done_FreeType(ftlib);
	   ::chdir(currentDir);
      }
    }
    else {
      std::cerr << "font directory \"" << _font_path << "\" does not exist" << __FLendl__;
    }

    if(_fontFileInfos.empty()) {
      _defaultFileInfo=0;
			std::cerr << "no fonts available" << __FLendl__;
		}
    else {
      try {
				FontFileInfo f;
				f.fontInfo.family="sans-serif";
				_defaultFileInfo=&(getFontFileInfo(f));
      }
      catch(NoFont&) {
				_defaultFileInfo= &(*(_fontFileInfos.begin()));
      }
#if 0
			std::cerr << "default font is "
				<< _defaultFileInfo->fontInfo.family << " "
				<< glft::FontInfo::styleDesc[_defaultFileInfo->fontInfo.style] << " "
				<< glft::FontInfo::weightDesc[_defaultFileInfo->fontInfo.weight] << __FLendl__;
#endif
    }
  }


  bool
  myequals(const FontFileInfo& f1, const FontFileInfo& f2)
  {
    bool styleeq = false;
    if(f1.fontInfo.style==f2.fontInfo.style) {
      styleeq=true;
    } else {
      if (f1.fontInfo.style==FontInfo::fs_normal && f2.fontInfo.style==FontInfo::fs_regular)
				styleeq=true;
      else
				if (f1.fontInfo.style==FontInfo::fs_regular && f2.fontInfo.style==FontInfo::fs_normal)
					styleeq=true;
    }
    return (f1.fontInfo.family==f2.fontInfo.family) && (f1.fontInfo.weight==f2.fontInfo.weight) && styleeq;
  }

  const FontFileInfo&
  FontManager::getFontFileInfo(const FontFileInfo& f) throw(FontManager::NoFont)
  {
    //std::cerr << "--------" << __FL__;
		//if(f.fontInfo.family=="sans-serif")
    //std::cerr << f.fontInfo.family << " " << f.fontInfo.style << " " << f.fontInfo.weight << __FLendl__;
    for(FontFileInfos::const_iterator i=_fontFileInfos.begin();
	   i!= _fontFileInfos.end();
	   ++i) {
			//if(i->fontInfo.family=="sans-serif")
      //std::cerr << i->fontInfo.family << " " << i->fontInfo.weight << " " << i->fontInfo.style << __FLendl__;
      if(myequals(*i,f)) {
				return (*i);
      }
    }
    if(_defaultFileInfo) {
			static int msg_given = 0;
			if (msg_given == 0) {
				msg_given = 1;
				
				std::cerr << "couldn't find font \"" << f.fontInfo.family << " "
				<< glft::FontInfo::styleDesc[f.fontInfo.style] << " "
				<< glft::FontInfo::weightDesc[f.fontInfo.weight] << " "
				<< "\". replacing with default." << __FLendl__;
			}
      return *_defaultFileInfo;
		}
		
		static int msg_given = 0;
		if (msg_given == 0) {
			msg_given = 1;
			if(_fontFileInfos.empty()) {
				std::cerr << "No fonts vailable: have you set the env variable GLFT_FONTDIR ?" << __FLendl__;
			}
		}
		throw NoFont();
  }

  FontManager::~FontManager()
  {
    glft_done();
  }

  void
  FontManager::updateCacheDirectories(DirectoriesPath& path)
  {
    system_string cachedir = _cache_path;

    for(DirectoriesPath::const_iterator i = path.begin();
	   i != path.end();
	   ++i) {
      cachedir+=DIRSEP+**i;
      //      std::cerr << **i << std::endl;
      //      std::cerr << cachedir << std::endl;
      struct stat buf;
      if(::stat(cachedir.c_str(), &buf)) {
#if defined(WIN32) && ! defined(__CYGWIN__)
	   int fd=::mkdir(cachedir.c_str());
#else
	   int fd=::mkdir(cachedir.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
#endif
	   if(fd==-1) {
		std::cerr << "cannot create " << cachedir << __FLendl__;
		return;
	   }
      }
      else {
	   if((buf.st_mode & S_IFDIR)==0) {
		STDDBG;
		return;
	   }
      }
    }    
  }

  void
  FontManager::glCreateTextureFont( PixmapFaceInfo& faceInfo)
  {
    glGenTextures(1, &(faceInfo._texnum));
    glBindTexture(GL_TEXTURE_2D, faceInfo._texnum);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8,
			  glft_pixmap_font_get_width(faceInfo._glft_face),
			  glft_pixmap_font_get_height(faceInfo._glft_face),
			  0, GL_ALPHA, GL_UNSIGNED_BYTE,
			  glft_pixmap_font_get_pixmap(faceInfo._glft_face));

  }

  void
  FontManager::glSetCurrentTextureFont( PixmapFaceInfo& faceInfo)
  {
    glBindTexture(GL_TEXTURE_2D, faceInfo._texnum);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1./glft_pixmap_font_get_width(faceInfo._glft_face), 1./glft_pixmap_font_get_height(faceInfo._glft_face),1);
    glMatrixMode(GL_MODELVIEW);

    _currentPixmapFaceInfo = &faceInfo;
  }

  void
  FontManager::setCurrentTextureFont(const FontFileInfo& fontFileInfoRequested, size_type size)
  {
    // check the existence of font file
    const FontFileInfo* fontFileInfo=0;
    try {
      fontFileInfo = &(getFontFileInfo(fontFileInfoRequested));
    }
    catch (NoFont&) {
      std::cerr << "no fonts available" << __FLendl__;
      return;
    }

    // check if it's in memory

    PixmapFontInfo& fontInfo = _pixmapFontInfos[fontFileInfo->fileName];
    PixmapFaceInfo& faceInfo = fontInfo._pixmapFaceInfos[size];

    if(faceInfo._glft_face) {
	 _currentPixmapFaceInfo = &faceInfo;
      // previous line was: glSetCurrentTextureFont(faceInfo);
      return;
    }

#if 0
    system_string fullpath = _font_path + DIRSEP + font_name->transcode();
    if(::stat(fullpath.c_str(), &buf)) {
      std::cerr << "couldn't find font " << fullpath << __FLendl__;
      fullpath = _font_path + DIRSEP + "Default";
      if(::stat(fullpath.c_str(), &buf)) {
	   std::cerr << "couldn't find font " << fullpath << __FLendl__;
	   fullpath = _font_path + DIRSEP + "Default";
	   return;
      }
    }
#endif



    // check if it's in the cache
    // .../cache_path/texture/fontname/size
    DirectoriesPath path;
    system_string texture_string = system_string("texture");
    path.push_back(&texture_string);
    //system_string font_name_sys = font_name->transcode();
    system_string font_name_sys = fontFileInfo->fileName;
    path.push_back(&font_name_sys);
    system_string sizestr;
    {
      // beerk FIXME
      char tmp[250];
      sprintf(tmp, "%d", size);
      //      cachedir = cachedir + "/" + tmp;
      sizestr=tmp;
    }
    path.push_back(&sizestr);

    updateCacheDirectories(path);
    
    system_string cachedir = _cache_path + DIRSEP + texture_string + DIRSEP + font_name_sys + DIRSEP + sizestr;

    system_string pixmapname = cachedir+DIRSEP + "pixmap.pgm";
    system_string metricsname = cachedir+DIRSEP + "metrics";

    struct stat buf;
    if(::stat(pixmapname.c_str(), &buf) || ::stat(metricsname.c_str(), &buf)) {
      // create it
      glft_font* font = glft_font_new(fontFileInfo->path.c_str());
      if(!font) {
	   STDDBG;
	   return;
      }
      faceInfo._glft_face = glft_new_pixmap_font(font, glft_printable, size);
      //faceInfo._glft_face = glft_new_pixmap_font(font, "to", size);
      if(!faceInfo._glft_face) {
	   STDDBG;
	   return;
      }
      // cache it
      {
	   FILE* f = fopen(pixmapname.c_str(), "wb");
	   if(!f) {
		perror(pixmapname.c_str());
		STDDBG;
		return;
	   }
	   glft_pixmap_font_save_pixmap_pgm(faceInfo._glft_face, f);
	   fclose(f) ;
      }
      {
	   FILE* f = fopen(metricsname.c_str(), "wb");
	   if(!f) {
		STDDBG;
		return;
	   }
	   glft_pixmap_font_save_metrics(faceInfo._glft_face, f);
	   fclose(f) ;
      }
      glft_font_delete(font);
    }
    else {
      // read it
      faceInfo._glft_face = glft_pixmap_font_new();
      {
	   FILE* f = fopen(pixmapname.c_str(), "rb");
	   glft_pixmap_font_load_pixmap_pgm(faceInfo._glft_face, f);
	   fclose(f) ;
      }
      {
	   FILE* f = fopen(metricsname.c_str(), "rb");
	   glft_pixmap_font_load_metrics(faceInfo._glft_face, f);
	   fclose(f) ;
      }
    }
    glCreateTextureFont(faceInfo);

    _currentPixmapFaceInfo = &faceInfo;
    // previous line was: glSetCurrentTextureFont(faceInfo);
  }


  void
  FontManager::glSetCurrentVectorFont( VectorFaceInfo& faceInfo)
  {
    _currentVectorFaceInfo = &faceInfo;
  }


  void
  FontManager::setCurrentVectorFont(const FontFileInfo& fontFileInfoRequested, size_type size)
  {
    // check the existence of font file
    const FontFileInfo* fontFileInfo=0;
    try {
      fontFileInfo = &(getFontFileInfo(fontFileInfoRequested));
		}
    catch (NoFont&) {
      std::cerr << "no fonts available" << __FLendl__;
      return;
    }


    // check if it's in memory

    VectorFontInfo& fontInfo = _vectorFontInfos[fontFileInfo->fileName];
    VectorFaceInfo& faceInfo = fontInfo._vectorFaceInfos[size];

    if(faceInfo._glft_face) {
      glSetCurrentVectorFont(faceInfo);
      return;
    }


    // check if it's in the cache
    // .../cache_path/vector/fontname/size
    DirectoriesPath path;
    system_string texture_string = system_string("vector");
    path.push_back(&texture_string);
    system_string font_name_sys = fontFileInfo->fileName;
    path.push_back(&font_name_sys);
    system_string sizestr;
    {
      // beerk FIXME
      char tmp[250];
      sprintf(tmp, "%d", size);
      //      cachedir = cachedir + DIRSEP + tmp;
      sizestr=tmp;
    }
    path.push_back(&sizestr);

    updateCacheDirectories(path);
    
    system_string cachedir = _cache_path + DIRSEP + texture_string + DIRSEP + font_name_sys + DIRSEP + sizestr;
    system_string outlinename = cachedir+DIRSEP + "outline";

    struct stat buf;
    if(::stat(outlinename.c_str(), &buf)) {
      // create it
      glft_font* font = glft_font_new(fontFileInfo->path.c_str());
      if(!font) {
	   STDDBG;
	   return;
      }
      faceInfo._glft_face = glft_new_vector_font(font, glft_printable, size);
      //faceInfo._glft_face = glft_new_vector_font(font, "to", size);
      if(!faceInfo._glft_face) {
	   STDDBG;
	   return;
      }
      // cache it
      {
	   FILE* f = fopen(outlinename.c_str(), "wb");
	   if(!f) {
		perror(outlinename.c_str());
		STDDBG;
		return;
	   }
	   glft_vector_font_save(faceInfo._glft_face, f);
	   fclose(f) ;
      }
      glft_font_delete(font);
    }
    else {
      // read it
      faceInfo._glft_face = glft_vector_font_new();
      {
	   FILE* f = fopen(outlinename.c_str(), "rb");
	   glft_vector_font_load(faceInfo._glft_face, f);
	   fclose(f) ;
      }
    }    
    glSetCurrentVectorFont(faceInfo);
  }

  void
  fineAlignment()
  {
    // fine alignment with pixels

    // first, round the MODEL_VIEW matrix
    // we don't cope with PROJECTION matrix, so it must be right (ie int values)

#if 1
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
#if 0
    std::cerr << "---" << std::endl ;
    for(int i=0; i<4; ++i) {
			for(int j=0; j<4; ++j)
				std::cerr << m[i+j*4] << "\t";
			std::cerr << "\n";
    }
    std::cerr << std::endl;
#endif
#if 0
    float dx = (rint(m[12])-m[12])/m[0]; //(m[0]==currentScale*scale ?
    float dy = (rint(m[13])-m[13])/m[0];
		glTranslatef(dx,dy,0);
#else
		m[12]=rint(m[12]);
		m[13]=rint(m[13]);
		glLoadMatrixf(m);
#endif
    
#if 0
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    for(int i=0; i<4; ++i) {
			for(int j=0; j<4; ++j)
				std::cerr << m[i+j*4] << "\t";
			std::cerr << "\n";
    }
    std::cerr << std::endl;
#endif
    // then apply a slight offset, to avoid chaoitic opengl choice of pixel

    //glTranslatef(.3,.3,0);
#endif
  }


  void
  FontManager::renderTexture(FontManager::string* msg)
  {
    if(!msg)
      return;
    if(!_currentPixmapFaceInfo) {
      std::cerr << "rendering a text, but there is no current texture...  " __FILE__":"<<__LINE__<<std::endl;
      return;
    }
    glMatrixMode(GL_MODELVIEW);

    if(_yUp)
      glScalef(1,-1,1);

    //std::cerr << msg.c_str() << std::endl;
    fineAlignment();

    glft_pixmap_font_render(_currentPixmapFaceInfo->_glft_face, msg->transcode());
  }

  void
  FontManager::renderTexture(const FontManager::char_type& domchar)
  {
    if(!_currentPixmapFaceInfo) {
      std::cerr << "rendering a char, but there is no current texture...  " __FILE__":"<<__LINE__<<std::endl;
      return;
    }
    glMatrixMode(GL_MODELVIEW);

    if(_yUp)
      glScalef(1,-1,1);

    //std::cerr << msg.c_str() << std::endl;

    char tmp[] = " ";
    tmp[0] = domchar;
    glft_pixmap_font_render(_currentPixmapFaceInfo->_glft_face, tmp);
  }


  void
  FontManager::renderVector(FontManager::string* msg)
  {
    if(!msg)
      return;
    if(!_currentVectorFaceInfo) {
      std::cerr << "rendering a text, but there is no current vector info...  " __FILE__":"<<__LINE__<<std::endl;
      return;
    }

    glMatrixMode(GL_MODELVIEW);

    if(_yUp)
      glScalef(1,-1,1);

    glft_set_gl_context();
    glft_vector_font_render_plain(_currentVectorFaceInfo->_glft_face, msg->transcode());
  }

  void FontManager::render(const char* msg) { string smsg = msg; render(&smsg); }
  void FontManager::renderAndAdvance(const char* msg) { string smsg = msg; renderAndAdvance(&smsg); }
  void FontManager::renderAutoScale(const char* msg) { string smsg = msg; renderAutoScale(&smsg); }
  void FontManager::renderAndAdvanceAutoScale(const char* msg) { string smsg = msg; renderAndAdvanceAutoScale(&smsg); }
  void FontManager::renderWithScale(const char* msg, double scale) { string smsg = msg; renderWithScale(&smsg, scale); }
  void FontManager::renderAndAdvanceWithScale(const char* msg, double scale) { string smsg = msg; renderAndAdvanceWithScale(&smsg, scale); }
  
  void FontManager::getBoundingBox(const char* msg, int* x, int* y, int* width, int* height) { string smsg = msg; getBoundingBox(&smsg, x,y,width,height); }
  void FontManager::getBoundingBoxWithScale(const char* msg, double scale, int* x, int* y, int* width, int* height) { string smsg = msg; getBoundingBoxWithScale(&smsg, scale, x,y,width,height); }
  void FontManager::getBoundingBoxAutoScale(const char* msg, int* x, int* y, int* width, int* height) { string smsg = msg; getBoundingBoxAutoScale(&smsg, x,y,width,height); }
  
  float getXAdvance(const char* msg);




  void
  FontManager::renderVector(const FontManager::char_type& domchar)
  {
    if(!_currentVectorFaceInfo) {
      std::cerr << "rendering a char, but there is no current vector info...  " __FILE__":"<<__LINE__<<std::endl;
      return;
    }
    glMatrixMode(GL_MODELVIEW);

    if(_yUp)
      glScalef(1,-1,1);

    //std::cerr << msg->transcode() << std::endl;

    char tmp[] = " ";
    tmp[0] = domchar;
    glft_vector_font_render_plain(_currentVectorFaceInfo->_glft_face, tmp);
  }


  void
  FontManager::getTextureBoundingBox(FontManager::string* msg, int* x, int* y, int* width, int* height)
  {
    if(!msg) {
      *x=*y=*width=*height=0;
      return;
    }
    if(!_currentPixmapFaceInfo) {
      std::cerr << "bbox of a text, but there is no current texture info...  " __FILE__":"<<__LINE__<<std::endl;
      return;
    }

    int x2,y2;
    glft_pixmap_font_bounding_box(_currentPixmapFaceInfo->_glft_face, msg->transcode(), x, y, &x2, &y2);
    *width = x2-*x;
    *height = y2-*y;
  }

  void
  FontManager::getVectorBoundingBox(FontManager::string* msg, int* x, int* y, int* width, int* height)
  {
    if(!msg) {
      *x=*y=*width=*height=0;
      return;
    }
    if(!_currentVectorFaceInfo) {
      std::cerr << "bbox of a text, but there is no current vector info...  " __FILE__":"<<__LINE__<<std::endl;
      return;
    }

    int x2,y2;
    glft_vector_font_bounding_box(_currentVectorFaceInfo->_glft_face, msg->transcode(), x, y, &x2, &y2);
    *width = x2-*x;
    *height = y2-*y;
    //*y=-(*height+*y);
  }

  void
  FontManager::getTextureBoundingBox(const FontManager::char_type& domchar, int* x, int* y, int* width, int* height)
  {
    if(!_currentPixmapFaceInfo) {
      std::cerr << "bbox of a text, but there is no current texture info...  " __FILE__":"<<__LINE__<<std::endl;
      return;
    }

    char msg[] = " ";
    msg[0]=domchar;
    int x2,y2;
    glft_pixmap_font_bounding_box(_currentPixmapFaceInfo->_glft_face, msg, x, y, &x2, &y2);
    *width = x2-*x;
    *height = y2-*y;
  }

  void
  FontManager::getVectorBoundingBox(const FontManager::char_type& domchar, int* x, int* y, int* width, int* height)
  {
    if(!_currentVectorFaceInfo) {
      std::cerr << "bbox of a text, but there is no current vector info...  " __FILE__":"<<__LINE__<<std::endl;
      return;
    }

    char msg[] = " ";
    msg[0]=domchar;
    int x2,y2;
    glft_vector_font_bounding_box(_currentVectorFaceInfo->_glft_face, msg, x, y, &x2, &y2);
    *width = x2-*x;
    *height = y2-*y;
    //*y=-(*height+*y);
  }


  void
  FontManager::setFontType(font_type t)
  {
    if(t==_font_type)
      return;

    _font_type=t;
    
    switch(t) {
    case vector:
      // if font was set, set it with the new type
      if(_currentPixmapFaceInfo) {
	   setCurrentVectorFont(_current_fontinfo, _current_size);
      }
      break;
    case texture:
      if(_currentVectorFaceInfo) {
	   setCurrentTextureFont(_current_fontinfo, _current_size);
      }
      break;
    }
    
  }

  void
  FontManager::setCurrentFont(const FontInfo& fontInfo, size_type size)
  {
    FontFileInfo fontFileInfo;
    fontFileInfo.fontInfo = fontInfo;
    
		try {
			_current_fontinfo = getFontFileInfo(fontFileInfo);;
			_current_size = size;
			_user_size = size;
			switch(_font_type) {
				case vector:
					setCurrentVectorFont(_current_fontinfo, _current_size);
					break;
				case texture:
					setCurrentTextureFont(_current_fontinfo, _current_size);
					break;
			}
		}
		catch (const NoFont&) {
		}
  }

  void
  FontManager::renderAndAdvance(FontManager::string* msg)
  {
    switch(_font_type) {
    case vector:
      renderVector(msg);
      break;
    case texture:
      if(_autoEnableTexture)
				glEnable(GL_TEXTURE_2D);
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glMatrixMode(GL_MODELVIEW);
      glSetCurrentTextureFont(*_currentPixmapFaceInfo);
      renderTexture(msg);
      glMatrixMode(GL_TEXTURE);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      if(_autoEnableTexture)
				glDisable(GL_TEXTURE_2D);

      break;
    }
  }


  double
  getCurrentScale()
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
  FontManager::getFontTypeWithScale(double s, FontManager::font_type& chosenType, float& exactApparentFontSize)
  {
    //int apparentFontSize = _user_size;
    // double scale = 1;
    exactApparentFontSize  = _user_size * s;

    int apparentFontSize = (int) rint(exactApparentFontSize);
    chosenType=texture;
    if(apparentFontSize > _maxTexturedFontSize) {
      chosenType=vector;
    }
    //return chosenType;
  }

  void
  FontManager::getFontTypeAutoScale(FontManager::font_type& chosenType, float& exactApparentFontSize)
  {
    double currentScale=1.0;
    currentScale = getCurrentScale();
    getFontTypeWithScale(currentScale, chosenType, exactApparentFontSize);
  }


  void
  FontManager::renderAndAdvanceAutoScale(FontManager::string* msg)
  {
    double currentScale=1.0;
    currentScale = getCurrentScale();
    renderAndAdvanceWithScale(msg,currentScale);
  }




  void
		FontManager::setupFontsForScale(double currentScale, FontManager::PixmapFaceInfo *& metricsPixmapFaceInfo, float& exactApparentFontSize)
  {
			font_type chosenType;
			//float exactApparentFontSize;
			font_type shouldBeType=texture;
			double scale=1;

			getFontTypeWithScale(currentScale, chosenType, exactApparentFontSize);
			int apparentFontSize = (int) rint(exactApparentFontSize);
			
	    if(_user_size > _maxTexturedFontSize) {
				shouldBeType=vector;
			}
			
#if 0
    //int apparentFontSize = _user_size;
    double scale = 1;
    //float exactApparentFontSize  = _user_size * currentScale;
    int apparentFontSize = (int) rint(exactApparentFontSize);

#if 0
    if(apparentFontSize<1) {
      renderAndAdvance(msg);
      return;
    }
#endif

    //std::cerr << DBGVAL(apparentFontSize) << __FLendl__;

    //glColor3f(1,1,1);
    //if(msg==FontManager::string::createString("Scalable Vector Graphics (SVG) Conformance Suite"))
    //std::cerr << msg << std::endl;
    //std::cerr << "local:" << apparentFontSize << " user:" << _current_size << std::endl;

    font_type chosenType=texture;
    font_type shouldBeType=texture;
    if(apparentFontSize > _maxTexturedFontSize) {
      chosenType=vector;
    }
    if(_user_size > _maxTexturedFontSize) {
      shouldBeType=vector;
    }
#endif
		//PixmapFaceInfo * metricsPixmapFaceInfo = 0;
    //VectorFaceInfo * _metricsVectorFaceInfo = 0;

#if 1
    // get the metrics from the chosen font
    if(chosenType==texture) {
      setFontType(texture);
      if(shouldBeType==texture) {
				// pick the metrics as they should be displayed
				size_type save_user_size = _user_size;
				setCurrentFont(_current_fontinfo.fontInfo, _user_size);
				_user_size = save_user_size;
      } else { // shouldBeType==vector
							 // take the metrics from the bigger texture font
				size_type save_user_size = _user_size;
				setCurrentFont(_current_fontinfo.fontInfo, _maxTexturedFontSize);
				_user_size = save_user_size;
      }
      metricsPixmapFaceInfo = _currentPixmapFaceInfo;
    }
    else {
      // FIXME
    }

#endif

    //float ddscale=1;
    if(chosenType==vector) {
      //STDDBG;
      glDisable(GL_TEXTURE_2D);
      setFontType(vector);
      // FIXME
      size_type save_user_size = _user_size;
      setCurrentFont(_current_fontinfo.fontInfo, _maxTexturedFontSize);
      _user_size = save_user_size;
      scale = (exactApparentFontSize/(double)_maxTexturedFontSize)/currentScale;
      //glColor3f(1,0,0);
    }
    else {
      //STDDBG;
      //glColor3f(0,1,0);
      glEnable(GL_TEXTURE_2D);
      setFontType(texture);
#if 0
      // I tried by interploating between xadvance, but it did not give good results...
      int nextapparentFontSize=apparentFontSize;
      ddscale = exactApparentFontSize-apparentFontSize;
      if(apparentFontSize>exactApparentFontSize) {
				//glColor3f(0,0,1);
				--nextapparentFontSize;
				ddscale = -ddscale;
				if(!nextapparentFontSize) {
					nextapparentFontSize=apparentFontSize;
					ddscale = 0;
				}
      }
      else if(apparentFontSize<exactApparentFontSize) {
				++nextapparentFontSize;
      }
			
      if(nextapparentFontSize) {
				size_type save_user_size = _user_size;
				setCurrentFont(_current_fontinfo, nextapparentFontSize);
				_user_size = save_user_size;
      }
			
      _save_currentPixmapFaceInfo = _currentPixmapFaceInfo;
#endif

      size_type save_user_size = _user_size;
      setCurrentFont(_current_fontinfo.fontInfo, apparentFontSize);
      _user_size = save_user_size;
      scale = 1./currentScale;
    }

    //std::cerr << apparentFontSize << " " << scale << " " << currentScale << std::endl;

    //int x,y,w,h;
    glPushMatrix();
    glScaled(scale, scale, 1);

    if(chosenType==texture) {
			//STDDBG;
      fineAlignment();
		}

	}
	
	void
	FontManager::renderAndAdvanceWithScale(FontManager::string* msg, double currentScale)
	{
		PixmapFaceInfo * metricsPixmapFaceInfo = 0;
		float exactApparentFontSize;
		setupFontsForScale(currentScale, metricsPixmapFaceInfo, exactApparentFontSize);
		
#if 1
    switch(_font_type) {
    case vector:
      renderVector(msg);
      break;
    case texture:

      if(_yUp)
				glScalef(1,-1,1);
			
      if(_autoEnableTexture)
				glEnable(GL_TEXTURE_2D);
				
			glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glMatrixMode(GL_MODELVIEW);
      glSetCurrentTextureFont(*_currentPixmapFaceInfo);
			
#if 1
      if(metricsPixmapFaceInfo) {
				//double s = ddscale;
				//double s = 1./scale;
				double s = exactApparentFontSize/_user_size;
				//std::cerr << "scale for advance: " << s << std::endl;
				//std::cerr << toto << std::endl;
				glft_pixmap_font_render_with_other_advance(_currentPixmapFaceInfo->_glft_face, metricsPixmapFaceInfo->_glft_face, s, msg->transcode());
				/*
      } else if(_save_currentVectorFaceInfo) {
				glft_pixmap_font_render_with_other_advance(_currentPixmapFaceInfo->_glft_face, _save_currentVectorFaceInfo->_glft_face, msg->transcode());
				*/
      } else
				
#endif
			{
				glft_pixmap_font_render(_currentPixmapFaceInfo->_glft_face, msg->transcode());
      }
			glMatrixMode(GL_TEXTURE);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      if(_autoEnableTexture)
				glDisable(GL_TEXTURE_2D);
    }
		
#else
    render(msg);
#endif
		
    glPopMatrix();
  }

  void
  FontManager::render(FontManager::string* msg)
  {
    glPushMatrix();
    renderAndAdvance(msg);
    glPopMatrix();
  }
  
  void
  FontManager::renderAutoScale(FontManager::string* msg)
  {
    glPushMatrix();
    renderAndAdvanceAutoScale(msg);
    glPopMatrix();
  }

  void
  FontManager::renderWithScale(FontManager::string* msg, double scale)
  {
    glPushMatrix();
    renderAndAdvanceWithScale(msg, scale);
    glPopMatrix();
  }

  void
  FontManager::renderAndAdvance(const FontManager::char_type& domchar)
  {
    switch(_font_type) {
    case vector:
      renderVector(domchar);
      break;
    case texture:
      if(_autoEnableTexture)
				glEnable(GL_TEXTURE_2D);
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glMatrixMode(GL_MODELVIEW);
      glSetCurrentTextureFont(*_currentPixmapFaceInfo);
      renderTexture(domchar);
      glMatrixMode(GL_TEXTURE);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      if(_autoEnableTexture)
				glDisable(GL_TEXTURE_2D);
      break;
    }   
  }

	void
	FontManager::renderAutoScale(const char_type& domchar)
	{
		renderWithScale(domchar, getCurrentScale());
	}
	
	void 
	FontManager::renderWithScale(const char_type& domchar, double scale)
	{
		glPushMatrix();
		renderAndAdvanceWithScale(domchar, scale);
		glPopMatrix();		
	}
	
	void
	FontManager::render(const char_type& domchar)
	{
		glPushMatrix();
		renderAndAdvance(domchar);
		glPopMatrix();
	}
	
  
  void
	FontManager::renderAndAdvanceAutoScale(const char_type& domchar)
	{
		renderAndAdvanceWithScale(domchar, getCurrentScale());		
	}
	
	
	void
	FontManager::renderAndAdvanceWithScale(const char_type& domchar, double scale)
	{
		PixmapFaceInfo * metricsPixmapFaceInfo = 0;
		float exactApparentFontSize;
		setupFontsForScale(scale, metricsPixmapFaceInfo, exactApparentFontSize);

		char tmp[2];
		tmp[1]=0;
		tmp[0]=domchar;
		
#if 1
    switch(_font_type) {
			case vector:
				renderVector(domchar);
				break;
			case texture:
				
				if(_yUp)
					glScalef(1,-1,1);
				
				if(_autoEnableTexture)
					glEnable(GL_TEXTURE_2D);
					
					glMatrixMode(GL_TEXTURE);
				glPushMatrix();
				glMatrixMode(GL_MODELVIEW);
				glSetCurrentTextureFont(*_currentPixmapFaceInfo);
				
#if 1
				if(metricsPixmapFaceInfo) {
					//double s = ddscale;
					//double s = 1./scale;
					double s = exactApparentFontSize/_user_size;
					//std::cerr << "scale for advance: " << s << std::endl;
					//std::cerr << toto << std::endl;
					glft_pixmap_font_render_with_other_advance(_currentPixmapFaceInfo->_glft_face, metricsPixmapFaceInfo->_glft_face, s, tmp);
					/*
				} else if(_save_currentVectorFaceInfo) {
					glft_pixmap_font_render_with_other_advance(_currentPixmapFaceInfo->_glft_face, _save_currentVectorFaceInfo->_glft_face, msg->transcode());
					*/
				} else
					
#endif
				{
					glft_pixmap_font_render(_currentPixmapFaceInfo->_glft_face, tmp);
				}
					glMatrixMode(GL_TEXTURE);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				if(_autoEnableTexture)
					glDisable(GL_TEXTURE_2D);
    }
		
#else
    render(msg);
#endif
		
    glPopMatrix();

	}
		
	
  
  void
  FontManager::getBoundingBox(FontManager::string* msg, int* x, int* y, int* width, int* height)
  {
    setCurrentFont(_current_fontinfo.fontInfo, _user_size);
    switch(_font_type) {
    case vector:
      getVectorBoundingBox(msg, x,y,width,height);
      break;
    case texture:
      getTextureBoundingBox(msg, x,y,width,height);
      break;
    }   
  }

  void
  FontManager::getBoundingBoxAutoScale(FontManager::string* msg, int* x, int* y, int* width, int* height)
  {
    double scale = getCurrentScale();
    getBoundingBoxWithScale(msg, scale, x, y, width, height);
  }

  void
  FontManager::getBoundingBoxWithScale(FontManager::string* msg, double currentScale, int* x, int* y, int* width, int* height)
  {
    int x2,y2;
    int apparentFontSize = _user_size;
    double scale = 1;
    float exactApparentFontSize  = apparentFontSize * currentScale;

    apparentFontSize = (int) rint(exactApparentFontSize);

    if(apparentFontSize<1) {
      getBoundingBox(msg,x,y,width,height);
      return;
    }


    font_type chosenType=texture;
    font_type shouldBeType=texture;
    if(apparentFontSize > _maxTexturedFontSize) {
      chosenType=vector;
    }
    if(_user_size > _maxTexturedFontSize) {
      shouldBeType=vector;
    }

    if(chosenType==vector) {
      setFontType(vector);
 
      size_type save_user_size = _user_size;
      setCurrentFont(_current_fontinfo.fontInfo, _maxTexturedFontSize);
      _user_size = save_user_size;

      scale = (exactApparentFontSize/(double)_maxTexturedFontSize)/currentScale;
			//std::cerr << scale << std::endl;
      glft_vector_font_bounding_box(_currentVectorFaceInfo->_glft_face, msg->transcode(), x, y, &x2, &y2);
			
    }
    else {
      setFontType(texture);

      size_type save_user_size = _user_size;
      setCurrentFont(_current_fontinfo.fontInfo, _user_size);
      _user_size = save_user_size;

      scale = (exactApparentFontSize/_user_size)/currentScale;
      glft_pixmap_font_bounding_box(_currentPixmapFaceInfo->_glft_face, msg->transcode(), x, y, &x2, &y2);
    }

    *width = x2-*x;
    *height = y2-*y;

		//std::cerr  << *width << " " << *height << std::endl;
    *x = (int)rint(*x * scale);
    *y = (int)rint(*y * scale);
		
    *width = (int)rint(*width * scale);
    *height = (int)rint(*height * scale);
		//std::cerr  << *width << " " << *height << std::endl;

  }

  void
  FontManager::getBoundingBox(const FontManager::char_type& domchar, int* x, int* y, int* width, int* height)
  {
    switch(_font_type) {
    case vector:
      getVectorBoundingBox(domchar, x,y,width,height);
      break;
    case texture:
      getTextureBoundingBox(domchar, x,y,width,height);
      break;
    }   
  }

  float
  FontManager::getXAdvance(FontManager::string* msg)
  {
    switch(_font_type) {
    case vector:
      return getVectorXAdvance(msg);
      break;
    case texture:
      return getTextureXAdvance(msg);
      break;
    }   
    return 0;
  }


  float
  FontManager::getXAdvance(const FontManager::char_type& domchar)
  {
    switch(_font_type) {
    case vector:
      return getVectorXAdvance(domchar);
      break;
    case texture:
      return getTextureXAdvance(domchar);
      break;
    }
    return 0;
  }


  float
  FontManager::getVectorXAdvance(FontManager::string* msg)
  {
    return glft_vector_font_get_xadvance(_currentVectorFaceInfo->_glft_face, msg->transcode());
  }

  float
  FontManager::getTextureXAdvance(FontManager::string* msg)
  {
    return glft_pixmap_font_get_xadvance(_currentPixmapFaceInfo->_glft_face, msg->transcode());
  }


  float
  FontManager::getVectorXAdvance(const FontManager::char_type& domchar)
  {
    char tmp[] = " ";
    tmp[0] = domchar;
    return glft_vector_font_get_xadvance(_currentVectorFaceInfo->_glft_face, tmp);
  }

  float
  FontManager::getTextureXAdvance(const FontManager::char_type& domchar)
  {
    char tmp[] = " ";
    tmp[0] = domchar;
    return glft_pixmap_font_get_xadvance(_currentPixmapFaceInfo->_glft_face, tmp);
  }

  FontManager *FontManager::_defaultManager = 0 ;

  FontManager *
  FontManager::getDefaultManager(void) {
    if (!_defaultManager) {
	 char *fontdir = getenv("GLFT_FONTDIR") ;
	 if (!fontdir) {
#if OS_IS_MACOSX
	   CFBundleRef myBundle = CFBundleGetMainBundle() ;
	   CFURLRef rel = CFBundleCopyResourcesDirectoryURL(myBundle) ;
	   CFURLRef abs = CFURLCopyAbsoluteURL(rel) ;
	   CFStringRef s = CFURLCopyFileSystemPath(abs,kCFURLPOSIXPathStyle) ;

	   fontdir = new char [1025] ; 
	   CFStringGetCString(s, fontdir, 1024, kCFStringEncodingISOLatin1) ;
	   strcat(fontdir,"/fonts/TrueType/") ;
#else
#ifdef WIN32
	   fontdir = "c:/Windows/Fonts" ;
#else
#ifndef DEFAULT_FONTDIR
#define DEFAULT_FONTDIR "/usr/X11R6/lib/X11/fonts/TrueType"
#endif // DEFAULT_FONTDIR
	   fontdir = DEFAULT_FONTDIR ;
#endif
#endif
	 }
	 char *cachedir = getenv("GLFT_CACHEDIR") ;
	 if (!cachedir) cachedir = "/tmp/glft-cache" ;
	 _defaultManager = new FontManager(fontdir, cachedir) ;
    }
    return _defaultManager ;
  }

}
