#ifndef _dom_AttributeMap_hpp_
#define _dom_AttributeMap_hpp_

#include <w3c/dom/Attr.hpp>
#include <w3c/dom/impl/NamedNodeMapImpl.hpp>

namespace dom {

class Element;

class AttributeMap : public NamedNodeMapImpl
{
public:
  			AttributeMap(Element * ownerNode, const NamedNodeMapImpl * defaults);
  			AttributeMap(const AttributeMap& other);
  virtual Node *	setNamedItem(Node * arg)
    throw(DOMException);
  virtual Node *	setNamedItemNS(Node * arg)
    throw(DOMException);
  virtual Node *	removeNamedItem(String * name)
    throw(DOMException);
  virtual Node *	safeRemoveNamedItem(String * name);
  virtual Node *	removeNamedItemNS(String * namespaceURI, String * name)
    throw(DOMException);
  virtual Node *	safeRemoveNamedItemNS(String * namespaceURI, String * name);
  virtual NamedNodeMapImpl * cloneMap(Node * ownerNode) const;
  
  Attr *		getNamedAttr(String * name) const {
    return dynamic_cast<Attr*>(getNamedItem(name));
  }
  Attr *		setNamedAttr(Attr * arg)
    throw(DOMException) {
    return dynamic_cast<Attr*>(setNamedItem(arg));
  }
  Attr *		removeNamedAttr(String * name)
    throw(DOMException) {
    return dynamic_cast<Attr*>(removeNamedItem(name));
  }
  Attr *		attr(unsigned int index) const {
    return dynamic_cast<Attr*>(item(index));
  }
  Attr *		getNamedAttrNS(String * namespaceURI, 
				       String * localName) const {
    return dynamic_cast<Attr*>(getNamedItemNS(namespaceURI, localName));
  }
  Attr *		setNamedAttrNS(Attr * arg)
    throw(DOMException) {
    return dynamic_cast<Attr*>(setNamedItemNS(arg));
  }
  Attr *		removeNamedAttrNS(String * namespaceURI, 
					  String * localName)
    throw(DOMException) {
    return dynamic_cast<Attr*>(removeNamedItemNS(namespaceURI, localName));
  }
  virtual void		reconcileDefaults(NamedNodeMapImpl * defaults);
protected:
  Element *		ownerElement;
  Node *		internalRemoveNamedItem(String * name, bool raiseEx);
  Node *		internalRemoveNamedItemNS(String * namespaceURI, String * name, bool raiseEx);
  virtual void		cloneContent(const NamedNodeMapImpl * srcmap);
};

}

#endif
