#include <w3c/dom/Attr.hpp>
#include <w3c/dom/Document.hpp>
#include <w3c/dom/Element.hpp>
#include <w3c/dom/Text.hpp>
#include <utils/StringBuffer.hpp>

namespace dom {

Attr::Attr(Document * ownerDocument, String * n)
  : Node(ownerDocument),
    name(n),
    text(null),
    specified(true),
    namespaceURI(null),
    localName(n)
{
}

Attr::Attr(Document * ownerDocument,
		   String * ns, String * qualifiedName)
  : Node(ownerDocument),
    name(qualifiedName),
    text(null),
    specified(true),
    namespaceURI(0),
    localName(null)
{
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
}

Attr::Attr(const Attr& other)
  : Node(other),
    name(other.name),
    text(null),
    specified(other.specified),
    namespaceURI(other.namespaceURI),
    localName(other.localName)
{
  setId();
}

unsigned int
Attr::getLength() const
{
  unsigned int length = 0;
  for (ChildNode * node = text; node != null; node = node->nextSibling) {
    length++;
  }
  return length;
}

Node *
Attr::item(unsigned int index) const
{
  ChildNode * node;
  for (node = text; node != null && index-- != 0; node = node->nextSibling)
    ;
  return node;
}

void
Attr::setOwnerDocument(Document * doc)
{
  Node::setOwnerDocument(doc);
  for (ChildNode * child = text;
       child != null; child = child->nextSibling) {
    child->setOwnerDocument(doc);
  }
  setId();
}

void
Attr::setOwnerElement(Element * e)
{
  if (e == null) {
    if (ownerNode != null) {
      ownerNode = getOwnerDocument();
      isOwned(false);
      isSpecified(true);
      isId(false);
    }
  }
  else {
    if (isOwned())
      throw DOMException(DOMException::INUSE_ATTRIBUTE_ERR);
    ownerNode = e;
    isOwned(true);
    setId();
  }
}

String *
Attr::getNodeName() const
{
  return name;
}

String *
Attr::getNodeValue() const
  throw (DOMException)
{
  return getValue();
}
void
Attr::setNodeValue(String * value)
  throw(DOMException)
{
  setValue(value);
}

Node::NodeType
Attr::getNodeType() const
{
  return Node::ATTRIBUTE_NODE;
}

Node *
Attr::cloneNode(bool deep) const
{
  Attr * attr = new Attr(*this);
  if (deep) {
    attr->setValue(getValue());
  }
  return attr;
}

String *
Attr::getName() const
{
  return name;
}

bool
Attr::getSpecified() const
{
  return isSpecified();
}

void
Attr::setSpecified(bool arg)
{
  isSpecified(arg);
}

String *
Attr::getValue() const
{
  if (text == null)
    return null;
  if (text->getNextSibling() == text)
    return text->getData();

  unicode::StringBuffer buf;
  for (Node * n = text; n != null; n = n->getNextSibling()) {
    buf.append(n->getNodeValue());
  }
  return buf.createString();
}

void
Attr::setValue(String * newvalue)
  throw(DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  if (newvalue == null) {
    if (isId())
      removeIdRef();
    while (text != null)
      removeChild(text);
    return;
  }
  if (text == null) {
    text = getOwnerDocument()->createTextNode(newvalue);
    text->isFirstChild(true);
    text->previousSibling = text;
    text->ownerNode = this;
    text->isOwned(true);
    addIdRef();
  }
  else {
    removeIdRef();
    while (text->getNextSibling() != null) {
      removeChild(text->getNextSibling());
    }
    text->setData(newvalue);
    addIdRef();
  }

  isSpecified(true);
}

Element *
Attr::getOwnerElement() const
{
  if (isOwned())
    return dynamic_cast<Element*>(ownerNode);
  else
    return null;
}

void
Attr::normalize()
{
  setNodeValue(getNodeValue());
}

bool
Attr::hasChildNodes() const
{
  return text != null;
}

NodeList *
Attr::getChildNodes()
{
  return this;
}

Node *
Attr::getFirstChild() const
{
  return text;
}

Node *
Attr::getLastChild() const
{
  return lastChild();
}

ChildNode *
Attr::lastChild() const
{
  if (text == null)
    return null;
  return text->previousSibling;
}

void
Attr::lastChild(ChildNode * node)
{
  if (text != null)
    text->previousSibling = node;
}

Node *
Attr::insertBefore(Node * newChild,
		       Node * refChild)
  throw(DOMException)
{
  return internalInsertBefore(newChild, refChild);
}

Node *
Attr::internalInsertBefore(Node * newChild,
			   Node * refChild)
  throw(DOMException)
{
  Document * owner = getOwnerDocument();

  if (newChild == refChild) {
    refChild = refChild->getNextSibling();
    removeChild(newChild);
    insertBefore(newChild, refChild);
    return newChild;
  }
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  if (newChild->getOwnerDocument() != owner)
    throw DOMException(DOMException::WRONG_DOCUMENT_ERR);
  if (refChild != null && refChild->getParentNode() != this)
    throw DOMException(DOMException::NOT_FOUND_ERR);
  if (newChild != null &&
      newChild->getNodeType() != Node::TEXT_NODE)
    throw DOMException(DOMException::NOT_SUPPORTED_ERR);

  Text * newInternal = dynamic_cast<Text*>(newChild);

  Node * oldparent = newInternal->getParentNode();

  if (oldparent != null)
    oldparent->removeChild(newInternal);

  Text * refInternal = dynamic_cast<Text*>(refChild);

  newInternal->ownerNode = this;
  newInternal->isOwned(true);

  if (text == null) {
    text = newInternal;
    newInternal->isFirstChild(true);
    newInternal->previousSibling = newInternal;
  }
  else {
    if (refInternal == null) {
      ChildNode * lastChild = text->previousSibling;
      lastChild->nextSibling = newInternal;
      newInternal->previousSibling = lastChild;
      text->previousSibling = newInternal;
    }
    else {
      if (refChild == text) {
	text->isFirstChild(false);
	newInternal->nextSibling = text;
	newInternal->previousSibling = text->previousSibling;
	text->previousSibling = newInternal;
	text = newInternal;
	newInternal->isFirstChild(true);
      }
      else {
	ChildNode * prev = refInternal->previousSibling;
	newInternal->nextSibling = refInternal;
	refInternal->previousSibling = newInternal;
	newInternal->previousSibling = prev;
      }
    }
  }
  changed();
  return newChild;
}

Node *
Attr::removeChild(Node * oldChild) 
    throw(DOMException)
{
  if (text == 0)
    throw DOMException(DOMException::NOT_FOUND_ERR);
  return internalRemoveChild(oldChild);
}

Node *
Attr::internalRemoveChild(Node * oldChild)
  throw(DOMException)
{
  Document * owner = getOwnerDocument();
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  if (oldChild != null && oldChild->getParentNode() != this)
    throw DOMException(DOMException::NOT_FOUND_ERR);
  owner->removedChildNode(oldChild);

  ChildNode * oldInternal = dynamic_cast<ChildNode*>(oldChild);
  if (oldInternal == text) {
    text->isFirstChild(false);
    text = dynamic_cast<Text*>(text->nextSibling);
    if (text != null) {
      text->isFirstChild(true);
      text->previousSibling = oldInternal->previousSibling;
    }
  }
  else {
    ChildNode * prev = oldInternal->previousSibling;
    ChildNode * next = oldInternal->nextSibling;
    prev->nextSibling = next;
    if (next != null) {
      text->previousSibling = prev;
    }
    else {
      next->previousSibling = prev;
    }
  }
  //ChildNode * oldPreviousSibling = oldInternal->previousSibling;
  oldInternal->ownerNode = owner;
  oldInternal->isOwned(false);
  oldInternal->nextSibling = null;
  oldInternal->previousSibling = null;
  changed();
  return oldInternal;
}

String *
Attr::getNamespaceURI() const
{
  return namespaceURI;
}

String *
Attr::getPrefix() const
{
  int index = name->indexOf(':');
  if (index < 0)
    return null;
  return name->substring(0, index);
}

void
Attr::setPrefix(String * prefix)
  throw(DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  // +++FIXME more checks
  unicode::StringBuffer buf(prefix);
  buf += ":";
  buf += localName;
  name = buf.internString();
}

String *
Attr::getLocalName() const
{
  return localName;
}

bool
Attr::setId()
{
  Element * e = getOwnerElement();
  if (e == null)
    return false;
  if (e->isId(this)) {
    isId(true);
    return true;
  }
  return false;
}

void
Attr::addIdRef() const
{
  if (isId())
    getOwnerDocument()->putIdentifier(getNodeValue(), getOwnerElement());
}

void
Attr::removeIdRef() const
{
  if (isId())
    getOwnerDocument()->removeIdentifier(getNodeValue());
}

}
