#ifndef __glPolyIncluded__
#define __glPolyIncluded__

#include <vector>
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

struct glPoly {
  typedef std::vector<GLfloat> point_list_type;

  struct primitive_info {
    GLenum draw_primitive;
    //GLint numBuffer;
    point_list_type points;
  };

  typedef std::vector<primitive_info> contour_type;
  contour_type contours;

  void reset() { /*points.clear(); */ contours.clear(); } //FIXME delete buffers
  //void updateGlBuffers();

  void setTessCallbacks(GLUtesselator*);

};

extern int combineTab_index;

void
draw(const glPoly& poly);


#endif // __glPolyIncluded__
