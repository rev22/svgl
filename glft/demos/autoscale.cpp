#include <math.h>

#include <glft/FontManager.hpp>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glut.h>
#include <GL/glext.h>
#endif

#include <string>
#include <iostream>

glft::FontManager * fontManager;
glft::FontInfo info;
int fontSize;
int maxTexturedFontSize;
bool doAA;

double xpos, ypos; // initial text position
int xlast, ylast; // last mouse position
int xclick, yclick; // last click position (window)
double xclickpan, yclickpan; // last click position (world)

enum state_t {
  idle,
  panning,
  zooming
};

state_t currentstate;

struct GLInfo {
  GLInfo() : scale(1), xpan(0), ypan(0) {}
  double scale, xpan, ypan;
  int winWidth, winHeight;

  void setZoomAndPan();
};

GLInfo glinfo;

void
GLInfo::setZoomAndPan()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,winWidth, 0, winHeight, -1,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glScalef(scale, scale, 1);
  glTranslatef(-xpan, -ypan, 0);
}

void
motion(int x, int y)
{
  int dx = x-xlast;
  int dy = y-ylast;

  switch(currentstate) {
  case panning:
    glinfo.xpan -= dx/glinfo.scale;
    glinfo.ypan += dy/glinfo.scale;
    glutPostRedisplay();
    break;
  case zooming: {
	// function dy:s : -100:2 0:1 100:0.5 200:0.25 ...
	// ensure that when going back to the init point, the scale is the same...
	// should be -dy but y is reversed...
	  double sscale = exp2(dy/100.);
	  //double sscale = exp(log(2)*dy/100.);
	double newscale = glinfo.scale*sscale;
	double dscale = newscale-glinfo.scale;
	double f = dscale/(glinfo.scale+dscale);

    glinfo.xpan += (xclickpan - glinfo.xpan) * ( f );
    glinfo.ypan += (yclickpan - glinfo.ypan) * ( f );

    glinfo.scale += dscale;

    glutPostRedisplay();
  }
    break;
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
  switch (state) {
  case GLUT_DOWN:
    switch(num) {
    case GLUT_LEFT_BUTTON:
      xlast=x;
      ylast=y;
      glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
      currentstate=panning;
      break;
    case GLUT_MIDDLE_BUTTON:
      xclick = x;
      yclick = y;

      xclickpan = (xclick)/glinfo.scale + glinfo.xpan;
      yclickpan = (glinfo.winHeight-yclick)/glinfo.scale + glinfo.ypan;
	  //yclickpan = (yclick)/glinfo.scale + glinfo.ypan;

      xlast=x;
      ylast=y;
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
special(int key, int x, int y)
{
  switch(key) {
  case GLUT_KEY_UP: break;
  case GLUT_KEY_DOWN: break;
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
  case 'r':
    glinfo.xpan=glinfo.ypan=0;
    glinfo.scale=1;
    glutPostRedisplay();
    break;
  case 'a':
    doAA = !doAA;
    glutPostRedisplay();
  }
}


void
reshape(int w, int h)
{
  static bool first = true;
  if(first) {
    xpos = w/2.;
    ypos = h/2.;
    first=false;
  }
  glViewport(0,0,w,h);
  glinfo.winWidth = w;
  glinfo.winHeight = h;
  glinfo.setZoomAndPan();
}

void
display()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glinfo.setZoomAndPan();

  std::string message ;

#if 0
  // computation unnecessary, example to change the message according to the type
  int localFontSize = fontSize;
  localFontSize = (int) roundf(localFontSize * glinfo.scale);
  if(localFontSize > maxTexturedFontSize) {
    message = "text drawn in vector mode" ;
    //message = "vector" ;
    if(doAA)
      glEnable(GL_MULTISAMPLE_ARB);
    else
      glDisable(GL_MULTISAMPLE_ARB);
  } else {
    message = "text drawn in texture mode" ;
    glDisable(GL_MULTISAMPLE_ARB);
  }
#else

  message = "text autoscale";
	//message = "Grpadient on fill";
#endif

  int x,y,w,h;

  glPushMatrix();

  //fontManager->getBoundingBox(&message, &x,&y,&w,&h);
  fontManager->getBoundingBoxAutoScale(&message, &x,&y,&w,&h);
  glTranslated(xpos,ypos,0);
  glTranslated(-w/2, h/2,0);
	//glRotated(90,0,0,1);
  fontManager->renderAutoScale(&message);
  //fontManager->renderWithScale(&message, glinfo.scale);
	//glEnable(GL_TEXTURE_2D);
  //fontManager->renderAndAdvance(&message);
  //std::cerr << "x:" << x << " y:" << y << " w:" << w << " h:" << h << std::endl;
#if 0
	glColor4d(1,0,0,1);
  glDisable(GL_TEXTURE_2D);
  glBegin(GL_LINE_LOOP);
  glVertex2f(x,y);
  glVertex2f(x+w,y);
  glVertex2f(x+w,y+h);
  glVertex2f(x,y+h);
  glEnd();
	glColor4d(1,1,1,1);
#endif
  glPopMatrix();

  glPushMatrix();
  std::string s = "(0,0)";
  fontManager->getBoundingBox(&s, &x,&y,&w,&h);
  glTranslated(-w/2., h/2.,0);
  //fontManager->renderAutoScale(&s);
  //fontManager->renderWithScale(&s, glinfo.scale);
	//fontManager->render(&s);
  glPopMatrix();

  glutSwapBuffers();
}

int
main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keypress);
  glutMotionFunc(motion);
  glutMouseFunc(button);
  glutSpecialFunc(special);

  fontManager = glft::FontManager::getDefaultManager() ;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_MULTISAMPLE_ARB);
  //glPolygonMode(GL_FRONT, GL_LINE);

  info.family = "helvetica";
	info.style = glft::FontInfo::fs_normal;
	info.weight = glft::FontInfo::fw_normal;
  maxTexturedFontSize = fontManager->getMaxTexturedFontSize();
  fontSize = 20;
  fontManager->setCurrentFont(info, fontSize);

  std::cerr << glGetString(GL_RENDERER) << std::endl;
  std::cerr << "r:reset a:antialiasing q,ESC:quit" << std::endl;

  glinfo = GLInfo();
  doAA = true;
  //doAA = false;

  glutMainLoop();
}
