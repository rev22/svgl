#ifndef _utils_InputSource_hpp_
#define _utils_InputSource_hpp_

//#include <utils/String.hpp>
#if SVGL_USE_GC
#include <gc_cpp.h>
#endif

namespace unicode {
  class String;
}

namespace sdom {

using unicode::String;

class InputSource
#if SVGL_USE_GC
: virtual public gc_cleanup
#endif
{

public:
  virtual ~InputSource() {}
  virtual int readBytes(char * bytes, int nBytes) = 0;
  virtual bool finished() const = 0;
  virtual String * getURI() const = 0;
  virtual void setURI(String * uri) = 0;
  virtual String * getEncoding() const = 0;
  virtual void setEncoding(String * encoding) = 0;
  virtual String * getTitle() const = 0;
  virtual void setTitle(String * title) = 0;
};

}

#endif
