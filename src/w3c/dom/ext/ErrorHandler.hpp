#ifndef _dom_ext_ErrorHandler_hpp_
#define _dom_ext_ErrorHandler_hpp_

#if SVGL_USE_GC
#include <utils/gc_alloc.hpp>
#endif
#include <w3c/dom/DOMException.hpp>

namespace dom {

class ErrorHandler
#if SVGL_USE_GC
: virtual public gc
#endif
{
public:
  ErrorHandler();
  virtual ~ErrorHandler();

  static ErrorHandler * instance();

  virtual void warning(const DOMException& exception);
  virtual void error(const DOMException& exception);
  virtual void fatalError(const DOMException& exception);
protected:
  static ErrorHandler * instance_;
};

}


#endif
