#include <utils/StdioInputSource.hpp>
#include <utils/String.hpp>

#include <svgl/debug.hpp>

namespace sdom {

StdioInputSource::StdioInputSource(FILE * i, String * uri, bool c)
    : in(i),
      URI(uri),
      close(c)
{
}

StdioInputSource::~StdioInputSource()
{
    if (close && in != null) {
	fclose(in);
	in = null;
    }
}

int
StdioInputSource::readBytes(char * bytes, int nBytes)
{
  int ret;
    if (in != 0)
	ret = fread(bytes, 1, nBytes, in);
    else
	ret = 0;
    //bytes[nBytes]=0;
    //std::cerr << bytes << __FL__;
    return ret;
}

bool
StdioInputSource::finished() const
{
    return in == null || feof(in);
}

String *
StdioInputSource::getURI() const
{
    return URI;
}

void
StdioInputSource::setURI(String * uri)
{
    URI = uri;
}

String *
StdioInputSource::getEncoding() const
{
    return null;
}

void
StdioInputSource::setEncoding(String * )
{
}

String *
StdioInputSource::getTitle() const
{
    return URI;
}

void
StdioInputSource::setTitle(String * title)
{
    URI = title;
}

}

