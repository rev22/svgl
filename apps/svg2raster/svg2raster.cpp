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

#include <w3c/svg/strings.hpp>
#include <w3c/svg/SVGDocument.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <svgl/Parser.hpp>
#include <svgl/printHelper.hpp>
#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/AnimationInfo.hpp>
#include <svgl/DisplayManager.hpp>
#include <glft/FontManager.hpp>
#include <svgl/ImageManager.hpp>
#include <FakeTimeManager.hpp>
#include <svgl/AnimationManager.hpp>
#include <svgl/ExternalEntityManager.hpp>
#include <svgl/InitHelper.hpp>

#include <w3c/css/CSSStyleDeclaration.hpp>

#include <iostream>
#include <xmlostream.hpp>
#include <map>
#include <vector>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

#include <GL/osmesa.h>

// getlong_opt(3)
#include <unistd.h>
#include <getopt.h>

// stat(2)
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>


extern 
DOM_Node*
dommain(const char* filename);

extern
bool
dom_init();

namespace svg {
  extern
  void
  init_colors();
}

svg::SVGSVGElement* thesvgelt;

svgl::Context svglContext;
svgl::GLInfo glinfo;
svgl::DisplayManager displayManager;
svgl::AnimationInfo animinfo;
//svgl::PickManager pickManager;

int num_filenames;
char** filenames;

