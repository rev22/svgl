#ifndef _utils_StringInputSource_hpp_
#define _utils_StringInputSource_hpp_

#include <utils/InputSource.hpp>
#include <utils/String.hpp>

namespace sdom {

class StringInputSource : public InputSource
{
protected:
  String * _content;
  String::size_type _count;
public:
  StringInputSource(String* content);
  virtual ~StringInputSource();

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
