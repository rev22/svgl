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

#include <w3c/dom/ext/DOMWriter.hpp>
#include <w3c/svg/strings.hpp>
#include <w3c/svg/SVGDocument.hpp>
#include <w3c/svg/SVGDefsElement.hpp>
#include <w3c/svg/SVGUseElement.hpp>
#include <w3c/svg/SVGGElement.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/svg/SVGRectElement.hpp>
#include <w3c/svg/SVGTransforms.hpp>
#include <svgl/InitHelper.hpp>

#include <svgl/Parser.hpp>
#include <svgl/printHelper.hpp>
#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/AnimationInfo.hpp>
//#include <glft/FontManager.hpp>
#include <svgl/ImageManager.hpp>
#include "GlutTimeManager.hpp"
#include <svgl/AnimationManager.hpp>
#include <svgl/ExternalEntityManager.hpp>
#include <svgl/PickManager.hpp>
#include <svgl/DisplayManager.hpp>
#include <svgl/screenshot.hpp>
#include <w3c/css/CSSStyleDeclaration.hpp>

#include <svgl/timeval_helpers.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>


#include <iostream>
#include <svgl/xmlostream.hpp>
#include <map>
#include <vector>


#if defined (__APPLE__)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#if defined(WIN32) && ! defined(__CYGWIN__)
#include <direct.h>
#include <sys/timeb.h>
int gettimeofday(struct timeval* tp, void* tzp);
#else
// chdir
#include <stdlib.h>
#endif

// FILE
#include <stdio.h>

namespace svg {
  extern
  void
  init_colors();
}

svg::SVGDocument* gsvgdoc;
svg::SVGSVGElement* gsvgelt;

svgl::Context * svglContext;
svgl::GLInfo * glinfo;
svgl::DisplayManager * displayManager;
svgl::AnimationInfo * animinfo;
svgl::PickManager *pickManager;
svgl::FontManager *fontManager;
svgl::ExternalEntityManager *externalEntityManager;

int num_filenames;
char** filenames;
int last_file_entry=0;
int max_file_entry;

bool debugDisplay;
GLenum debugPolygonMode;
bool debugDoubleBuffer;
static bool createList;
static GLuint displayList;

int xlast=0;
int ylast=0;
double xclickpan, yclickpan;
// global height and width
int gw;
int gh;

bool animation_started=false;
bool animation_paused=false;


FILE* debugFile;

GLenum screenshotType;

std::vector<svg::SVGSVGElement*> gc_dbg_save;

void
printDocument(svg::SVGDocument * doc)
{
#if 1
    if(doc) {
      std::cerr << "writing document..." << __FL__;
      dom::DOMWriter writer(std::cerr);
      writer.visitDocument(doc);      
      std::cerr << std::endl << "document written." << __FL__;
    }
#else
    if(gsvgelt) {
      xmlostream out(std::cerr);
      gsvgelt->print(out);
    }
#endif
}

