#ifndef _utils_EntityResolver_hpp_
#define _utils_EntityResolver_hpp_

//#include <utils/String.hpp>
//#include <utils/InputSource.hpp>

namespace unicode {
  class String;
};

namespace sdom {
  class InputSource;
}

namespace sdom {

  using unicode::String;

class EntityResolver {
protected:
  static EntityResolver *	instance_;
public:
  				EntityResolver();
  virtual			~EntityResolver();
  static EntityResolver *	instance();

  virtual sdom::InputSource *		resolveEntity(String * publicId,
					      String * systemId);

  static void			addPublicIdFile(String*, String*);
  static void			removePublicIdFile(String*);
  static String *		getPublicIdFile(String*);
  static void			addCatalogFile(String*);
  static void			addPath(String *);
  static unsigned		getPathLength();
  static String *		getPath(unsigned index);
  static void			initCatalogFile();
};

}

#endif
