#if defined (__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <svgl/InitHelper.hpp>
#include <svgl/DisplayManager.hpp>
#include <svgl/ExternalEntityManager.hpp>

#include <w3c/svg/SVGDocument.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/svg/SVGGElement.hpp>
#include <w3c/svg/SVGUseElement.hpp>
#include <w3c/svg/SVGTransforms.hpp>


#include <svgl/debug.hpp>
#include "../../tests/panAndZoom.hpp"


svg::SVGDocument * thesvgdoc=0;
svgl::DisplayManager * displayManager=0;
svgl::ExternalEntityManager * externalEntityManager=0;

svg::SVGDocument*
createSampleDoc(const char * useHref)
{
  svg::SVGDocument * doc = new svg::SVGDocument();
  svg::SVGSVGElement * thesvgelt = new svg::SVGSVGElement(doc);
  thesvgelt->setWidth(450);
  thesvgelt->setHeight(450);

  doc->appendChild(thesvgelt);
	
	svg::SVGGElement * view = panAndZoom::setup(doc);
	thesvgelt->appendChild(view);
  
  double w=100,h=100;

  svg::SVGUseElement * use1 = new svg::SVGUseElement(doc);
  use1->setWidth(w);
  use1->setHeight(h);      
  use1->translate(100,100);
  use1->setHref(useHref);
  view->appendChild(use1);

	dom::Element* de = externalEntityManager->get(unicode::String::createString(useHref), thesvgdoc);
	//std::cerr << this << " " << de << __FL__;
	//SVGSymbolElement *svgsym = dynamic_cast<SVGSymbolElement*>(de);
	//SVGSVGElement *svgsvg = dynamic_cast<SVGSVGElement*>(de);
	svg::SVGElement* svge = dynamic_cast<svg::SVGElement*>(de);
	if(svge)
		svge->compileAsATexture(true);

  svg::SVGUseElement * use2 = new svg::SVGUseElement(doc);
  use2->setWidth(w);
  use2->setHeight(h);
  use2->setHref(useHref);  
  use2->scale(2);
  use2->translate(230,300);
  //view->appendChild(use2);

  //use2->compile(true);

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
	glutMouseFunc(panAndZoom::button);
	glutMotionFunc(panAndZoom::motion);

  try {
    svgl::InitHelper * initHelper = svgl::InitHelper::get();
    displayManager = initHelper->displayManager;
		externalEntityManager = initHelper->externalEntityManager;

    const char * href="";
    if(argc==2) {
      href=argv[1];
    }

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
