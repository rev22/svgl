#include <w3c/dom/Document.hpp>
#include <w3c/dom/DocumentType.hpp>
#include <w3c/dom/impl/NamedNodeMapImpl.hpp>

namespace dom {

DocumentType::DocumentType(Document * ownerDocument,
				   String *n)
  : ParentNode(ownerDocument),
    name(n),
    entities(new NamedNodeMapImpl(0)),
    notations(new NamedNodeMapImpl(0)),
    elements(new NamedNodeMapImpl(0)),
    publicID(0),
    systemID(0),
    internalSubset(0)
{
}

DocumentType::DocumentType(Document * ownerDocument,
				   String * qualifiedName,
				   String * pubID, String * sysID)
  : ParentNode(ownerDocument),
    name(qualifiedName),
    entities(new NamedNodeMapImpl(0)),
    notations(new NamedNodeMapImpl(0)),
    elements(new NamedNodeMapImpl(0)),
    publicID(pubID),
    systemID(sysID),
    internalSubset(0)
{
}

DocumentType::DocumentType(const DocumentType& other)
  : ParentNode(other),
    name(other.name),
    entities(other.entities),
    notations(other.notations),
    elements(other.elements),
    publicID(other.publicID),
    systemID(other.systemID),
    internalSubset(0)
{
}

Node::NodeType
DocumentType::getNodeType() const
{
  return Node::DOCUMENT_TYPE_NODE;
}

String *
DocumentType::getNodeName() const
{
  return name;
}

Node *
DocumentType::cloneNode(bool deep) const
{
  DocumentType * ret = new DocumentType(*this);
  ret->setOwnerDocument(0);
  return ret;
}

void
DocumentType::setOwnerDocument(Document * doc)
{
  ParentNode::setOwnerDocument(doc);
#if 0
  entities->setOwnerDocument(doc);
  notations->setOwnerDocument(doc);
  elements->setOwnerDocument(doc);
#endif
}

void
DocumentType::setReadOnly(bool readOnly, bool deep)
{
  ParentNode::setReadOnly(readOnly, deep);
  elements->setReadOnly(readOnly, true);
  entities->setReadOnly(readOnly, true);
  notations->setReadOnly(readOnly, true);
}

String *
DocumentType::getName() const
{
  return name;
}

NamedNodeMap *
DocumentType::getEntities() const
{
  return entities;
}

NamedNodeMap *
DocumentType::getNotations() const
{
  return notations;
}

NamedNodeMap *
DocumentType::getElements() const
{
  return elements;
}

String *
DocumentType::getPublicId() const
{
  return publicID;
}

String *
DocumentType::getSystemId() const
{
  return systemID;
}

String *
DocumentType::getInternalSubset() const
{
  return internalSubset;
}

bool
DocumentType::isLoaded() const
{
  return elements->getLength() != 0;
}

Node *
DocumentType::insertBefore(Node * newChild, Node * refChild)
  throw(DOMException)
{
  NodeType type = newChild->getNodeType();
  if ((type != Node::ELEMENT_NODE) &&
      (type != Node::COMMENT_NODE))
    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
  ParentNode::insertBefore(newChild, refChild);
  if (type == Node::ELEMENT_NODE)
    elements->setNamedItemNS(newChild);
  return newChild;
}

Node *
DocumentType::removeChild(Node * oldChild)
    throw(DOMException)
{
  ParentNode::removeChild(oldChild);
  NodeType type = oldChild->getNodeType();
  if (type == Node::ELEMENT_NODE)
    elements->removeNamedItemNS(oldChild->getNamespaceURI(),
				oldChild->getLocalName());
  return oldChild;
}

Node *
DocumentType::replaceChild(Node * newChild, Node * oldChild)
    throw(DOMException)
{
  ParentNode::replaceChild(newChild, oldChild);

  NodeType type = oldChild->getNodeType();
  if (type == Node::ELEMENT_NODE) {
    elements->setNamedItemNS(newChild);
  }
  return newChild;
}

}
