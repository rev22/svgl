#ifndef __svgl_FilterContext__
#define __svgl_FilterContext__

#include <utils/String.hpp>
#include <map>

#if SVGL_USE_GC
#include <utils/gc_alloc.hpp>
#endif
//#include <svgl/TextureSnapshot.hpp>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace svgl {

  class TextureSnapshot;

#if SVGL_USE_GC
  typedef std::map<unicode::String*, svgl::TextureSnapshot*, string_less, ::gc_alloc<std::pair<const unicode::String*, svgl::TextureSnapshot*> > > FilterResults;
#else
  typedef std::map<unicode::String*, svgl::TextureSnapshot*, string_less> FilterResults;
#endif

  class FilterContext {
  public:

    void
    glRect() const {
      glRectf(x,y,x+width,y+height);
    }

    FilterResults results;
    unicode::String* lastResult;
    int x,y,width,height;
    int dx,dy;
  };

}

#endif // __svgl_FilterContext__
