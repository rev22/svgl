#include <assert.h>
#include <w3c/dom/Attr.hpp>
#include <w3c/dom/AttrDef.hpp>
#include <w3c/dom/impl/AttributeMap.hpp>
#include <w3c/dom/Document.hpp>
#include <w3c/dom/DocumentType.hpp>
#include <w3c/dom/Element.hpp>
#include <w3c/dom/Text.hpp>
#include <utils/StringBuffer.hpp>

namespace dom {

Element::Element(Document * ownerDoc, String * n)
  : ParentNode(ownerDoc),
    name(n),
    attributes(0),
    namespaceURI(0),
    localName(n)
{
  assert(ownerDoc != null);
  attributes = ownerDoc->createAttributeMap(this, 0);
  reconcileDefaultAttributes();
}

Element::Element(Document * ownerDoc,
			 String * ns, String * qualifiedName)
  : ParentNode(ownerDoc),
    name(qualifiedName),
    attributes(0),
    namespaceURI(0),
    localName(0)
{
  assert(ownerDoc != null);
  attributes = ownerDoc->createAttributeMap(this, 0);
  int index = qualifiedName->indexOf(':');
  String * prefix;
  if (index < 0) {
    prefix = null;
    localName = qualifiedName;
    // +++ FIXME, check for "xmlns" namespace
  }
  else {
    prefix = qualifiedName->substring(0, index);
    localName = qualifiedName->substring(index+1);
    // +++ FIXME, check for "xmlns" namespace
  }
  namespaceURI = ns;
  reconcileDefaultAttributes();
}

Element::Element(const Element& other)
  : ParentNode(other),
    name(other.name),
    attributes(0),
    namespaceURI(other.namespaceURI),
    localName(other.localName)
{
  assert(ownerDoc != null);
  attributes = ownerDoc->createAttributeMap(this, other.attributes);
  reconcileDefaultAttributes();
}

Node::NodeType
Element::getNodeType() const
{
  return Node::ELEMENT_NODE;
}

String *
Element::getNodeName() const
{
  return name;
}

NamedNodeMap *
Element::getAttributes() const
{
  return attributes;
}

Node *
Element::cloneNode(bool deep) const
{
  Element * newnode = new Element(*this);
  if (deep != 0) {
    // FIXME +++
  }
  return newnode;
}

void
Element::setOwnerDocument(Document * doc)
{
  ParentNode::setOwnerDocument(doc);
  if (attributes != null)
    attributes->setOwnerDocument(doc);
}

String *
Element::getAttribute(String * name) const
{
  if (attributes == null)
    return null;
  Node * attr = attributes->getNamedItem(name);
  if (attr == null)
    return 0;
  else
    return attr->getNodeValue();
}

Attr *
Element::getAttributeNode(String * name) const
{
  if (attributes == null)
    return null;
  return attributes->getNamedAttr(name);
}

NodeList *
Element::getElementsByTagName(String * tagname) const
{
  throw DOMException(DOMException::NOT_SUPPORTED_ERR);
}

String *
Element::getTagName() const
{
  return name;
}

void
Element::normalize()
{
  ChildNode * kid, * next;
  for (kid = firstChild; kid != null; kid = next) {
    next = kid->nextSibling;
    if (kid->getNodeType() == Node::TEXT_NODE) {
      if (next != null &&
	  next->getNodeType() == Node::TEXT_NODE) {
	Text * text = dynamic_cast<Text*>(kid);
	text->appendData(next->getNodeValue());
	removeChild(next);
	next = kid;
      }
      else {
	if (kid->getNodeValue()->getLength() == 0)
	  removeChild(kid);
      }
    }
    else if (kid->getNodeType() == Node::ELEMENT_NODE) {
      kid->normalize();
    }
  }
  if (attributes != null) {
    for (AttributeMap::iterator i = attributes->begin();
	 i != attributes->end(); i++) {
      AttributeMap::it(i)->normalize();
    }
  }
}

void
Element::removeAttribute(String * name) throw (dom::DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  if (attributes == null)
    return;
  attributes->safeRemoveNamedItem(name);
}

Attr *
Element::removeAttributeNode(Attr * oldAttr)
  throw(DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  if (attributes == null)
    throw DOMException(DOMException::NOT_SUPPORTED_ERR);
  return attributes->removeNamedAttr(oldAttr->getName());
}

void
Element::setAttribute(String * name, String * value) throw (dom::DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  Attr * newAttr = getAttributeNode(name);
  if (newAttr == null) {
    newAttr = getOwnerDocument()->createAttribute(name);

    if (attributes == null)
      attributes = new AttributeMap(this, null);

    newAttr->setNodeValue(value);
    attributes->setNamedItem(newAttr);
  }
  else {
    newAttr->setNodeValue(value);
  }
}

Attr *
Element::setAttributeNode(Attr * newAttr)
    throw(DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  if (newAttr->getOwnerDocument() != ownerDoc)
    throw DOMException(DOMException::WRONG_DOCUMENT_ERR);
  if (attributes == null) {
    attributes = new AttributeMap(this, null);
    attributes->setNamedItem(newAttr);
    return null;
  }
  return attributes->setNamedAttr(newAttr);
}

String *
Element::getAttributeNS(String * namespaceURI,
			    String * localName) const
{
  if (attributes == null)
    return null;
  Attr * attr = attributes->getNamedAttrNS(namespaceURI, localName);
  return (attr == null) ? null : attr->getValue();
}

void
Element::setAttributeNS(String * namespaceURI,
			    String * localName,
			    String * value) throw (dom::DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  Attr * newAttr = getAttributeNodeNS(namespaceURI, localName);
  if (newAttr == null) {
    newAttr = getOwnerDocument()->createAttributeNS(namespaceURI, localName);
    if (attributes == null)
      attributes = new AttributeMap(this, null);
    newAttr->setNodeValue(value);
    attributes->setNamedItemNS(newAttr);
  }
  else {
    newAttr->setNodeValue(value);
  }
}

void
Element::removeAttributeNS(String * namespaceURI,
			       String * localName) throw (dom::DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  if (attributes == null)
    return;
  attributes->safeRemoveNamedItemNS(namespaceURI, localName);
}

Attr *
Element::getAttributeNodeNS(String * namespaceURI,
				String * localName) const
{
  if (attributes == null)
    return null;
  return attributes->getNamedAttrNS(namespaceURI, localName);
}

Attr *
Element::setAttributeNodeNS(Attr * newAttr)
  throw(DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  if (newAttr->getOwnerDocument() != ownerDoc)
    throw DOMException(DOMException::WRONG_DOCUMENT_ERR);

  if (attributes == null)
    attributes = new AttributeMap(this, null);
  return attributes->setNamedAttrNS(newAttr);
}

bool
Element::hasAttributes() const
{
  return attributes != null && attributes->getLength() != 0;
}

bool
Element::hasAttribute(String * name) const
{
  return getAttributeNode(name) != null;
}

bool
Element::hasAttributeNS(String * namespaceURI,
			    String * localName) const
{
  return getAttributeNodeNS(namespaceURI, localName) != null;
}

NodeList *
Element::getElementsByTagNameNS(String * namespaceURI,
				    String * localName) const
{
  throw DOMException(DOMException::NOT_SUPPORTED_ERR);
}

void
Element::setReadOnly(bool readOnly, bool deep)
{
  ParentNode::setReadOnly(readOnly, deep);
  if (attributes != null)
    attributes->setReadOnly(readOnly, true);
}

void
Element::reconcileDefaultAttributes()
{
  NamedNodeMapImpl * defaults = getAttributeDefs();
  if (defaults == null)
    return;

  if (defaults != null)
    attributes->reconcileDefaults(defaults);
}

NamedNodeMapImpl *
Element::getAttributeDefs() const
{
  DocumentType * doctype = ParentNode::getOwnerDocument()->getDoctype();
  if (doctype == null)
    return null;
  Element * e =
    dynamic_cast<Element*>(doctype->getElements()->getNamedItemNS(getNamespaceURI(),
								  getLocalName()));
  if (e == 0)
    return null;
  return e->getAttributesImpl();
}

String *
Element::getNamespaceURI() const
{
  return namespaceURI;
}
String *
Element::getPrefix() const
{
  int index = name->indexOf(':');
  if (index < 0)
    return null;
  else
    return name->substring(0, index);
}

void
Element::setPrefix(String * prefix)
    throw(DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  // +++FIXME check prefix validity
  unicode::StringBuffer buf(prefix);
  buf += ":";
  buf += localName;
  name = buf.internString();
}

String *
Element::getLocalName() const
{
  return localName;
}

AttributeMap *
Element::getAttributesImpl() const
{
  return attributes;
}

bool
Element::isId(const Attr * attr) const
{
  NamedNodeMapImpl * defaults = getAttributeDefs();
  if (defaults == null)
    return false;
  AttrDef * def =
    dynamic_cast<AttrDef*>(defaults->getNamedItemNS(attr->getNamespaceURI(),
						    attr->getLocalName()));
  if (def != null &&
      def->getAttributeType() == AttrDef::ID_ATTR_TYPE)
    return true;
  return false;
}

bool
Element::hasId() const
{
  if (attributes == null)
    return false;
  for (AttributeMap::iterator i = attributes->begin();
       i != attributes->end(); i++) {
    if (AttributeMap::it(i)->isId())
      return true;
  }
  return false;
}

}
