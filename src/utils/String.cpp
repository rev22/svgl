#include <cstring>
#include <set>

#include <w3c/dom/DOMException.hpp>
#include <utils/String.hpp>
#include <utils/StringImpl.hpp>
#include <utils/StringBuffer.hpp>

#if SVGL_USE_GC
#include <utils/gc_alloc.hpp>
#endif

namespace unicode {

String::String() : size(0), debug_str(0) {}

String::~String() {}

String::size_type
String::getLength() const throw()
{
  return size; 
}

String * 
String::createString(const char * str, size_type size)
{
  if (str == 0)
    return 0;
  return new StringLat1(str, size);
}

String * 
String::createStringLat1(const lat1_char_type * str, size_type size)
{
  if (str == 0)
    return 0;
  return new StringLat1(str, size);
}

String *
String::createStringUtf8(const utf8_char_type * str, size_type size)
{
  if (str == 0)
    return 0;
  return new StringUtf8(str, size);
}

String *
String::createStringUtf16(const utf16_char_type * str, size_type size)
{
  if (str == 0)
    return 0;
  return new StringUtf16(str, size);
}

String *
String::createStringUtf32(const utf32_char_type * str, size_type size)
{
  if (str == 0)
    return 0;
  return new StringUtf32(str, size);
}

bool String::equals(const String& other) const throw()
{
  if (this == &other) {
    return true;
  }
  if (getLength() != other.getLength()) {
    return false;
  }
  for (size_type i = 0; i < getLength(); i++) {
    if (charAt(i) != other.charAt(i)) {
      return false;
    }
  }
  return true;
}

bool
String::equals(const char * str) const throw()
{
  if (str == 0)
    return false;
  size_type i;
  for (i = 0; i < getLength() && str[i] != 0; i++) {
    if (charAt(i) != char_type(str[i]))
      return false;
  }
  if (i == getLength() && str[i] == 0)
    return true;
  return false;
}

int
String::compare(const String& other) const throw()
{
  if (this == &other)
    return 0;
  int len1 = getLength();
  int len2 = other.getLength();
  int i;
  for (i = 0; i < len1 && i < len2; i++) {
    char_type c1 = charAt(i);
    char_type c2 = other.charAt(i);
    if (c1 < c2)
      return -1;
    else if (c1 > c2)
      return 1;
  }
  if (i == len1 && i == len2)
    return 0;
  else if (i == len1)
    return -1;
  else
    return 1;
}

char *
String::transcode() const
{
  size_type len = getLength();
#if SVGL_USE_GC
  char * buffer = new (UseGC) char[len+1];
#else
  char * buffer = new char[len+1];
#endif
  return transcode(buffer, len+1);
}

char *
String::transcode(char * buffer, unsigned int size) const
{
  size_type len = getLength();
  if (len == 0) {
    if (buffer != null)
      buffer[0] = 0;
    return buffer;
  }
  if (len >= size)
    len = size-1;
  if (buffer == null) {
#if SVGL_USE_GC
    buffer = new (UseGC) char[len+1];
#else
    buffer = new char[len+1];
#endif
  }

  size_type i;
  for (i = 0; i < len; i++) {
    buffer[i] = char(charAt(i));
  }
  buffer[len] = 0;
  return buffer;
}

String * 
String::substring(size_type from) const
  throw(std::out_of_range)
{
  size_type len = getLength();
  if (from > len)
    throw std::out_of_range("String::substring");

  else if (from == len) {
    return 0;
  }
  return 0;
}

String *
String::substring(size_type from, size_type to) const
  throw(std::out_of_range)
{
  if (from > to ||
      to > getLength()) {
    throw std::out_of_range("String::substring");
  }
  else if (from == to) {
    return 0;
  }
  return 0;
}

String::size_type
String::indexOf(char_type c, size_type from) const
{
  size_type len = getLength();
  for (size_type i = from; i < len; ++i)
    if (charAt(i) == c)
      return i;
  return static_cast<size_type>(-1);
}

bool
String::startsWith(String * prefix, size_type offset) const
{
  if (prefix == null)
    return true;
  size_type len = getLength();
  size_type len2 = prefix->getLength();
  if ((offset + len2) > len)
    return false;
  for (unsigned int i = 0; i < len2; i++) {
    if (charAt(i+offset) != prefix->charAt(i))
      return false;
  }
  return true;
}

unsigned int
String::hash() const
{
  unsigned long h = 0;
  size_type len = getLength();
  for (size_type i = 0; i < len; ++i)
    h = 5*h + charAt(i);
  return size_type(h);
}

static const char utf8_length[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};

int
String::len_utf8(const String::utf8_char_type * str)
{
  int i = *((unsigned char*)str);
  int l = utf8_length[i];
  if (l != 0)
	return l;
  return 0;
}

String::char_type
String::next_utf8(const String::utf8_char_type*& str)
{
  int i = *((unsigned char*)str);
  int len = utf8_length[i];
  char_type c = 0;

  switch(len) {    
  case 5: c += *str++; c <<= 6;
  case 4: c += *str++; c <<= 6;
  case 3: c += *str++; c <<= 6;
  case 2: c += *str++; c <<= 6;
  case 1: c += *str++; c <<= 6;
  case 0: c += *str++; 
    break;
  default:
    throw std::runtime_error("Invalid UTF-8 sequence");
  }
  return c;
}

const String::utf8_char_type *
String::nth_utf8(const String::utf8_char_type * str, String::size_type n)
{
  while (n-- != 0) {
    str += len_utf8(str) + 1;
  }
  return str;
}

const String::utf16_char_type * 
String::nth_utf16(const String::utf16_char_type * str, String::size_type n)
{
  return str + n;
}

#if SVGL_USE_GC
typedef std::set<String*,string_less, gc_alloc<String*> > StringSet;
#else
typedef std::set<String*, string_less> StringSet;
#endif

StringSet uniq;

static inline StringSet&
uniq_strings()
{
  return uniq;
}

String *
String::internString(String * str)
{
  if (str == null || str->getLength() == 0)
    return null;
  StringSet::iterator i = uniq_strings().find(str);
  if (i != uniq_strings().end())
    return *i;
  uniq_strings().insert(str);
  return str;
}

String *
String::internString(const char * str, size_type size)
{
  if (str == 0 || size == 0)
    return 0;
  String * s = createString(str, size);
  StringSet::iterator i = uniq_strings().find(s);
  if (i != uniq_strings().end()) {
    //    delete s;
    return *i;
  }
  uniq_strings().insert(s);
  return s;
}

String *
String::internStringLat1(const lat1_char_type * str, size_type size)
{
  if (str == 0 || size == 0)
    return 0;
  String * s = createString(str, size);
  StringSet::iterator i = uniq_strings().find(s);
  if (i != uniq_strings().end()) {
    //    delete s;
    return *i;
  }
  uniq_strings().insert(s);
  return s;
}

String *
String::internStringUtf8(const utf8_char_type * str, size_type size)
{
  if (str == 0 || size == 0)
    return 0;
  String * s = createStringUtf8(str, size);
  StringSet::iterator i = uniq_strings().find(s);
  if (i != uniq_strings().end()) {
    //    delete s;
    return *i;
  }
  uniq_strings().insert(s);
  return s;
}

String *
String::internStringUtf16(const utf16_char_type * str,  size_type size)
{
  if (str == 0 || size == 0)
    return 0;
  String * s = createStringUtf16(str, size);
  StringSet::iterator i = uniq_strings().find(s);
  if (i != uniq_strings().end()) {
    //    delete s;
    return *i;
  }
  uniq_strings().insert(s);
  return s;
}

String *
String::internStringUtf32(const utf32_char_type * str, size_type size)
{
  if (str == 0 || size == 0)
    return 0;
  String * s = createStringUtf32(str, size);
  StringSet::iterator i = uniq_strings().find(s);
  if (i != uniq_strings().end()) {
    //    delete s;
    return *i;
  }
  uniq_strings().insert(s);
  return s;
}

} // namespace unicode

