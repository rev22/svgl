#include <w3c/dom/Node.hpp>
#include <w3c/dom/DOMException.hpp>
#include <w3c/dom/Document.hpp>
#include <w3c/dom/DOMImplementation.hpp>
#include <w3c/dom/impl/NodeListImpl.hpp>

namespace dom {

Node::~Node() {}

Node::Node(Document * o)
  : ownerNode(o), flags(0)
{}

Node::Node(const Node& other)
  : ownerNode(other.getOwnerDocument()),
    flags(other.flags)
{
  isOwned(false);
  isReadOnly(false);
}

String *
Node::getNodeValue() const
  throw(DOMException)
{
  return 0;
}

void
Node::setNodeValue(String *)
  throw(DOMException)
{
  throw DOMException(DOMException::NO_DATA_ALLOWED_ERR);
}

Node *
Node::getParentNode() const
{
  return 0;
}

const NodeList *
Node::getChildNodes() const
{
  return EmptyNodeList::instance();
}

Node *
Node::getFirstChild() const
{
  return 0;
}

Node *
Node::getLastChild() const
{
  return 0;
}

Node *
Node::getPreviousSibling() const
{
  return 0;
}

Node *
Node::getNextSibling() const
{
  return 0;
}

NamedNodeMap *
Node::getAttributes() const
{
  return 0;
}

Document *
Node::getOwnerDocument() const
{
  if (isOwned())
    return ownerNode->getOwnerDocument();
  else
    return dynamic_cast<Document*>(ownerNode);
}

void
Node::setOwnerDocument(Document * doc)
{
  if (!isOwned())
    ownerNode = doc;
}

Node *
Node::insertBefore(Node * newChild, Node * refChild)
  throw(DOMException)
{
  throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
}

Node *
Node::replaceChild(Node * newChild, 
				     Node * oldChild)
  throw(DOMException)
{
  throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
}

Node *
Node::removeChild(Node * oldChild)
  throw(DOMException)
{
  throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
}

Node *
Node::appendChild(Node * newChild)
  throw(DOMException)
{
  return insertBefore(newChild, 0);
}

bool
Node::hasChildNodes() const
{
  return false;
}

void
Node::normalize()
{
}

bool
Node::isSupported(String * feature, 
		  String * version)
{
  return getOwnerDocument()->getImplementation()->hasFeature(feature,
							     version);
}

String *
Node::getNamespaceURI() const
{
  return 0;
}

String *
Node::getPrefix() const
{
  return 0;
}

void
Node::setPrefix(String * prefix)
  throw(DOMException)
{
  throw DOMException(DOMException::NAMESPACE_ERR);
}

String *
Node::getLocalName() const
{
  return 0;
}

bool
Node::hasAttributes() const
{
  return false;
}

void
Node::setReadOnly(bool readOnly, bool)
{
  isReadOnly(readOnly);
}

bool
Node::getReadOnly()
{
  return isReadOnly();
}


void
Node::changed()
{
  getOwnerDocument()->changed();
}

int
Node::changes() const
{
  return getOwnerDocument()->changes();
}


}