bool
loadSvgFile(const char* filename)
{
  svgl::parser parser;
  svg::SVGDocument *thesvgdoc;
  try {
    thesvgdoc = parser.parse(filename);
  }
  catch (const svgl::parser::exception& e) {
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
    return false;

  thesvgdoc->updateStyle();

#if 0
  {
    std::cerr << "writing document..." << __FL__;
    dom::DOMWriter writer(std::cerr);
    //std::cout << std::flush;
    writer.visitDocument(thesvgdoc);
    std::cerr << "document written." << __FL__;

  }
#endif

  animinfo.animationManager->stop();
  animinfo.animationManager->unsubscribe_all();
  thesvgelt = thesvgdoc->GET_SIMPLE_VAL(RootElement);

  //std::cerr << DBGVAR(thesvgelt) << __FL__;

  svg::SVGLength widthl = thesvgelt->GETVAL(Width);
  svg::SVGLength heightl = thesvgelt->GETVAL(Height);

#if 0
  float width = svglContext.computeWidthLength(widthl);
  float height = svglContext.computeHeightLength(heightl);
#else
  float width = widthl.GET_SIMPLE_VAL(Value);
  float height = heightl.GET_SIMPLE_VAL(Value);
#endif

  svglContext.setViewportWidthHeight(width, height);
  svglContext.externalEntityManager->setDocumentName(unicode::String::createString(filename));
  svglContext.svgOwner = thesvgelt;

  return true;
}

void
screenshot_ppm(FILE* f, unsigned char *pixels, int width, int height)
{
  fprintf(f, "P6 %d %d 255\n",width,height);
  const int wlen = width*4;
  for(int row=0; row<height; ++row) {
    for(int i=0; i<wlen; i+=4) {
      fwrite( &pixels[row*wlen+i], 3, 1, f);
    }
  }
}

void
reshape(int w, int h)
{
  glinfo.winWidth=w;
  glinfo.winHeight=h;

  displayManager.reshape(&svglContext, &glinfo, thesvgelt);

  int err = glGetError();
  if(err)
    std::cerr << "GL error: " << gluErrorString(err) << __FL__;
}

void
display()
{
  displayManager.display(&svglContext, &glinfo, thesvgelt);
  glFinish();

  int err = glGetError();
  if(err)
    std::cerr << "GL error: " << gluErrorString(err) << __FL__;
}


void
print_usage(const char* progname)
{
  std::cerr << "usage: " << progname << " [options] file1.svg file2.Svg ..." << std::endl;
  std::cerr << "options:" << std::endl;
  std::cerr << "-d, --directory dirname" << std::endl;
  std::cerr << "-t, --type (ppm)" << std::endl;
  std::cerr << "-q, --quiet" << std::endl;
}

int
main(int argc, char** argv)
{
  option long_options[] = 
  {
    {"directory", 1, 0, 0},
    {"type", 1, 0, 0},
    {"quiet", 0, 0, 0},
    {0,0,0,0}
  };

  std::string out_dir ="./";
  std::string extension = ".ppm";
  bool quiet=false;

  while (1) {
    int option_index;
    int c = getopt_long(argc,argv, "d:t:q", long_options, &option_index);
    if(c==-1)
      break;
    switch(c) {
    case 'd': {
      out_dir = optarg;
      int s=out_dir.size();
      if( s>0 && out_dir[s-1]!='/')
	out_dir+='/';
      struct stat buf;
      if(::stat(out_dir.c_str(), &buf)) {
	int fd=::mkdir(out_dir.c_str(), S_IRWXU);
	if(fd==-1) {
	  std::cerr << "unable to make directory " << out_dir << std::endl;
	  return 1;
	}
      }
      else {
	if(!S_ISDIR(buf.st_mode)) {
	  std::cerr << out_dir << " is not a directory" << std::endl;
	  return 1;
	}
      }
    }
    break;
    case 't':
      if(std::string(optarg)!="ppm")
	print_usage(argv[0]);
      break;
    case 'q':
      quiet=true;
      break;
    default:
      print_usage(argv[0]);
      return 1;
      break;
    }
  }

  num_filenames = argc-optind;
  if(num_filenames < 1) {
    print_usage(argv[0]);
    return 1;
  }
  filenames = &argv[optind];


  OSMesaContext mesactx = OSMesaCreateContext(OSMESA_RGBA, 0);
  //OSMesaContext mesactx = OSMesaCreateContextExt(OSMESA_RGBA, 24, 8, 0, 0);
  unsigned char initbuf[4];
  OSMesaMakeCurrent(mesactx, initbuf, GL_UNSIGNED_BYTE, 1, 1);
  OSMesaPixelStore( OSMESA_Y_UP, 0);

#if 1
  svg2raster::FakeTimeManager timeManager(.01);
  svgl::InitHelper initHelper(&timeManager);
  svglContext = *(initHelper.context);
  glinfo = *(initHelper.glinfo);
  animinfo = *(initHelper.animinfo);
  //pickManager = initHelper.pickManager;
#else
  css::CSSStyle::init();
  svg::init_strings();
  svg::init_colors();

  svglContext.fontSize=12;
  svglContext.fontXHeight=12;

#ifdef WIN32
  svglContext.fontManager = new svgl::FontManager("c:/Winnt/Fonts", "c:/Temp/svgl_font_cache");
#else
  svglContext.fontManager = new svgl::FontManager("/home/conversy/local/share/fonts", "/tmp/svgl_font_cache");
#endif

  svglContext.imageManager = new svgl::ImageManager;
  svglContext.externalEntityManager = new svgl::ExternalEntityManager;

  svgl::GLInfo glinfo2;
  glinfo = glinfo2;
  glinfo.glInit();
  //std::cerr << glGetString(GL_EXTENSIONS) << __FL__;

  displayManager.init(&svglContext, &glinfo);

  svg2raster::FakeTimeManager timeManager(.01);
  animinfo.animationManager = new svgl::Animation::Manager(&timeManager, .01);
#endif

  bool err;

  for (int i=0; i<num_filenames; ++i) {
    err = loadSvgFile(filenames[i]);
    if(!err)
      continue;
    int width = (int)(svglContext.viewport_width+.5);
    int height = (int)(svglContext.viewport_height+.5);
    unsigned char * buffer = new unsigned char[width * height * 4];
    err = OSMesaMakeCurrent(mesactx, buffer, GL_UNSIGNED_BYTE, width, height);
    reshape(width,height);
    display();
    {
      std::string fn(filenames[i]);
      std::string::size_type pos1 = fn.rfind('/');
      std::string::size_type pos2 = fn.rfind('.');

      if(pos1==std::string::npos)
	pos1=0;
      else
	++pos1;
      if(pos2==std::string::npos) {
	pos2 = fn.size();
      }
      std::string result = out_dir + fn.substr(pos1, pos2-pos1) + extension;
      if(!quiet) {
	std::cout << "saving " << result << std::endl;
      }
      FILE* f=fopen(result.c_str(),"w");
      screenshot_ppm(f,buffer,width,height);
    }
    delete [] buffer;
  }

  OSMesaDestroyContext(mesactx);
  return 0;
}
