#include <utils/IstreamInputSource.hpp>
#include <utils/String.hpp>

namespace sdom {

	IstreamInputSource::IstreamInputSource(std::istream *s) : InputSource(), stream(s) {}

	int IstreamInputSource::readBytes(char *bytes, int nBytes) {
		stream->get(bytes, nBytes, EOF);
		return stream->gcount();
	}

	bool IstreamInputSource::finished() const {
		return stream->eof();
	}

	String *IstreamInputSource::getURI() const   {
		static String *uri = String::internString("stream");
		return uri;
	}
	void IstreamInputSource::setURI(String *uri) {}
	
	String *IstreamInputSource::getTitle() const     { return 0; }
	void IstreamInputSource::setTitle(String *title) {}

	String *IstreamInputSource::getEncoding() const        { return 0; }
	void IstreamInputSource::setEncoding(String *encoding) {}

}
