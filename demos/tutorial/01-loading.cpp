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

#include <svgl/debug.hpp>

svg::SVGDocument * thesvgdoc = 0;
svgl::DisplayManager * displayManager = 0;

svg::SVGDocument *
loadDoc(const char* filename)
{
  svgl::Parser parser;
  svg::SVGDocument * doc=0;
  try {
    doc = parser.parse(filename);
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
  glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_STENCIL);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keypress);

  try {
    svgl::InitHelper * initHelper = svgl::InitHelper::get();
    displayManager = initHelper->displayManager;

    const char * filename = "";
    if(argc==2) {
      filename = argv[1];
    }

    thesvgdoc = loadDoc(filename);
    if(thesvgdoc)
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
