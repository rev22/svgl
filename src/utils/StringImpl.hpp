#ifndef _dom_StringImpl_hpp
#define _dom_StringImpl_hpp

#include <utils/String.hpp>


namespace unicode {

class StringLat1 : public String {

protected:
  lat1_char_type * str;
  
public:
  StringLat1(const lat1_char_type * s, size_type size);
  StringLat1(const utf32_char_type * s, size_type size);
  ~StringLat1();
  virtual bool equals (const String& other) const throw();

  virtual char_type charAt(size_type) const
    throw(std::out_of_range);
  virtual String * substring(size_type from) const
    throw(std::out_of_range);
  virtual String * substring(size_type from, size_type to) const
    throw(std::out_of_range);

  virtual Type getType() const;
  virtual size_type getSize() const;
  virtual bool hasVariableSize() const;
  virtual const lat1_char_type * as_lat1() const;
  virtual const utf8_char_type * as_utf8() const;
  virtual const utf16_char_type * as_utf16() const;
  virtual const utf32_char_type * as_utf32() const;
  virtual const byte_type * repr() const { return str; }
};

class StringUtf8 : public unicode::String {
protected:
  utf8_char_type * str;
  size_type length;
  
public:
  StringUtf8(const utf8_char_type * str, size_type size);
  ~StringUtf8();
  virtual bool equals (const String& other) const throw();

  virtual size_type getLength() const throw();
  virtual char_type charAt(size_type) const
    throw(std::out_of_range);
  virtual String * substring(size_type from) const
    throw(std::out_of_range);
  virtual String * substring(size_type from, size_type to) const
    throw(std::out_of_range);

  virtual Type getType() const;
  virtual size_type getSize() const;
  virtual bool hasVariableSize() const;
  virtual const lat1_char_type * as_lat1() const;
  virtual const utf8_char_type * as_utf8() const;
  virtual const utf16_char_type * as_utf16() const;
  virtual const utf32_char_type * as_utf32() const;
  virtual const byte_type * repr() const { return str; }
};

class StringUtf16 : public unicode::String {
protected:
  utf16_char_type * str;
  size_type length;
  
public:
  StringUtf16(const lat1_char_type * s, size_type size);
  StringUtf16(const utf16_char_type * str, size_type size);
  StringUtf16(const utf32_char_type * s, size_type size);
  ~StringUtf16();
  virtual bool equals (const String& other) const throw();

  virtual size_type getLength() const throw();
  virtual char_type charAt(size_type) const
    throw(std::out_of_range);
  virtual String * substring(size_type from) const
    throw(std::out_of_range);
  virtual String * substring(size_type from, size_type to) const
    throw(std::out_of_range);

  virtual Type getType() const;
  virtual size_type getSize() const;
  virtual bool hasVariableSize() const;
  virtual const lat1_char_type * as_lat1() const;
  virtual const utf8_char_type * as_utf8() const;
  virtual const utf16_char_type * as_utf16() const;
  virtual const utf32_char_type * as_utf32() const;
  virtual const byte_type * repr() const { return (byte_type*)str; }
};

class StringUtf32 : public unicode::String {
protected:
  char_type * str;
  
public:
  StringUtf32(const lat1_char_type * s, size_type size);
  StringUtf32(const utf16_char_type * str, size_type size);
  StringUtf32(const utf32_char_type * s, size_type size);
  ~StringUtf32();
  virtual bool equals (const String& other) const throw();

  virtual char_type charAt(size_type) const
    throw(std::out_of_range);
  virtual String * substring(size_type from) const
    throw(std::out_of_range);
  virtual String * substring(size_type from, size_type to) const
    throw(std::out_of_range);

  virtual Type getType() const;
  virtual size_type getSize() const;
  virtual bool hasVariableSize() const;
  virtual const lat1_char_type * as_lat1() const;
  virtual const utf8_char_type * as_utf8() const;
  virtual const utf16_char_type * as_utf16() const;
  virtual const utf32_char_type * as_utf32() const;
  virtual const byte_type * repr() const { return (byte_type*)str; }
};

} // namespace unicode

#endif
