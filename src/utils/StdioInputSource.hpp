#ifndef _utils_StdioInputSource_hpp_
#define _utils_StdioInputSource_hpp_

#include <stdio.h>

//#include <utils/gc_alloc.hpp>
#include <utils/InputSource.hpp>

namespace sdom {

class StdioInputSource : public InputSource
{
protected:
  FILE * in;
  String * URI;
  bool close;
public:
  StdioInputSource(FILE * i, String * uri, bool close = true);
  virtual ~StdioInputSource();

  int readBytes(char * bytes, int nBytes);
  bool finished() const;
  String * getURI() const;
  void setURI(String * uri);
  String * getEncoding() const;
  void setEncoding(String * );
  String * getTitle() const;
  void setTitle(String * title);
};

}

#endif
