#ifndef _utils_IstreamInputSource_hpp_
#define _utils_IstreamInputSource_hpp_

#include <istream>
#include <utils/InputSource.hpp>

namespace sdom {

	class IstreamInputSource : public InputSource {
	protected :
		std::istream *stream;

	public:
		IstreamInputSource(std::istream *s);

		virtual int readBytes(char *bytes, int nBytes);
		virtual bool finished() const;

		virtual String *getURI() const;
		virtual void setURI(String *uri);
		virtual String *getTitle() const;
		virtual void setTitle(String *title);
		virtual String *getEncoding() const;
		virtual void setEncoding(String *encoding);
	};

}

#endif
