#ifndef _utils_StringBuffer_hpp
#define _utils_StringBuffer_hpp

#include <vector>
#include <utils/StringImpl.hpp>

namespace unicode {

class StringBuffer : public std::vector<String::char_type> {
public:
  typedef String::char_type char_type;
  typedef std::vector<char_type> super;
  StringBuffer() {}
  explicit StringBuffer(size_type sz) {
    reserve(sz);
  }
  StringBuffer(const String& s) {
    String::size_type len = s.getLength();
    reserve(len);
    for (unsigned int i = 0; i < len; i++) {
      push_back(s[i]);
    }
  }
  StringBuffer(const String * s) {
    append(s);
  }
  StringBuffer(const char * s) {
    append(s);
  }
  void append(const String * s) {
    if (s != 0) {
      String::size_type len = s->getLength();
      reserve(size()+len);
      for (unsigned int i = 0; i < len; i++) {
	push_back(s->charAt(i));
      }
    }
  }
  void append(const char * s) {
    if (s != 0) {
      while (*s != 0) {
	push_back(*s++);
      }
    }
  }
  void append(char_type c) {
    push_back(c);
  }
  void insert(unsigned int offset, const String * s);
  int computeWidth() const;
  String * createString() const;
  String * internString() const;
  char * transcode(char * buf, unsigned int size) const;

  StringBuffer& operator += (const String * str) {
    append(str);
    return *this;
  }
  StringBuffer& operator += (const char * str) {
    append(str);
    return *this;
  }
  StringBuffer& operator += (char_type c) {
    append(c);
    return *this;
  }
  StringBuffer& operator << (const String * str) {
    append(str);
    return *this;
  }
  StringBuffer& operator << (const char * str) {
    append(str);
    return *this;
  }
  StringBuffer& operator << (char_type c) {
    append(c);
    return *this;
  }
  operator String *() const { return createString(); }
};

} // namepace unicode

#if 0
using unicode::StringBuffer;
#endif

#endif






