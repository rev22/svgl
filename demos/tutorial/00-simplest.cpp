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

#include <svgl/debug.hpp>

svg::SVGDocument * thesvgdoc=0;
svgl::DisplayManager * displayManager=0;

svg::SVGDocument*
createSampleDoc()
{
  svg::SVGDocument * doc = new svg::SVGDocument();
  svg::SVGSVGElement * thesvgelt = new svg::SVGSVGElement(doc);
  thesvgelt->setWidth(450);
  thesvgelt->setHeight(450);

  doc->appendChild(thesvgelt);

  svg::SVGRectElement * rect = new svg::SVGRectElement(doc);

  double w=100,h=100;
  double x = 100;
  double y = 100;

  rect->setX(-w/2);
  rect->setY(-h/2);
  rect->setWidth(w);
  rect->setHeight(h);
  //rect->setFill(0,0,1);
	rect->setFill("blue");
  rect->setStroke(0,0,0);
  rect->setStrokeWidth(2);
	rect->setOpacity(.75);
      
  // order independant
  //rect->scale(.5);
  //rect->rotate(20);
  rect->translate(x,y);

  thesvgelt->appendChild(rect);
	
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

int
main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_STENCIL);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keypress);

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