bool
createSampleDoc()
{
  svg::SVGDocument * thesvgdoc = new svg::SVGDocument();

  svg::SVGSVGElement * thesvgelt = new svg::SVGSVGElement(thesvgdoc);
  //thesvgelt = dynamic_cast<svg::SVGSVGElement*>(thesvgdoc->createElement(unicode::String::internString("svg")));
  svg::SVGLength& widthl = const_cast<svg::SVGLength&> (thesvgelt->GETVAL(Width));
  svg::SVGLength& heightl =  const_cast<svg::SVGLength&> (thesvgelt->GETVAL(Height));

  //std::cerr << DBGVAR(&widthl) << DBGVAR(&heightl) << __FL__
  widthl.SET_SIMPLE_VAL(Value,450);
  heightl.SET_SIMPLE_VAL(Value,450);
  svglContext->setViewportWidthHeight(450, 450);
  //svglContext->externalEntityManager->setDocumentName(unicode::String::createString(""));
  svglContext->externalEntityManager->register_(thesvgdoc,unicode::String::createString(""));
  svglContext->svgOwner = thesvgelt;

  thesvgdoc->insertBefore(thesvgelt, 0);

  {
    svg::SVGRectElement * rect = new svg::SVGRectElement(thesvgdoc);
    //svg::SVGRectElement * rect = dynamic_cast<svg::SVGRectElement*>(thesvgdoc->createElement(unicode::String::internString("rect")));
  svg::SVGLength& widthl = const_cast<svg::SVGLength&> (rect->GETVAL(Width));
  svg::SVGLength& heightl =  const_cast<svg::SVGLength&> (rect->GETVAL(Height));

  widthl.newValueSpecifiedUnits(svg::SVGLength::SVG_LENGTHTYPE_NUMBER, 100);
  heightl.newValueSpecifiedUnits(svg::SVGLength::SVG_LENGTHTYPE_NUMBER, 100);

  svg::SVGLength& xl = const_cast<svg::SVGLength&> (rect->GETVAL(X));
  svg::SVGLength& yl =  const_cast<svg::SVGLength&> (rect->GETVAL(Y));

  xl.newValueSpecifiedUnits(svg::SVGLength::SVG_LENGTHTYPE_NUMBER, 150);
  yl.newValueSpecifiedUnits(svg::SVGLength::SVG_LENGTHTYPE_NUMBER, 150);

  thesvgelt->insertBefore(rect, 0);
  }

  thesvgdoc->updateStyle();

  animinfo->animationManager->stop();
  animinfo->animationManager->unsubscribe_all();

  thesvgelt->animationTraverse(animinfo);

  glutReshapeWindow(static_cast<int>(450), static_cast<int>(450));
  glutSetWindowTitle("sampleDoc");
  return true;

}


