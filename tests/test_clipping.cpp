#if defined (__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <svgl/debug.hpp>
#include <svgl/GLInfo.hpp>

svgl::GLInfo * glinfo;

void
reshape(int w, int h)
{
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	glinfo->setZoomAndPan();
	glViewport(0,0,w,h);
}

void
display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glLoadIdentity();
#if 0
	glColor4d(1,0,0,1);
	glinfo->stencilPush();
	glRectf(30,30,100,100);
	glinfo->stencilEnableTop();
	glRectf(30,30,100,100);
	glColor4d(0,0,1,1);
	glRectf(60,60,130,130);
	glinfo->stencilPop();
	glRectf(30,30,100,100);
#else
	glColor4d(1,0,0,1);
	glinfo->stencilEnableTopAndPush();
	glRectf(30,30,100,100);
	glinfo->stencilEnableTop();
	//glRectf(30,30,100,100);
	glColor4d(0,0,1,1);
	glRectf(60,60,130,130);
	glinfo->stencilPop();
	glRectf(30,30,100,100);
#endif
	
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
  case 'q':
    glutDestroyWindow(glutGetWindow());
    exit(0);
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
  //glutMotionFunc(panAndZoom::motion);
  //glutMouseFunc(panAndZoom::button);
	
	glClearColor(1,1,1,1);
	glColor4d(1,0,0,1);
	glinfo = new svgl::GLInfo();
	glinfo->glInit();
	glutMainLoop();
  return 0;
}
