#ifndef __svgl_screenshot__
#define __svgl_screenshot__

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace svgl {

  void
  screenshot(unsigned int w, unsigned int h, const char* path="svgtest.pnm", GLenum screenshotType=GL_RGB);
  
  inline
  void
  screenshot(unsigned int w, unsigned int h, GLenum screenshotType) {
    screenshot(w,h,"sgvtest.pnm", screenshotType);
  }

  inline
  void
  screenshot(const char* path="svgtest.pnm", GLenum screenshotType=GL_RGB) {
    screenshot(450,450,path, screenshotType);
  }


}

#endif // ____svgl_screenshot__