// removed because of a g++-3.0 bug:
// anonymous namespace is different from global namespace
//namespace {

  using unicode::String;

void
print_xmlchar(std::ostream& out, String::char_type c)
{
  unsigned char lo = c&0xFF;
  unsigned char hi = c >> 8;
  if (c < 0x80) {
    out << lo;
  }
  else if (c < 0x700) {
    out << char((lo >> 6) | (hi << 2) | 0xc0)
	<< char((lo & 0x3f) | 0x80);
  }
  else if (c < 0xdb) {
    // +++ FIXME, unhandled UTF16
  }
  else {
    out << char((hi >> 4) | 0xe0)
	<< char(((hi & 0xf) << 2) | (lo >> 6) | 0x80)
	<< char((lo & 0x3f) | 0x80);
  }
}

void
print(std::ostream& out, const String& str)
{
  for (String::size_type i = 0; i != str.getLength(); i++) {
    print_xmlchar(out,str[i]);
  }
}

void
print_xmlchar_quoted(std::ostream& out, String::char_type c)
{
  switch(c) {
  case '<':
    out << "&lt;";
    break;
  case '>':
    out << "&gt;";
    break;
  case '&':
    out << "&amp;";
    break;
  case '\'':
    out << "&apos;";
    break;
  case '"':
    out << "&quot;";
    break;
  default:
    print_xmlchar(out, c);
  }
}

void
print_quoted(std::ostream& out, const String& str)
{
 
  for (String::size_type i =0; i != str.getLength(); i++) {
    print_xmlchar_quoted(out,str[i]);
  }
}

String *
concat (String * s1, String * s2)
{
  if (s1 == null || s1->getLength() == 0)
    return s2;
  if (s2 == null || s2->getLength() == 0)
    return s1;
  unicode::StringBuffer buf(s1);
  buf.append(s2);
  return buf.createString();
}

//} // namespace



