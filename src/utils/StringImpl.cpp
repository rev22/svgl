#include <string>
#include <utils/StringImpl.hpp>

namespace unicode {

template <class T>
static unsigned int
str_len(const T * s)
{
  unsigned int sz = 0;
  while (*s++ != 0)
    sz++;
  return sz;
}

template <class T1, class T2>
void
str_cpy(T1 * to, T2 * from, unsigned int sz)
{
  while (sz-- != 0)
    *to++ = *from++;
}


StringLat1::StringLat1(const lat1_char_type * s, size_type sz)
{
  if (sz == size_type(-1)) {
    sz = strlen(s);
  }
  size = sz;
#if SVGL_USE_GC
  str = new (UseGC) lat1_char_type[sz];
#else
  str = new lat1_char_type[sz];
#endif
  memcpy(str, s, sz);
  debug_str = str;
}

StringLat1::StringLat1(const utf32_char_type * s, size_type sz)
{
  if (sz == size_type(-1)) {
    sz = str_len(s);
  }
  size = sz;
#if SVGL_USE_GC
  str = new (GC) lat1_char_type[sz];
#else
  str = new lat1_char_type[sz];
#endif
  str_cpy(str, s, sz);
  debug_str = str;
}

StringLat1::~StringLat1()
{
  //  delete str;
}

bool
StringLat1::equals(const String& other) const throw()
{
  if (getLength() != other.getLength())
    return false;
  const lat1_char_type * s = other.as_lat1();
  if (s == 0) {
    return String::equals(other);
  }
  else {
    return strncmp(str, s, size) == 0;
  }
}

String::Type
StringLat1::getType() const { return string_lat1; }

String::size_type
StringLat1::getSize() const { return size; }

bool
StringLat1::hasVariableSize() const
{
  return false;
}

String::char_type
StringLat1::charAt(size_type index) const
  throw(std::out_of_range)
{
  if (index > size)
    throw std::out_of_range("String::charAt");
  return str[index];
}

String * 
StringLat1::substring(size_type from) const
  throw(std::out_of_range)
{
  size_type len = getLength();
  if (from > len) {
    throw std::out_of_range("StringLat1::substring");
  }
  else if (from == len) {
    return 0;
  }
  return new StringLat1(str + from, size - from);
}

String *
StringLat1::substring(size_type from, size_type to) const
  throw(std::out_of_range)
{
  if (from > to ||
      to > getLength()) {
    throw std::out_of_range("StringLat1::substring");
  }
  else if (from == to) {
    return 0;
  }
  return new StringLat1(str + from, to - from);
}

const String::lat1_char_type * 
StringLat1::as_lat1() const
{
  return str;
}

const String::utf8_char_type * 
StringLat1::as_utf8() const
{
  return 0;
}

const String::utf16_char_type * 
StringLat1::as_utf16() const
{
  return 0;
}

const String::utf32_char_type * 
StringLat1::as_utf32() const
{
  return 0;
}


StringUtf8::StringUtf8(const utf8_char_type * s, size_type sz)
{
  if (sz == size_type(-1)) {
    sz = strlen(s);
  }
  size = sz;
#if SVGL_USE_GC
  str = new (GC) utf8_char_type[size];
#else
  str = new utf8_char_type[size];
#endif
  memcpy(str, s, size);
  length = size;
  size_type i;
  for (i = 0; i < size; i++) {
    int len = len_utf8(str+i);
    if (len != 0) {
      length -= len;
      i += len;
    }
  }
  debug_str = str;
}

StringUtf8::~StringUtf8()
{
  //  delete str;
}

bool
StringUtf8::equals(const String& other) const throw()
{
  if (getLength() != other.getLength())
    return false;
  const utf8_char_type * s = other.as_utf8();
  if (s == 0) {
    return String::equals(other);
  }
  else {
    return strncmp(str, s, size) == 0;
  }
}

String::size_type
StringUtf8::getLength() const throw()
{
  return length;
}

String::char_type
StringUtf8::charAt(size_type index) const
  throw(std::out_of_range)
{
  if (index > length)
    throw std::out_of_range("StringUtf8::charAt");
  if (size == length)
    return str[index];
  const utf8_char_type *s = nth_utf8(str, index);
  char_type c = *s;
  if ((c&0x80) != 0) {
    if ((c&0xf0) == 0xf0) {
      // ignore ++ FIXME
    }
    else if ((c&0xe0) == 0xe0) {
      c = ((c&0xf) << 12) | ((s[1]&0x3f) << 6) | (s[2] & 0x3f);
    }
    else /* if ((c&0xc0) == 0xc0) */ {
      c = (c & 0x1f) << 6 | s[1] & 0x3f;
    }
  }
  return c;
}

String * 
StringUtf8::substring(size_type from) const
  throw(std::out_of_range)
{
  size_type len = getLength();
  if (from > len) {
    throw std::out_of_range("StringUtf8::substring");
  }
  else if (from == len) {
    return 0;
  }
  if (hasVariableSize()) {
    const utf8_char_type * s = nth_utf8(str, from);
    return new StringUtf8(s, size - (s - str));
  }
  else
    return new StringUtf8(str + from, size - from);
}

String *
StringUtf8::substring(size_type from, size_type to) const
  throw(std::out_of_range)
{
  if (from > to ||
      to > getLength()) {
    throw std::out_of_range("StringUtf8::substring");
  }
  else if (from == to) {
    return 0;
  }
  if (hasVariableSize()) {
    const utf8_char_type * s = nth_utf8(str, from);
    const utf8_char_type * t = nth_utf8(s, to - from);
    return new StringUtf8(s, t - s);
  }
  else
    return new StringUtf8(str + from, to - from);
}

String::Type
StringUtf8::getType() const { return string_lat1; }

String::size_type
StringUtf8::getSize() const { return size; }

bool
StringUtf8::hasVariableSize() const
{
  return size != length;
}

const String::lat1_char_type * 
StringUtf8::as_lat1() const
{
  return 0;
}

const String::utf8_char_type * 
StringUtf8::as_utf8() const
{
  return str;
}

const String::utf16_char_type * 
StringUtf8::as_utf16() const
{
  return 0;
}

const String::utf32_char_type * 
StringUtf8::as_utf32() const
{
  return 0;
}

StringUtf16::StringUtf16(const lat1_char_type * s, size_type sz)
{
  if (sz == size_type(-1)) {
    sz = strlen(s);
  }
  size = sz;
#if SVGL_USE_GC
  str = new (GC) utf16_char_type[size];
#else
  str = new utf16_char_type[size];
#endif
  str_cpy(str, s, size);
  length = size;
}

StringUtf16::StringUtf16(const utf16_char_type * s, size_type sz)
{
  if (sz == size_type(-1)) {
    for (sz = 0; s[sz] != 0; sz++)
      ;
  }
  size = sz;
#if SVGL_USE_GC
  str = new (GC) utf16_char_type[size];
#else
  str = new utf16_char_type[size];
#endif
  memcpy(str, s, size * sizeof(utf16_char_type));
  length = size;
  size_type i;
  for (i = 0; i < size; i++) {
    char_type c = str[i];
    switch((c>>8)&0xFF) {
    case 0xD8: case 0xD9: case 0xDA: case 0xDB: \
      length -= 1;
    }
  }
}

StringUtf16::StringUtf16(const utf32_char_type * s, size_type sz)
{
  if (sz == size_type(-1)) {
    sz = str_len(s);
  }
  size = sz;
#if SVGL_USE_GC
  str = new (GC) utf16_char_type[size];
#else
  str = new utf16_char_type[size];
#endif
  str_cpy(str, s, size);
  length = size;
}

StringUtf16::~StringUtf16()
{
  //  delete str;
}
  

bool
StringUtf16::equals(const String& other) const throw()
{
  if (getLength() != other.getLength())
    return false;
  const utf16_char_type * s = other.as_utf16();
  if (s == 0) {
    return String::equals(other);
  }
  else {
    return memcmp(str, s, size * sizeof(utf16_char_type)) != 0;
  }
}

String::size_type
StringUtf16::getLength() const throw ()
{
  return length;
}

String::char_type
StringUtf16::charAt(size_type index) const
  throw(std::out_of_range)
{
  if (index > length)
    throw std::out_of_range("StringUtf16::charAt");
  if (size == index)
    return str[index];
  return *nth_utf16(str, index);
}

String * 
StringUtf16::substring(size_type from) const
  throw(std::out_of_range)
{
  size_type len = getLength();
  if (from > len) {
    throw std::out_of_range("StringUtf16::substring");
  }
  else if (from == len) {
    return 0;
  }
  if (hasVariableSize()) {
    const utf16_char_type * s = nth_utf16(str, from);
    return new StringUtf16(s, size - (s - str));
  }
  else
    return new StringUtf16(str + from, size - from);
}

String *
StringUtf16::substring(size_type from, size_type to) const
  throw(std::out_of_range)
{
  if (from > to ||
      to > getLength()) {
    throw std::out_of_range("StringUtf16::substring");
  }
  else if (from == to) {
    return 0;
  }
  if (hasVariableSize()) {
    const utf16_char_type * s = nth_utf16(str, from);
    const utf16_char_type * t = nth_utf16(s, to - from);
    return new StringUtf16(s, t - s);
  }
  else
    return new StringUtf16(str + from, to - from);
}

String::Type
StringUtf16::getType() const { return string_lat1; }

String::size_type
StringUtf16::getSize() const { return size; }

bool
StringUtf16::hasVariableSize() const
{
  return size != length;
}

const String::lat1_char_type * 
StringUtf16::as_lat1() const
{
  return 0;
}

const String::utf8_char_type * 
StringUtf16::as_utf8() const
{
  return 0;
}

const String::utf16_char_type * 
StringUtf16::as_utf16() const
{
  return str;
}

const String::utf32_char_type * 
StringUtf16::as_utf32() const
{
  return 0;
}

StringUtf32::StringUtf32(const utf32_char_type * s, size_type sz)
{
  if (sz == size_type(-1)) {
    sz = str_len(s);
  }
  size = sz;
#if SVGL_USE_GC
  str = new (GC) utf32_char_type[sz];
#else
  str = new utf32_char_type[sz];
#endif
  memcpy(str, s, sizeof(utf32_char_type) * sz);
}

StringUtf32::StringUtf32(const utf16_char_type * s, size_type sz)
{
  if (sz == size_type(-1)) {
    sz = str_len(s);
  }
  size = sz;
#if SVGL_USE_GC
  str = new (GC) utf32_char_type[sz];
#else
  str = new utf32_char_type[sz];
#endif
  str_cpy(str, s, sz);
}

StringUtf32::StringUtf32(const lat1_char_type * s, size_type sz)
{
  if (sz == size_type(-1)) {
    sz = strlen(s);
  }
  size = sz;
#if SVGL_USE_GC
  str = new (GC) utf32_char_type[sz];
#else
  str = new utf32_char_type[sz];
#endif
  str_cpy(str, s, sz);
}

StringUtf32::~StringUtf32()
{
  //  delete str;
}

bool
StringUtf32::equals(const String& other) const throw()
{
  if (getLength() != other.getLength())
    return false;
  const utf32_char_type * s = other.as_utf32();
  if (s == 0) {
    return String::equals(other);
  }
  else {
    return memcmp(str, s, sizeof(utf32_char_type) * size) == 0;
  }
}

String::char_type
StringUtf32::charAt(size_type index) const
  throw(std::out_of_range)
{
  if (index > size)
    throw std::out_of_range("StringUtf32::charAt");
  return str[index];
}

String * 
StringUtf32::substring(size_type from) const
  throw(std::out_of_range)
{
  size_type len = getLength();
  if (from > len) {
    throw std::out_of_range("StringUtf32::substring");
  }
  else if (from == len) {
    return 0;
  }
  return new StringUtf32(str + from, size - from);
}

String *
StringUtf32::substring(size_type from, size_type to) const
  throw(std::out_of_range)
{
  if (from > to ||
      to > getLength()) {
    throw std::out_of_range("StringUtf32::substring");
  }
  else if (from == to) {
    return 0;
  }
  return new StringUtf32(str + from, to - from);
}

String::Type
StringUtf32::getType() const { return string_utf32; }

String::size_type
StringUtf32::getSize() const { return size; }

bool
StringUtf32::hasVariableSize() const
{
  return false;
}

const String::lat1_char_type * 
StringUtf32::as_lat1() const
{
  return 0;
}

const String::utf8_char_type * 
StringUtf32::as_utf8() const
{
  return 0;
}

const String::utf16_char_type * 
StringUtf32::as_utf16() const
{
  return 0;
}

const String::utf32_char_type * 
StringUtf32::as_utf32() const
{
  return str;
}
}
