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
#include <w3c/svg/SVGGElement.hpp>
#include <w3c/svg/SVGTransforms.hpp>

#include <svgl/debug.hpp>

svg::SVGDocument * thesvgdoc = 0;
svgl::DisplayManager * displayManager = 0;

svg::SVGTranslateTransform * panTransform = 0;
svg::SVGScaleTransform * zoomTransform = 0;

enum state_t {
  idle,
  panning,
  zooming
};

state_t currentState=idle;

int xlast=0;
int ylast=0;
int xclickpan, yclickpan;

float xpan=0, ypan=0, zoom=1;

svg::SVGDocument*
createSampleDoc()
{
  svg::SVGDocument * doc = new svg::SVGDocument();
  svg::SVGSVGElement * thesvgelt = new svg::SVGSVGElement(doc);
  thesvgelt->setWidth(450);
  thesvgelt->setHeight(450);

  doc->appendChild(thesvgelt);

  // add a new "layer"
  // there can be another "layer" for tools, widgets which may not be zoomable...
  svg::SVGGElement * view = new svg::SVGGElement(doc);
  thesvgelt->appendChild(view);
  
  panTransform = new svg::SVGTranslateTransform;
  zoomTransform = new svg::SVGScaleTransform;
  svg::SVGTransformList& l = const_cast<svg::SVGTransformList&>(view->getTransform());
  l.appendItem(zoomTransform);
  l.appendItem(panTransform);

  svg::SVGRectElement * rect = new svg::SVGRectElement(doc);

  double w=100,h=100;
  double x = 100;
  double y = 100;

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
  rect->translate(x,y);

  view->appendChild(rect);

  glutReshapeWindow(450, 450);
  return doc;
}

void
updatePan(int x, int y)
{
  int dx = x-xlast;
  int dy = y-ylast;

  xpan -= dx/zoom;
  ypan -= dy/zoom;

  panTransform->tx = -xpan;
  panTransform->ty = -ypan;

  glutPostRedisplay();
}

void
updateZoom(int x, int y)
{
  int dx = x-xlast;
  int dy = y-ylast;

  double dzoom = -(zoom/2.)*dy/25.;
  double f = dzoom/(zoom+dzoom);

  xpan += (xclickpan - xpan) * f;
  ypan += (yclickpan - ypan) * f;
  zoom += dzoom;
  
  panTransform->tx = -xpan;
  panTransform->ty = -ypan;
  zoomTransform->sx = zoom;
  zoomTransform->sy = zoom;

  glutPostRedisplay();
}

void
motion(int x, int y)
{
  switch(currentState) {
  case panning:
    updatePan(x,y);
    break;
  case zooming:
    updateZoom(x,y);
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
      if(modifiers & GLUT_ACTIVE_SHIFT) {
	xclickpan = (int)(x/zoom + xpan);
	yclickpan = (int)(y/zoom + ypan);
	glutSetCursor(GLUT_CURSOR_UP_DOWN);
	currentState=zooming;
      }
      else {
	glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
	currentState=panning;
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
keypress(unsigned char key, int x, int y)
{
  switch (key) {
  case 27: /*ESC*/
    glutDestroyWindow(glutGetWindow());
    exit(0);
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
