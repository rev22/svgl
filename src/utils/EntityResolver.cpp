#include <fstream>
#include <string.h>
#include <map>
#include <vector>
#include <stdlib.h>
#include <utils/EntityResolver.hpp>
#include <utils/FileInputSource.hpp>

#include <utils/String.hpp>

#if SVGL_USE_GC
#include <utils/gc_alloc.hpp>
#endif

namespace sdom {

#if SVGL_USE_GC
typedef std::map<String*,String*,string_less, gc_alloc<String*> > StringMap;
#else
typedef std::map<String*,String*,string_less> StringMap;
#endif

static StringMap		publicIdFiles;
static std::vector<String*>		paths;

void
EntityResolver::addPublicIdFile(String * pubId, String * file)
{
  publicIdFiles.insert(StringMap::value_type(pubId, file));
}

void
EntityResolver::removePublicIdFile(String * pubId)
{
  StringMap::iterator i = publicIdFiles.find(pubId);
  if (i != publicIdFiles.end())
    publicIdFiles.erase(i);
}

String *
EntityResolver::getPublicIdFile(String * pubId)
{
  StringMap::iterator i = publicIdFiles.find(pubId);
  if (i != publicIdFiles.end())
    return i->second;
  return null;
}  

EntityResolver *		EntityResolver::instance_;


EntityResolver::EntityResolver()
{
}

EntityResolver::~EntityResolver()
{
}

EntityResolver *
EntityResolver::instance()
{
  if (instance_ == null)
    instance_ = new EntityResolver();
  return instance_;
}

static FILE *
try_open(String * name, String * path)
{
  char buffer[1024];
  char * end = buffer;

  buffer[0] = 0;
  if (path != null) {
    path->transcode(buffer, 1024);
    end = buffer + strlen(buffer);
    *end++ = '/';
  }
  name->transcode(end, 1024 - (end - buffer));
  return fopen(buffer, "rb");
}

static FILE *
try_open(String * name)
{
  FILE * in = try_open(name, null);
  if (in != null)
    return in;
  for (unsigned i = 0; i != EntityResolver::getPathLength(); i++) {
    in = try_open(name, EntityResolver::getPath(i));
    if (in != null)
      return in;
  }
  return null;
}

InputSource *
EntityResolver::resolveEntity(String * publicId, String * systemId)
{
  FILE * in;
  if (systemId != null) {
    in = try_open(systemId);
    if (in != null)
      return new StdioInputSource(in, systemId);
  }
  if (publicId != null) {
    systemId = getPublicIdFile(publicId);
  }
  if (systemId == 0)
    return null;
  // should create an URL here +++ FIXME
  in = try_open(systemId);
  if (in == null)
    return null;
  return new StdioInputSource(in, systemId);
}

static void
readCatalogEntry(std::ifstream& in, char buffer[2048], const char * dirname)
{
  char * p = buffer + 7;
  while (*p == ' ' ||
	 *p == '\t')
    p++;
  if (*p++ != '"')
    return;			// syntax error
  const char * pubStart = p;
  while (*p != 0 &&
	 *p != '"')
    p++;
  if (*p == 0)
    return;			// syntax error
  const char * pubEnd = p++;
  String * pubId = String::createString(pubStart, pubEnd - pubStart);
  while (*p == ' ' ||
	 *p == '\t')
    p++;
  if (*p == '\n' || *p == 0)
    in.getline(buffer, 2048);
  while (*p == ' ' ||
	 *p == '\t')
    p++;
  if (*p == 0)
    return;			// syntax error
  bool quoted = false;
  if (*p == '"') {
    quoted = true;
    p++;
  }
  const char * sysStart = p;
  if (quoted) {
    while (*p != 0 &&
	   *p != '"')
      p++;
  }
  else {
    while (*p != 0 &&
	   *p != ' ' &&
	   *p != '\t' &&
	   *p != '\n')
      p++;
  }
  const char * sysEnd = p;

  EntityResolver::addPublicIdFile(pubId,
				  String::createString(sysStart,
						       sysEnd - sysStart));
  EntityResolver::addPath(String::internString(dirname));
}

void
EntityResolver::addCatalogFile(String * file)
{
  char filename[1024];
  file->transcode(filename, 1024);
  std::ifstream in(filename);
  char * p = strrchr(filename, '/');
  if (p != 0)
    *p = 0;
  while (in) {
    char buffer[2048];
    in.getline(buffer, 2048);
    if (strncmp(buffer, "PUBLIC ", 7) == 0) {
      readCatalogEntry(in, buffer, filename);
    }
  }
}

void
EntityResolver::addPath(String * s)
{
  for (unsigned int i = 0; i <getPathLength(); i++) {
    if (s->equals(getPath(i)))
      return;
  }
  paths.push_back(s);
}

unsigned
EntityResolver::getPathLength()
{
  return paths.size();
}

String *
EntityResolver::getPath(unsigned index)
{
  if (index < paths.size())
    return paths[index];
  else
    return null;
}

void
EntityResolver::initCatalogFile()
{
  const char * cat = getenv("CATALOG_FILE");
  if (cat != 0) 
    addCatalogFile(String::createString(cat));
}

}
