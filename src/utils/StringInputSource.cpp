#include <utils/StringInputSource.hpp>
#include <utils/String.hpp>

#include <svgl/debug.hpp>

namespace sdom {

StringInputSource::StringInputSource(String * content)
  : _content(content), _count(0)
{
}

StringInputSource::~StringInputSource()
{
}

int
StringInputSource::readBytes(char * bytes, int nBytes)
{
  //std::cerr << DBGVAR(_count) << DBGVAR(nBytes) << DBGVAR(_content->getLength()) << __FL__;
  //String::size_type c = _count;
  if( (_count+nBytes) >= _content->getLength() ) {
    nBytes  =_content->getLength()-_count;
  }

  memcpy(bytes, &(_content->repr())[_count], nBytes);
  //bytes[nBytes]=0;
  //std::cerr << bytes << __FL__;
  _count += nBytes;
  return nBytes;

}

bool
StringInputSource::finished() const
{
  return (_count >= _content->getLength());
}

String *
StringInputSource::getURI() const
{
  static String * file_string = String::internString("file://");
  return file_string;
}

void
StringInputSource::setURI(String * uri)
{
}

String *
StringInputSource::getEncoding() const
{
    return null;
}

void
StringInputSource::setEncoding(String * )
{
}

String *
StringInputSource::getTitle() const
{
    return 0;
}

void
StringInputSource::setTitle(String * title)
{
}

}