svg::SVGDocument *
simplyLoadSvgFile(const char* filename)
{
  svgl::Parser parser;
  svg::SVGDocument *thesvgdoc=0;
  try {
    thesvgdoc = parser.parse(filename);
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
  return thesvgdoc;
}



svg::SVGDocument *
loadSvgFile(const char* filename)
{
  svg::SVGDocument *thesvgdoc=0;
  thesvgdoc=simplyLoadSvgFile(filename);

  if(!thesvgdoc)
    return 0;

  thesvgdoc->updateStyle();

#if 0
  {
    std::cerr << "writing document..." << __FL__;
    dom::DOMWriter writer(std::cerr);
    //std::cout << std::flush;
    writer.visitDocument(thesvgdoc);
    std::cerr << std::endl << "document written." << __FL__;

  }
#endif

  animinfo->animationManager->stop();
  animinfo->animationManager->unsubscribe_all();

  //gc_dbg_save.push_back(thesvgelt);

  //svg::SVGSVGElement * thesvgelt = thesvgdoc->getRootElement().getValue();
  svg::SVGSVGElement * thesvgelt = thesvgdoc->GET_SIMPLE_VAL(RootElement);
  if(!thesvgelt) {
    std::cerr << "no svg element in " << filename << __FL__;
    return 0;
  }
  thesvgelt->animationTraverse(animinfo);

  //std::cerr << DBGVAR(thesvgelt) << __FL__;

  //const svg::SVGLength& widthl = thesvgelt->GETVAL(Width);
  svg::SVGLength widthl = thesvgelt->GETVAL(Width);
  svg::SVGLength heightl = thesvgelt->GETVAL(Height);
	const svg::SVGRect& viewbox = thesvgelt->GETVAL(ViewBox);
  //std::cerr << DBGVAR(widthl) << DBGVAR(heightl) << __FL__
	float width, height;
	width = widthl.computeValue(svglContext->dpi, viewbox.getWidth(), svglContext->fontSize, svglContext->fontXHeight);
	height = heightl.computeValue(svglContext->dpi, viewbox.getHeight(), svglContext->fontSize, svglContext->fontXHeight);
	
  std::cerr << DBGVAR(width) << DBGVAR(height) << __FL__
  //std::cerr << DBGVAR(widthl.getValueAsString()) << DBGVAR(heightl.getValueAsString()) << __FL__

  svglContext->setViewportWidthHeight(width, height);
  //svglContext->externalEntityManager->setDocumentName(unicode::String::createString(filename));
  svglContext->externalEntityManager->register_(thesvgdoc,unicode::String::createString(filename));
  svglContext->svgOwner = thesvgelt;

  glutReshapeWindow(static_cast<int>(width), static_cast<int>(height));
  glutSetWindowTitle(filename);
  return thesvgdoc;
}

svg::SVGDocument *
loadAllSvgFiles()
{
  svg::SVGDocument * svgdoc = new svg::SVGDocument();
  svg::SVGSVGElement * svgelt = new svg::SVGSVGElement(svgdoc);
  svgdoc->insertBefore(svgelt, 0);
  //svgelt->setOwnerAndViewPort(svgelt,svgelt);

  // definitions
  svg::SVGDefsElement * defs = new svg::SVGDefsElement(svgdoc);
  svgelt->insertBefore(defs, 0);
  // main canvas
  svg::SVGGElement * view = new svg::SVGGElement(svgdoc);
  svgelt->insertBefore(view, 0);

  float totalWidth=0;

  for(int i=0; i<num_filenames;++i) {
    svg::SVGDocument * tmpdoc = simplyLoadSvgFile(filenames[i]);
    if(!tmpdoc)
      continue;
    //printDocument(toolglass);

    externalEntityManager->register_(tmpdoc, unicode::String::createString(filenames[i]));

    svg::SVGSVGElement * selt = tmpdoc->GET_SIMPLE_VAL(RootElement);
    totalWidth += selt->getWidth().GET_SIMPLE_VAL(Value);
  }

  float tx=0, ty=0;
  float maxWidth = totalWidth/(sqrt(num_filenames)+1);
  float maxHeight=0;
  for(int i=0; i<num_filenames;++i) {
    svg::SVGDocument * tmpdoc = dynamic_cast<svg::SVGDocument*>(externalEntityManager->get(unicode::String::createString(filenames[i])));
    if(!tmpdoc) {
      STDDBG;
      continue;
    }

    svg::SVGSVGElement * selt = tmpdoc->GET_SIMPLE_VAL(RootElement);
    unicode::String * seltidref = selt->GET_SIMPLE_VAL(Id);

    if(!seltidref) {
      seltidref = unicode::String::internString("mainsvg");
      selt->SET_SIMPLE_VAL(Id,seltidref);
      tmpdoc->putIdentifier(seltidref, selt);
    }
    //std::cerr << seltidref << " " << selt->GET_SIMPLE_VAL(Id) << __FL__;

    unicode::String * href = concat(unicode::String::createString(filenames[i]), concat(unicode::String::createString("#"),seltidref));

    svg::SVGUseElement * use = new svg::SVGUseElement(svgdoc);
    use->setWidth(selt->getWidth());
    use->setHeight(selt->getHeight());      
    use->translate(100,100);
    use->setHref(href);
    view->insertBefore(use, 0);
    svg::SVGTransformList& transformList = const_cast<svg::SVGTransformList&>(use->GETVAL(Transform));
    {
      svg::SVGTranslateTransform::ArgsContainer args;
      args.push_back(tx);
      args.push_back(ty);
      svg::SVGTranslateTransform * viewTranslate = new svg::SVGTranslateTransform(args);
      //viewTranslate->setNeutral();
      transformList.insertItemBefore(viewTranslate, 0);
    }
    svg::SVGLength& widthl = const_cast<svg::SVGLength&> (selt->GETVAL(Width));
    float width = widthl.GET_SIMPLE_VAL(Value);
    float height = selt->getHeight().GET_SIMPLE_VAL(Value);
    if(height>maxHeight)
      maxHeight=height;
    if(tx>maxWidth) {
      tx=0;
      ty += maxHeight;
      maxHeight=0;
    }
    tx+=width;
  }

  svgelt->setOwnerAndViewPort(svgelt,svgelt);
  //printDocument(svgdoc);
#if 0
  std::cerr << "--------------------" << __FL__;
  for(dom::Document::Ids::const_iterator idit = svgdoc->id_begin();
      idit != svgdoc->id_end();
      ++idit) {
    unicode::String * id = idit->first;
    std::cerr << DBGVAR(id) << DBGVAR(idit->second) << __FL__;
  }
#endif
  svgdoc->updateStyle();

  animinfo->animationManager->stop();
  animinfo->animationManager->unsubscribe_all();

  svgelt->animationTraverse(animinfo);
  svg::SVGLength& widthl = const_cast<svg::SVGLength&> (svgelt->GETVAL(Width));
  svg::SVGLength& heightl =  const_cast<svg::SVGLength&> (svgelt->GETVAL(Height));

  widthl.SET_SIMPLE_VAL(Value,450);
  heightl.SET_SIMPLE_VAL(Value,450);
  float width = widthl.GET_SIMPLE_VAL(Value);
  float height = heightl.GET_SIMPLE_VAL(Value);

  //std::cerr << DBGVAR(width) << DBGVAR(height) << __FL__;

  svglContext->setViewportWidthHeight(width, height);
  svglContext->externalEntityManager->register_(svgdoc,unicode::String::createString("all"));
  svglContext->svgOwner = svgelt;

  glutReshapeWindow(static_cast<int>(width), static_cast<int>(height));
  glutSetWindowTitle("all");
  return svgdoc;
}


void
selectSvgFile(int entry)
{
  if(entry>=0 && entry<=max_file_entry) {
    last_file_entry = entry;
    std::cerr << "loading " << filenames[entry] << "..." << std::endl;
    gsvgdoc=loadSvgFile(filenames[entry]);
    if(gsvgdoc) {
      gsvgelt = gsvgdoc->GET_SIMPLE_VAL(RootElement);
      glutPostRedisplay();
      std::cerr << "loaded." << std::endl;
    }
    else
      std::cerr << "loading failed." << std::endl;
  } else {
    switch(entry-(max_file_entry+1)) {
    case 0:
      // all
      std::cerr << "all" << std::endl;
      gsvgdoc=loadAllSvgFiles();
      if(gsvgdoc) {
	gsvgelt = gsvgdoc->GET_SIMPLE_VAL(RootElement);
	//std::cerr << DBGVAR(gsvgelt) << __FL__;
	glutPostRedisplay();
      }
      break;
    default:
      break;
    }
  }

}

void
selectNextSvgFile()
{
  if(last_file_entry<max_file_entry) {
    int entry=last_file_entry+1;
    selectSvgFile(entry);
  }
}

void
selectPreviousSvgFile()
{
  if(last_file_entry>0) {
    int entry=last_file_entry-1;
    selectSvgFile(entry);
  }
}

void screenshotStencil()
{
  screenshotType=GL_STENCIL_INDEX;
  svgl::screenshot(gw,gh,screenshotType);
}

void screenshotRGB()
{
  screenshotType=GL_RGB;
  svgl::screenshot(gw,gh,screenshotType);
}




enum menu_entry {
  reset_menu,
  screenshot_menu,
  debug_display_menu,
  debug_polygonmode_menu,
  debug_doublebuffer_menu,
  debug_globaldlist_menu,
  debug_print_menu,
  debug_antialiasing_menu,
  start_animation_menu,
  stop_animation_menu,
  pause_animation_menu,
  continue_animation_menu,
  quit_menu
};


void
mainmenu(int entry)
{
  switch(entry) {
  case reset_menu:
    glinfo->xpan=glinfo->ypan=0;
    glinfo->zoom=1;
    glutPostRedisplay();
    break;
  case screenshot_menu:
    screenshotRGB();
    break;
  case debug_display_menu:
    debugDisplay = !debugDisplay;
    glutPostRedisplay();
    break;
  case debug_polygonmode_menu:
    if(debugPolygonMode==GL_LINE)
      debugPolygonMode=GL_FILL;
    else
      debugPolygonMode=GL_LINE;
    glPolygonMode(GL_FRONT_AND_BACK, debugPolygonMode);
    glutPostRedisplay();
    break;
  case debug_doublebuffer_menu:
    debugDoubleBuffer=!debugDoubleBuffer;
    if(!debugDoubleBuffer) {
      glDrawBuffer(GL_FRONT);
    }
    else {
      glDrawBuffer(GL_BACK);
    }
    break;
  case debug_globaldlist_menu:
    createList = ! createList;
    if(debugFile)
      fprintf(debugFile, "global list: %d\n", createList);
    glutPostRedisplay();
    break;
  case debug_print_menu:
    printDocument(gsvgdoc);
    break;
  case debug_antialiasing_menu:
    if (glinfo->doAntiAliasing())
      glinfo->antialiasing=false;
    else
      glinfo->antialiasing=true;
    glutPostRedisplay();
    break;

  case start_animation_menu:
    animinfo->animationManager->start();
    animation_started=true;    
    break;
  case stop_animation_menu:
    animinfo->animationManager->stop();
    animation_started=false;
    break;
  case pause_animation_menu:
    animinfo->animationManager->pause();
    animation_paused=true;
    break;
  case continue_animation_menu:
    animinfo->animationManager->continue_();
    animation_paused=false;
    break;
  case quit_menu:
    glutDestroyWindow(glutGetWindow());
    exit(0);
    break;
  }
}

void
special(int key, int x, int y)
{
  switch(key) {
  case GLUT_KEY_UP: selectNextSvgFile(); break;
  case GLUT_KEY_DOWN: selectPreviousSvgFile(); break;
  }
}


void
keypress(unsigned char key, int x, int y)
{
  switch (key) {

  case 'r': mainmenu(reset_menu); break;
  case 'l': mainmenu(debug_globaldlist_menu); break;
  case 'a': mainmenu(debug_antialiasing_menu); break;
  case 'o': mainmenu(debug_polygonmode_menu); break;

  case 's':
    if(!animation_started)
      mainmenu(start_animation_menu);
    else
      mainmenu(stop_animation_menu);
    break;
  case 'p': 
    if(!animation_paused)
      mainmenu(pause_animation_menu);
    else
      mainmenu(continue_animation_menu);
    break;

  case 'q':
  case 27: /*ESC*/
    mainmenu(quit_menu);
    break;

  }

}


enum state_t {
  idle,
  panning,
  zooming,
  shape_moving
};

state_t currentstate;
svg::SVGTranslateTransform * translate;

void
motion(int x, int y)
{
  int dx = x-xlast;
  int dy = y-ylast;
  switch(currentstate) {
  case panning:
    glinfo->xpan -= dx/glinfo->zoom;
    glinfo->ypan -= dy/glinfo->zoom;
    glutPostRedisplay();
    break;
  case zooming: {
      double sscale = exp2(dy/100.);
	  //double sscale = exp(log(2)*dy/100.);
      double newscale = glinfo->zoom*sscale;
      double dscale = newscale-glinfo->zoom;
      double f = dscale/(glinfo->zoom+dscale);
    // this one
    glinfo->xpan += (xclickpan - glinfo->xpan) * f;
    glinfo->ypan += (yclickpan - glinfo->ypan) * f;

    glinfo->zoom += dscale;

    glutPostRedisplay();
  }
  break;
  case shape_moving: {
    translate->tx += dx/glinfo->zoom;
    translate->ty += dy/glinfo->zoom;
    glutPostRedisplay();
  }    
  break;
  default:
    break;
  }
  xlast=x;
  ylast=y;
}

void
button(int num, int state, int x, int y)
{
  int modifiers = glutGetModifiers();
  switch (state) {

  case GLUT_DOWN:
    switch(num) {
    case GLUT_LEFT_BUTTON:
      xlast=x;
      ylast=y;
      if(modifiers & GLUT_ACTIVE_CTRL) {
	glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
	currentstate=panning;
      }
      else if(modifiers & GLUT_ACTIVE_SHIFT) {
	xclickpan = x/glinfo->zoom + glinfo->xpan;
	yclickpan = y/glinfo->zoom + glinfo->ypan;

	glutSetCursor(GLUT_CURSOR_UP_DOWN);
	currentstate=zooming;
      }
      else {
	svgl::PickManager::PickResult p = pickManager->pick(gsvgelt, svglContext, glinfo, x,y,5,5);

	if(p.first==p.second) {
	  // nothing hit
	  glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
	  currentstate=panning;
	}
	else {

	  svgl::PickManager::HitElementIterator remember;
	  bool remember_ok=false;
	  //xmlostream out(std::cerr);
	  //std::cerr << "process hits" << __FL__;
	  //int i=0;
	  std::cerr << "hit on (" << x << "," << y << "): ";
	  for(; p.first!=p.second; ++p.first) {
	    //std::cerr << "hit #" << ++i << __FL__;
	    svgl::PickManager::HitIterator::value_type onehit = *p.first;
	    //std::cerr << DBGVAR(*onehit.first) << DBGVAR(*onehit.second) << __FL__;
	    for(; onehit.first!=onehit.second; ++onehit.first) {
	      //std::cerr << "===================" << __FL__;
	      //(*onehit.first)->print(out);
	      //std::cerr << "===================" << __FL__;
	      //std::cerr << "<" << (*onehit.first)->getDtdElement() << ">";
	      if((*onehit.first)->GET_SIMPLE_VAL(Id)) {
		std::cerr << "<" << (*onehit.first)->getDtdElement() << " \"" << (*onehit.first)->GET_SIMPLE_VAL(Id) << "\" >";
	      }
	      else {
		std::cerr << "<" << (*onehit.first)->getDtdElement() << ">";
	      }
	      remember = onehit.first;
	      remember_ok=true;
	    }
	  }
	  std::cerr << __FL__;
	  if(remember_ok) {
	    //std::cerr << "<" << (*remember)->getDtdElement() << ">";
	    svg::SVGTransformable * transformable = dynamic_cast<svg::SVGTransformable * >(*remember);
	    if(transformable) {
	      svg::SVGTransformList& transformList = const_cast<svg::SVGTransformList&>(transformable->GETVAL(Transform));
	      translate = 0;
	      if(transformList.numberOfItems()!=0) {
		svg::SVGTransform * transform = transformList.getItem(0);
		translate = dynamic_cast<svg::SVGTranslateTransform*>(transform);
	      }
	      if(!translate) {
		svg::SVGTranslateTransform::ArgsContainer args;
		args.push_back(0);
		translate = new svg::SVGTranslateTransform(args);
		translate->setNeutral();
		transformList.insertItemBefore(translate, 0);
	      }
	      currentstate = shape_moving;
	    }
	  }
	}
      }
      break;

    case GLUT_MIDDLE_BUTTON:
      xlast=x;
      ylast=y;

      xclickpan = x/glinfo->zoom + glinfo->xpan;
      yclickpan = y/glinfo->zoom + glinfo->ypan;
      
      glutSetCursor(GLUT_CURSOR_UP_DOWN);
      currentstate=zooming;
      break;
    }
    break;
    
  case GLUT_UP:
    if(currentstate!=idle) {
      currentstate = idle;
      glutSetCursor(GLUT_CURSOR_INHERIT);
    }
    break;
  }
}
  

void
reshape(int w, int h)
{
  //std::cerr << "reshape " << w << " " << h << __FL__;
  displayManager->reshape(svglContext, glinfo, gsvgelt, w, h);

  gw=w;
  gh=h;
  glFinish();
}

void
displayString(const char* str, int y)
{
  const char* s = str;
  int w=0;
  for(s=str; *s; ++s) {
    w+=glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_10, *s);
  }

  glRasterPos2i(-w-5, y);

  for(s=str; *s; ++s) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *s);
  }

}

