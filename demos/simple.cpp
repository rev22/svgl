#include <glft/FontManager.hpp>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>
#include <iostream>

glft::FontManager *fontManager;

void
reshape(int w, int h)
{
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-w/2,w/2,-h/2.,h/2.,-1,1);
  glMatrixMode(GL_MODELVIEW);
}

void
display()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  std::string message ;

  message = "texture" ;
  int x,y,w,h;
  fontManager->getBoundingBox(&message, &x,&y,&w,&h);
  glTranslated(-w, 0,0);
  glColor3f(1,0,0) ;
  glEnable(GL_TEXTURE_2D);
  fontManager->setFontType(glft::FontManager::texture);    
  fontManager->renderAndAdvance(&message);

  message = "vector" ;
  //message = glft_printable;
  //message = "b";
  glColor3f(1,1,0.8) ;
  glDisable(GL_TEXTURE_2D);
  fontManager->setFontType(glft::FontManager::vector);
  fontManager->renderAndAdvance(&message);

  glutSwapBuffers();
}

int
main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);

  fontManager = glft::FontManager::getDefaultManager() ;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glPolygonMode(GL_FRONT, GL_LINE);

  glft::FontInfo info;
  info.family = "sans-serif";
	info.style = glft::FontInfo::fs_regular;
	info.weight = glft::FontInfo::fw_normal;
  fontManager->setCurrentFont(info, 40);

  glutMainLoop();
}
