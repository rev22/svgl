#include <utils/StringBuffer.hpp>

namespace unicode {

void
StringBuffer::insert(unsigned int offset, const String * s)
{
  if (s != 0) {
    if (offset > size())
      throw std::out_of_range("StringBuffer::insert");
    String::size_type len = s->getLength();
    super::insert(begin()+offset, len, s->charAt(0));
    for (String::size_type i = 1; i < len; i++)
      (*this)[offset+i] = s->charAt(i);
  }
}

int
StringBuffer::computeWidth() const
{
  int width = 0;
  for (const_iterator i = begin(); i != end(); i++) {
    if (*i > 0x80) {
      width |= 1;
      if (*i > 0x700) {
	width |= 2;
	if (*i > 0xdb) {
	  width |= 3;
	}
      }
    }
  }
  return width;
}

String *
StringBuffer::createString() const
{
  if (size() == 0)
    return 0;
  int width = computeWidth();
  if (width == 0) {
    return new StringLat1(&*(begin()),size());
  }
  else if (width == 1) {
    return new StringUtf16(&*(begin()),size());
  }
  else {
    return new StringUtf32(&*(begin()), size());
  }
}

String *
StringBuffer::internString() const
{
  if (size() == 0)
    return 0;
  int width = computeWidth();
  if (width == 0) {
    return new StringLat1(&*(begin()), size());
  }
  else if (width == 1) {
    return new StringUtf16(&*(begin()),size());
  }
  else {
    return new StringUtf32(&*(begin()), size());
  }
}

char *
StringBuffer::transcode(char * buf, unsigned int sz) const
{
  unsigned int i;
  sz--;			// for null at the end
  for (i = 0; i < sz && i < size(); i++) {
    buf[i] = char((*this)[i]);
  }
  buf[i] = 0;
  return buf;
}


}