void
displayDebugInfo(int delta_time)
{
  glinfo->pushWindowMode();
  glColor3f(1,0,0);
  glTranslatef(glinfo->winWidth, 0,0);

  int ystart = 15;
  const int ydelta = 10;

  // FIXME
  if(!delta_time)
    delta_time=1;
  if(delta_time) {
    float fps=1000000./delta_time;
    char buffer[33];
    sprintf(buffer, "%4.2f ms / %4.2f fps", delta_time/1000., fps);
    buffer[30]=0;
    displayString(buffer, ystart);
    if(debugFile)
      fprintf(debugFile, "%d\n", delta_time);
  }
  ystart+=ydelta;
  {
    char buffer[25];
    sprintf(buffer, "x:%4.2f y:%4.2f s:%4.2f", glinfo->xpan, glinfo->ypan, glinfo->zoom);
    buffer[24]=0;
    displayString(buffer, ystart);
  }
  ystart+=ydelta;

  const char * vendor = (const char*)glGetString(GL_RENDERER);
  displayString(vendor, ystart);
  ystart+=ydelta;


  glinfo->popWindowMode();

}

void
display()
{
  if (!gsvgelt)
    return;
  timeval start_time, end_time;
  //  start_time = glutGet(GLUT_ELAPSED_TIME);
  gettimeofday(&start_time,0);

  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  glinfo->setZoomAndPan();

  if (createList) {
    if (displayList == 0) {
      displayList = glGenLists(1);
      glNewList(displayList, GL_COMPILE_AND_EXECUTE);
      gsvgelt->glTraverse(svglContext, glinfo);
      glEndList();
    }
    else
      glCallList(displayList);
  }
  else {
    if (displayList != 0) {
      glDeleteLists(displayList, 1);
      displayList = 0;
    }
    try {
      gsvgelt->glTraverse(svglContext, glinfo);
    }
		catch (const svgl::Parser::exception& e) {
				std::cerr << e.what() << __FL__;
				throw;
			}
		catch (dom::DOMException& e) {
			std::cerr << e.what() << __FL__;
			throw;
		}
		catch (std::exception& e) {
			std::cerr << e.what() << __FL__;
			throw;
		}
		catch (...) {
			std::cerr << "unknown exception" << __FL__;
			throw;
		}
		
  }
  glFinish();
  //end_time = glutGet(GLUT_ELAPSED_TIME);
  gettimeofday(&end_time,0);
  int delta_time =  timeval_to_us((end_time-start_time));
  if(debugDisplay) {
    displayDebugInfo( delta_time );
  }
 
  if(debugDoubleBuffer) {
    glutSwapBuffers();
  }
  else {
    glFlush();
  }
  
  int err = glGetError();
  if(err) {
    std::cerr << "GL error: " << gluErrorString(err) << __FL__;
    createList = false;
  }
}

