#if defined (__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <svgl/InitHelper.hpp>
#include <svgl/DisplayManager.hpp>

#include <w3c/svg/SVGDocument.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/svg/SVGRectElement.hpp>
#include <w3c/svg/SVGAnimateElement.hpp>
#include <w3c/svg/SVGAnimateColorElement.hpp>

#include <svgl/AnimationManager.hpp>


#include "GlutTimeManager.hpp"

#include <svgl/debug.hpp>

svg::SVGDocument * thesvgdoc=0;
svgl::DisplayManager * displayManager=0;
svgl::Animation::Manager * animationManager=0;
svgl::AnimationInfo * animinfo=0;

svg::SVGDocument*
createSampleDoc()
{
  svg::SVGDocument * doc = new svg::SVGDocument();
  svg::SVGSVGElement * thesvgelt = new svg::SVGSVGElement(doc);
  thesvgelt->setWidth(450);
  thesvgelt->setHeight(450);

  doc->appendChild(thesvgelt);

  svg::SVGRectElement * rect = new svg::SVGRectElement(doc);

  double w=10,h=10;

  rect->setX(-w/2);
  rect->setY(-h/2);
  rect->setWidth(w);
  rect->setHeight(h);
  rect->setFill(0,0,1);
  rect->setStroke(0,0,0);
  rect->setStrokeWidth(2);
      
  // order independant
  //rect->scale(.5);
  //rect->rotate(20);
  rect->translate(100,100);

  thesvgelt->appendChild(rect);

  svg::SVGAnimateElement * anim = new svg::SVGAnimateElement(doc);
  anim->setAttributeName(unicode::String::createString("x"));
  anim->setBegin(unicode::String::createString("0s"));
  anim->setDur(unicode::String::createString("9s"));
  anim->setTo(unicode::String::createString("200"));

  svg::SVGAnimateColorElement * animColor = new svg::SVGAnimateColorElement(doc);
  animColor->setAttributeName(unicode::String::createString("fill"));
  animColor->setAttributeType(svg::SVGAnimationElement::SVG_ANIMATION_ATTRIBUTE_TYPE_CSS);
  animColor->setBegin(unicode::String::createString("0s"));
  animColor->setDur(unicode::String::createString("1s"));
  animColor->setTo(unicode::String::createString("rgb(255,0,0)"));

  rect->appendChild(anim);
  rect->appendChild(animColor);
  thesvgelt->animationTraverse(animinfo);

  glutReshapeWindow(450, 450);
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
}

void
keypress(unsigned char key, int x, int y)
{
  switch (key) {
  case 27: /*ESC*/
    glutDestroyWindow(glutGetWindow());
    exit(0);
    break;
  }
}

struct GlutRedisplayListener : svgl::Animation::RedisplayEventListener {
  virtual void doit(const svgl::Animation::RedisplayEvent&) { glutPostRedisplay(); }
};


int
main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_STENCIL);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keypress);

  try {
    GlutSvgl::GlutTimeManager timeManager(.01);
    GlutRedisplayListener redisplayListener;
    svgl::InitHelper * initHelper = svgl::InitHelper::get(&timeManager, &redisplayListener);
    displayManager = initHelper->displayManager;
    animationManager = initHelper->animationManager;
    animinfo = initHelper->animinfo;
    thesvgdoc = createSampleDoc();
    animationManager->start();

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
