#ifndef _utils_String_hpp
#define _utils_String_hpp

#include <iostream>
#include <stdexcept>
#if SVGL_USE_GC
#include <gc_cpp.h>
#endif
#include <functional>
#include <w3c/dom/config.hpp>

#define null 0

namespace unicode {

class String
#if SVGL_USE_GC
: public gc
#endif
{
public:
  enum Type {
    string_lat1,
    string_utf8,
    string_utf16,
    string_utf32,
    string_other
  };
  typedef char byte_type;
  typedef byte_type lat1_char_type;
  typedef byte_type utf8_char_type;
  typedef unsigned short utf16_char_type;
  typedef unsigned int char_type;
  typedef char_type utf32_char_type;
  typedef unsigned int size_type;

#ifdef _MSC_VER
  enum { npos = -1 };
#else
  static const size_type npos = static_cast<size_type>(-1);
#endif

  static String * createString(const char *,
				  size_type size = size_type(-1));
  static String * createStringLat1(const lat1_char_type *,
				   size_type size = size_type(-1));
  static String * createStringUtf8(const utf8_char_type *,
				      size_type size = size_type(-1));
  static String * createStringUtf16(const utf16_char_type *,
				       size_type size = size_type(-1));
  static String * createStringUtf32(const utf32_char_type *,
				       size_type size = size_type(-1));

  static String * internString(String *);
  static String * internString(const char *,
				  size_type size = size_type(-1));
  static String * internStringLat1(const lat1_char_type *,
				  size_type size = size_type(-1));
  static String * internStringUtf8(const utf8_char_type *,
				      size_type size = size_type(-1));
  static String * internStringUtf16(const utf16_char_type *,
				       size_type size = size_type(-1));
  static String * internStringUtf32(const utf32_char_type *,
				       size_type size = size_type(-1));

  virtual size_type getLength() const throw();
  virtual bool equals (const String& other) const throw();
  virtual bool equals (const char * other) const throw();
  //virtual bool equalsIgnoreCase(const String& other) const throw();
  virtual int compare(const String& other) const throw();
  //virtual int compareIgnoreCase(const String& other) const throw();
  virtual char * transcode() const;
  virtual char * transcode(char * buffer, unsigned int size) const;

  virtual char_type charAt(size_type) const
    throw(std::out_of_range) = 0;
  virtual String * substring(size_type from) const
    throw(std::out_of_range) = 0;
  virtual String * substring(size_type from, size_type to) const
    throw(std::out_of_range) = 0;
  virtual size_type indexOf(char_type c, size_type from=0) const;
  virtual bool startsWith(String * prefix, size_type offset) const;
  virtual unsigned int hash() const;

  virtual Type getType() const = 0;
  virtual size_type getSize() const = 0;
  virtual bool hasVariableSize() const = 0;
  virtual const lat1_char_type * as_lat1() const = 0;
  virtual const utf8_char_type * as_utf8() const = 0;
  virtual const utf16_char_type * as_utf16() const = 0;
  virtual const utf32_char_type * as_utf32() const = 0;
  virtual const byte_type * repr() const = 0;

  bool equals (const String * other) const throw() {
    if (other == 0)
      return this == 0 || getLength() == 0;
    return equals(*other);
  }
  //bool equalsIgnoreCase (const String * other) const throw() {
  //  return equalsIgnoreCase(*other);
  //}
  bool operator == (const String& other) const throw() {
    return equals(other);
  }
  bool operator != (const String& other) const throw() {
    return !equals(other);
  }


  char_type operator[](size_type index) const { return charAt(index); }
  bool startsWith(String * prefix) const { return startsWith(prefix, 0); }
  static const utf8_char_type * nth_utf8(const utf8_char_type *, size_type n);
  static int len_utf8(const utf8_char_type *);
  static char_type next_utf8(const utf8_char_type*&);
  static const utf16_char_type * nth_utf16(const utf16_char_type *, size_type n);

protected:
  String();
  virtual ~String();
  size_type size;
  const char * debug_str;
};

} // namespace unicode



struct string_less : public std::binary_function<unicode::String*,unicode::String*,bool> {
  bool operator()(const unicode::String * x, const unicode::String* y) const {
    if (x == y) return false;
    if (x == 0) return true;
    if (y == 0) return false;
    return x->compare(*y) < 0;
  }
};

struct string_equal : public std::binary_function<unicode::String*,unicode::String*,bool> {
  bool operator()(const unicode::String * x, const unicode::String* y) const {
    if (x == y) return true;
    if (x == 0 ||
	y == 0) return false;
    return x->equals(*y);
  }
};

unicode::String *
concat(unicode::String * s1, unicode::String * s2);

void
print(std::ostream& out, const unicode::String& str);
void
print_xmlchar(std::ostream& out, unicode::String::char_type c);
void
print_xmlchar_quoted(std::ostream& out, unicode::String::char_type c);
void
print_quoted(std::ostream& out, const unicode::String& str);

inline
void
print_quoted(std::ostream& out, const unicode::String * str)
{
  if (str != 0)
    print_quoted(out,*str);
}



inline
std::ostream&
operator << (std::ostream& out, const unicode::String& str)
{
  print(out,str);
  return out;
}

inline 
std::ostream&
operator << (std::ostream& out, const unicode::String * str)
{
  if (str != 0)
    out << *str;
  return out;
}


#endif