struct GlutRedisplayListener : svgl::Animation::RedisplayListener {
  virtual void doit(const svgl::Animation::RedisplayEvent&) { glutPostRedisplay(); }
};

struct A {virtual ~A() {}};
struct B : A {};

int
main(int argc, char** argv)
{
#if SVGL_USE_GC
  GC_dont_gc=0;
#endif

  {
    // test for C++ link
    // if it seg faults, then some library was compiled with another compiler
    A * tmp = new A;
    B * tmpb = dynamic_cast<B*>(tmp);
    tmpb=0;
  }

  try {
  if(argc <2) {
    std::cerr << "usage: " << argv[0] << " file1.svg file2.Svg ..." << __FL__;
    return 1;
  }
  num_filenames = argc-1;
  filenames = &argv[1];

  // Glut

  glutInit(&argc, argv);
	int initDisplayMode = GLUT_RGBA | GLUT_DOUBLE | GLUT_STENCIL;
	initDisplayMode |= GLUT_MULTISAMPLE;
	initDisplayMode |= GLUT_ALPHA;
  glutInitDisplayMode( initDisplayMode );

  glutCreateWindow("no file");
  
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keypress);
  glutMotionFunc(motion);
  glutMouseFunc(button);
  glutSpecialFunc(special);

  typedef std::map < std::string, int > Menus;
  Menus menus;
  max_file_entry = num_filenames-1;

  int loadmenu=glutCreateMenu(selectSvgFile);
  glutAddMenuEntry("all", max_file_entry+1);
  for (int i=0; i<num_filenames; ++i) {
    std::string fn(filenames[i]);
    std::string::size_type pos = fn.rfind('/');
    if(pos!=std::string::npos) {
      std::string::size_type pos2 = fn.find('-', pos);
      if(pos2!=std::string::npos) {
	std::string menuname = fn.substr(pos+1, pos2-(pos+1));
	if (!menus.count(menuname) )
	  menus[menuname] = glutCreateMenu(selectSvgFile);
	glutAddMenuEntry(fn.substr(pos2+1).c_str(), i);
      }
      else {
	glutSetMenu(loadmenu);
	glutAddMenuEntry(fn.substr(pos+1).c_str(), i);
      }

    }
  }


  glutSetMenu(loadmenu);
  if (menus.size()) {
    for (Menus::const_iterator it = menus.begin();
	 it != menus.end();
	 ++it) {
      glutAddSubMenu((*it).first.c_str(), (*it).second);
    }
  }

  int animmenu = glutCreateMenu(mainmenu);
  glutAddMenuEntry("start", start_animation_menu);
  glutAddMenuEntry("stop", stop_animation_menu);
  glutAddMenuEntry("pause", pause_animation_menu);
  glutAddMenuEntry("continue", continue_animation_menu);

  int debugmenu = glutCreateMenu(mainmenu);
  glutAddMenuEntry("display", debug_display_menu);
  glutAddMenuEntry("polygon mode", debug_polygonmode_menu);
  glutAddMenuEntry("single/double buffer", debug_doublebuffer_menu);
  glutAddMenuEntry("global display list", debug_globaldlist_menu);
  glutAddMenuEntry("print document", debug_print_menu);
  glutAddMenuEntry("antialiasing", debug_antialiasing_menu);

  glutCreateMenu(mainmenu);
  glutAddMenuEntry("reset view", reset_menu);
  if(loadmenu)
    glutAddSubMenu("load", loadmenu);
  glutAddSubMenu("animation", animmenu);
  glutAddMenuEntry("screenshot", screenshot_menu);
  glutAddSubMenu("debug", debugmenu);
  glutAddMenuEntry("quit", quit_menu);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  GlutSvgl::GlutTimeManager timeManager(.01);
  GlutRedisplayListener redisplayListener;
  svgl::InitHelper initHelper(&timeManager, &redisplayListener);
  svglContext = initHelper.context;
  glinfo = initHelper.glinfo;
  animinfo = initHelper.animinfo;
  pickManager = initHelper.pickManager;
  externalEntityManager = initHelper.externalEntityManager;

  selectSvgFile(0);
  //selectSvgFile(max_file_entry+1);
  //createSampleDoc();

  currentstate=idle;
  debugDisplay = true;
  debugPolygonMode=GL_FILL;
  debugDoubleBuffer=true;
  glPolygonMode(GL_FRONT_AND_BACK, debugPolygonMode);
  //debugFile = fopen("fps.txt","w");
  //screenshotType = GL_RGB;
  screenshotType = GL_STENCIL_INDEX;

  glutMainLoop();

  }
  catch (const svgl::Parser::exception& e) {
    std::cerr << e.what() << __FL__;
    throw;
  }
  catch (dom::DOMException& e) {
    std::cerr << e.what() << __FL__;
    throw;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << __FL__;
    throw;
  }
  catch (...) {
    std::cerr << "unknown exception" << __FL__;
    throw;
  }

  return 0;

}
