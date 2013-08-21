#if defined (__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <svgl/InitHelper.hpp>
#include <svgl/DisplayManager.hpp>
#include <svgl/PickManager.hpp>

#include <w3c/svg/SVGDocument.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/svg/SVGRectElement.hpp>
#include <w3c/svg/SVGGElement.hpp>
#include <w3c/svg/SVGTransforms.hpp>
#include <w3c/svg/SVGTransformable.hpp>

#include <svgl/getattr.hpp>

#include <svgl/debug.hpp>

#include "../../tests/panAndZoom.hpp"
#include <iostream>

#include <assert.h>

svg::SVGDocument * thesvgdoc=0;
svgl::DisplayManager * displayManager=0;
svgl::PickManager * pickManager=0;

svg::SVGTranslateTransform * translate = 0;
svg::SVGMatrixTransform invTransform;

enum state_t {
  idle,
  panning,
  zooming,
  moving
};

state_t currentState=idle;

int xlast=0;
int ylast=0;

svg::SVGDocument*
createSampleDoc()
{
  svg::SVGDocument * doc = new svg::SVGDocument();
  svg::SVGSVGElement * thesvgelt = new svg::SVGSVGElement(doc);
  thesvgelt->setWidth(450);
  thesvgelt->setHeight(450);

  doc->appendChild(thesvgelt);

  svg::SVGGElement * view = new svg::SVGGElement(doc);
  thesvgelt->appendChild(view);
  
	panAndZoom::panTransform = new svg::SVGTranslateTransform;
  panAndZoom::zoomTransform = new svg::SVGScaleTransform;
  svg::SVGTransformList& l = const_cast<svg::SVGTransformList&>(view->getTransform());
  l.appendItem(panAndZoom::zoomTransform);
  l.appendItem(panAndZoom::panTransform);

  double w=100,h=100;

  svg::SVGGElement * g = new svg::SVGGElement(doc);
  view->appendChild(g);
  //g->scale(0.2);
  g->rotate(45);
  
  svg::SVGRectElement * rect = new svg::SVGRectElement(doc);
  rect->setX(-w/2);
  rect->setY(-h/2);
  rect->setWidth(w);
  rect->setHeight(h);
  rect->setFill(0,0,1);
  rect->setStroke(0,0,0);
  rect->setStrokeWidth(2);
  rect->translate(100,100);
  //rect->scale(0.5);
  g->appendChild(rect);

  rect = new svg::SVGRectElement(doc);
  rect->setX(-w/2);
  rect->setY(-h/2);
  rect->setWidth(w);
  rect->setHeight(h);
  rect->setFill(1,0,0);
  rect->setStroke(0,0,0);
  rect->setStrokeWidth(2);
  rect->translate(230,300);
  view->appendChild(rect);

  glutReshapeWindow(450, 450);
  return doc;
}


std::ostream&
operator<< (std::ostream& out, const svg::SVGMatrix& m)
{
    out << "a:" << m.getA() << " b:" << m.getB() << " c:" << m.getC() << std::endl;
    out << "d:" << m.getD() << " e:" << m.getE() << " f:" << m.getF() << std::endl;
    return out;
}

void
setupTranslation(svg::SVGElement *e)
{
    svg::SVGTransformable * transformable = dynamic_cast<svg::SVGTransformable *>(e);
    assert(transformable);
    translate = transformable->getTranslate(true);
    svg::SVGMatrix m;
    m.getA()=1; m.getB()=0; m.getC()=0;
    m.getD()=0; m.getE()=1; m.getF()=0;
		svg::SVGTransformable * parentTransformable = dynamic_cast<svg::SVGTransformable *>(e->getParentNode());
		parentTransformable->getConsolidation(&m);
    //std::cerr << m << __FL__;
    svg::SVGMatrix* minv = m.inverse();
    assert(minv);
    //std::cerr << *minv << __FL__;
    invTransform.setMatrix(*minv);
    currentState=moving;
    glutPostRedisplay();
}

typedef bool selectit_func(svg::SVGElement*);

svg::SVGElement *
pick(int x, int y, selectit_func selectit)
{
  svgl::PickManager::PickResult pickResult = pickManager->pick(thesvgdoc, x,y,5,5);
  //pickManager->pick(thesvgdoc, x,y,5,5);
  // we want it front to back (may be default in next versions)
  svgl::PickManager::ReversePickResult hitIterator = pickManager->reversePick();

  //svg::SVGTransformable * transformable = 0;
  svg::SVGElement * result = 0;
  for(; hitIterator.first!=hitIterator.second; ++hitIterator.first) {
    svgl::PickManager::HitIterator::value_type svgElementIterator = *hitIterator.first;
    // get the deepest (int the svg tree) transformable to move it...
    for(; svgElementIterator.first!=svgElementIterator.second; ++svgElementIterator.first) {
      svg::SVGElement * elem = (*svgElementIterator.first);
      //std::cerr << "<" << elem->getDtdElement() << ">";

      //bool p = pickable[elem];
      bool p = selectit(elem);
      if(p) {
				result = elem;
      }

    }
    //std::cerr << std::endl;
    // stop processing remaining hits

    if(result)
      break;
  }
  return result;
}

bool
selectit_transformable(svg::SVGElement *e)
{
  if(dynamic_cast<svg::SVGTransformable * >(e))
    return true;
  else
    return false;
}


void
moveShape(int x, int y)
{
  int dx = x-xlast;
  int dy = y-ylast;
  //double newx = translate->tx + dx/zoom;
  //double newy = translate->ty + dy/zoom;
  double invdx, invdy;
  invTransform.apply(dx,dy,&invdx,&invdy);
  //std::cerr << invTransform.getMatrix();
  //std::cerr << DBGVAR(dx) << DBGVAR(dy) << DBGVAR(invdx) << DBGVAR(invdy) << __FL__;
  translate->tx += invdx;
  translate->ty += invdy;
  
#if 0
  translate->tx += dx/zoom;
  translate->ty += dy/zoom;
#endif
  
  glutPostRedisplay();
}

void
motion(int x, int y)
{
  switch(currentState) {
#if 0
  case panning:
    panAndZoom::motion(x,y);
    break;
  case zooming:
    panAndZoom::motion(x,y);
    break;
#endif
  case moving:
    moveShape(x,y);
    break;
		default:
			panAndZoom::motion(x,y);
			break;
  }
	glutPostRedisplay();
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
      if(modifiers & GLUT_ACTIVE_SHIFT) {
				panAndZoom::button(num,state,x,y);
				glutSetCursor(GLUT_CURSOR_UP_DOWN);
				currentState=zooming;
      }
      else {
				svg::SVGElement * picked = pick(x,y,selectit_transformable);
				if(picked) {
					picked->raise();
					setupTranslation(picked);
				}
				else {
					panAndZoom::button(num,state,x,y);
					glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
					currentState=panning;
				}
      }
    }
    break;
  case GLUT_UP:
    if(currentState!=idle) {
      currentState = idle;
      glutSetCursor(GLUT_CURSOR_INHERIT);
    }
    break;
  }
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

int
main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_STENCIL);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keypress);
  glutMotionFunc(motion);
  glutMouseFunc(button);

  try {
    svgl::InitHelper * initHelper = svgl::InitHelper::get();
    displayManager = initHelper->displayManager;
    pickManager = initHelper->pickManager;

    thesvgdoc = createSampleDoc();
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
