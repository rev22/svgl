#ifndef _dom_ext_DOMParser_hpp_
#define _dom_ext_DOMParser_hpp_

#include <w3c/dom/Node.hpp>

namespace sdom {
  class InputSource;
  class EntityResolver;
}

namespace dom {

class ErrorHandler;

class DOMParser {
public:
protected:
  Document *		document;
  Node *		currentNode;
  bool			inDocument;
  bool			inCDATASection;
  String *		attrName;
  sdom::EntityResolver *	entityResolver;
  ErrorHandler *	errorHandler;
public:
  DOMParser();
  ~DOMParser();

  Document *		getDocument() const { return document; }
  void			reset();
  void			parse(String * systemID);
  void			parse(sdom::InputSource * input);
  void			setEntityResolver(sdom::EntityResolver * resolver);
  sdom::EntityResolver *	getEntityResolver() const;
  void			setErrorHandler(ErrorHandler * errorHandler);
  ErrorHandler *	getErrorHandler() const;
  
};

}

#endif
