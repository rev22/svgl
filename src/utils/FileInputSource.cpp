#include <utils/FileInputSource.hpp>
#include <utils/String.hpp>


namespace sdom {

FileInputSource::FileInputSource(String * file)
  : StdioInputSource(null, file, true)
{
  char buffer[2048];
  file->transcode(buffer, 2048);
  in = fopen(buffer,"r");
}

}
