#if defined (__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <svgl/InitHelper.hpp>
#include <svgl/DisplayManager.hpp>
#include <svgl/Parser.hpp>

#include <w3c/svg/SVGDocument.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/svg/SVGUseElement.hpp>
#include <w3c/svg/SVGGElement.hpp>
#include <w3c/svg/SVGCircleElement.hpp>
#include <svgl/ExternalEntityManager.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

#include "panAndZoom.hpp"
#include <w3c/svg/SVGTransforms.hpp>


svg::SVGDocument * thesvgdoc=0;
svgl::DisplayManager * displayManager=0;


char svgdesc [] = "<?xml version='1.0' encoding='UTF-8'?>"
"<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.0//EN' 'http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd'>"
"<svg width='450' height='450' xmlns:xlink='http://www.w3.org/1999/xlink'>"
"<g id='view'>"
"   <defs>"
"      <g id='foo' stroke-width='3'>"
"         <rect id='rect' width='100' height='100' fill='black'/>"
"         <circle id='circle' r='50' fill='red' transform='translate(200,50)'/>"
"      </g>"
"      <g id='canvas'>"
"         <use xlink:href='#foo' id='use1' transform='translate(100,100)'/>"
"         <use xlink:href='#foo' id='use2' transform='translate(100,250) scale(.1)' stroke='green'/>"
"      </g>"
"   </defs>"
"   <use id='view1' xlink:href='#canvas' stroke='blue'/>"
"   <use id='view2' xlink:href='#canvas' transform='translate(0,400) scale(.1)' stroke='pink'/>"
"</g>"
"</svg>"
;

svg::SVGDocument*
createSampleDoc(const char * useHref)
{
  // useless but necessary because of a bug on macosx
  svg::SVGDocument * doc = new svg::SVGDocument();
  svg::SVGSVGElement * thesvgelt = new svg::SVGSVGElement(doc);

  doc=0;
  {
    for(char * s = svgdesc; *s; ++s) {
      if ( (*s)=='\'') (*s)='"';
      if ( (*s)=='\n') (*s)=' ';
    }
  }
  //std::cerr << svgdesc << __FL__;
  svgl::Parser parser;
  try {
    //STDDBG;
    doc = parser.parseFromString(unicode::String::createString(svgdesc));
    //doc = parser.parse(useHref);
    //STDDBG;
    if(doc) {
      const svg::SVGSVGElement * thesvgelt = doc->getRootElement();
      glutReshapeWindow((int)thesvgelt->getWidth().getValue(), (int)thesvgelt->getHeight().getValue());
    }
  }
  catch (const svgl::Parser::exception& e) {
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

  {
    svg::SVGGElement *view = dynamic_cast<svg::SVGGElement*>(doc->getElementById(unicode::String::createString("view")));
    panAndZoom::panTransform = new svg::SVGTranslateTransform;
    panAndZoom::zoomTransform = new svg::SVGScaleTransform;
    svg::SVGTransformList& l = const_cast<svg::SVGTransformList&>(view->getTransform());
    l.appendItem(panAndZoom::zoomTransform);
    l.appendItem(panAndZoom::panTransform);
  }

  {
    svg::SVGElement *svge = dynamic_cast<svg::SVGElement*>(doc->getElementById(unicode::String::createString("view1")));
    if(svge) svge->compile(true);
  }

  {
    //svg::SVGElement *svge = dynamic_cast<svg::SVGElement*>(doc->getElementById(unicode::String::createString("rect")));
    //if(svge) svge->compile(true);
  }

#if 0
  {
    svg::SVGElement *svge = dynamic_cast<svg::SVGElement*>(doc->getElementById(unicode::String::createString("foo")));
    std::pair<svgl::ExternalEntityManager::UsedBy::iterator, svgl::ExternalEntityManager::UsedBy::iterator> p  = svgl::ExternalEntityManager::getDefault()->getUsers(svge);

    std::cerr << "users of <" << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
    for(; p.first != p.second; ++(p.first)) {
      svg::SVGElement * svge = dynamic_cast<svg::SVGElement*>(p.first->second);
      if(svge)
	std::cerr << "<"  << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
    }

  }
#endif
  

  glutReshapeWindow(450, 450);
  //STDDBG;
  return doc;
}

void
reshape(int w, int h)
{
  displayManager->reshape(w, h);
}

void
display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  displayManager->display(thesvgdoc);

  glutSwapBuffers();

  int err = glGetError();
  if(err) {
    std::cerr << "GL error: " << gluErrorString(err) << __FL__;
  }  
#if 0
  {
    svg::SVGElement *svge = dynamic_cast<svg::SVGElement*>(thesvgdoc->getElementById(unicode::String::createString("foo")));
    std::pair<svgl::ExternalEntityManager::UsedBy::iterator, svgl::ExternalEntityManager::UsedBy::iterator> p  = svgl::ExternalEntityManager::getDefault()->getUsers(svge);

    std::cerr << "users of <" << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
    for(; p.first != p.second; ++(p.first)) {
      svg::SVGElement * svge = dynamic_cast<svg::SVGElement*>(p.first->second);
      if(svge)
	std::cerr << "<"  << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
    }

  }
#endif

}

void
keypress(unsigned char key, int x, int y)
{
  switch (key) {
  case 27: /*ESC*/
  case 'q':
    glutDestroyWindow(glutGetWindow());
    exit(0);
    break;

  case 'c': {
    svg::SVGCircleElement *svge = dynamic_cast<svg::SVGCircleElement*>(thesvgdoc->getElementById(unicode::String::createString("circle")));
    svge->setFill(0,1,1);
    glutPostRedisplay();
  }
    break;

  case 'd': {
    svg::SVGCircleElement *svge = dynamic_cast<svg::SVGCircleElement*>(thesvgdoc->getElementById(unicode::String::createString("circle")));
    //svge->translate(200,100);
    svge->scale(2);
    glutPostRedisplay();
  }
    break;

  case 'u': {
    svg::SVGStylable *svge = dynamic_cast<svg::SVGStylable*>(thesvgdoc->getElementById(unicode::String::createString("use1")));
    svge->setFill(.5,1,.5);
    glutPostRedisplay();
  }
    break;

  case 'v': {
    svg::SVGTransformable *svge = dynamic_cast<svg::SVGTransformable*>(thesvgdoc->getElementById(unicode::String::createString("use1")));
    svge->scale(2);
    glutPostRedisplay();
  }
    break;

  case 'r':
    glutPostRedisplay();
    break;

  }

}

int
main(int argc, char **argv)
{
  //STDDBG;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_STENCIL | GLUT_MULTISAMPLE);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keypress);
  glutMotionFunc(panAndZoom::motion);
  glutMouseFunc(panAndZoom::button);

  try {
    //STDDBG;
    svgl::InitHelper * initHelper = svgl::InitHelper::get();
    //STDDBG;
    displayManager = initHelper->displayManager;

    const char * href="";
    if(argc==2) {
      href=argv[1];
    }
    //STDDBG;
    thesvgdoc = createSampleDoc(href);
    glutMainLoop();
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
