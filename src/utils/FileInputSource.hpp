#ifndef _utils_FileInputSource_hpp_
#define _utils_FileInputSource_hpp_

#include <utils/StdioInputSource.hpp>

namespace sdom {

class FileInputSource : public StdioInputSource
{
public:
  FileInputSource(String * file);

};

}

#endif
