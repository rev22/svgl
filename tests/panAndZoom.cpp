#if defined (__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <w3c/svg/SVGTransforms.hpp>
#include <w3c/svg/SVGGElement.hpp>
#include <w3c/svg/SVGDocument.hpp>

#include "panAndZoom.hpp"

namespace panAndZoom {

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

svg::SVGGElement*
setup(svg::SVGDocument *doc)
{
		// panAndZoom
	svg::SVGGElement * view = new svg::SVGGElement(doc);
  
	panAndZoom::panTransform = new svg::SVGTranslateTransform;
  panAndZoom::zoomTransform = new svg::SVGScaleTransform;
  svg::SVGTransformList& l = const_cast<svg::SVGTransformList&>(view->getTransform());
  l.appendItem(panAndZoom::zoomTransform);
  l.appendItem(panAndZoom::panTransform);
	return view;
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

	xlast=x;
  ylast=y;
	
  glutPostRedisplay();
}

void
updateZoom(int x, int y)
{
  int dx = x-xlast;
  int dy = y-ylast;

	double szoom = exp2(-dy/100.);
	double newzoom = zoom*szoom;
	double dzoom = newzoom-zoom;
	double f = dzoom/newzoom;
	
  xpan += (xclickpan - xpan) * f;
  ypan += (yclickpan - ypan) * f;
  zoom += dzoom;
  
  panTransform->tx = -xpan;
  panTransform->ty = -ypan;
  zoomTransform->sx = zoom;
  zoomTransform->sy = zoom;

	xlast=x;
  ylast=y;

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

}
