#include <algorithm>
#include <functional>
#include <w3c/dom/impl/NamedNodeMapImpl.hpp>
#include <w3c/dom/Node.hpp>

namespace dom {

NamedNodeMap::~NamedNodeMap() {}

#if 0
struct find_node_named
{
  const String * name;
  find_node_named(const String * n) : name(n) {}
  bool operator()(const Node * x) const {
    return x->getNodeName()->equals(name);
  }
};

struct find_node_ns_named
{
  const String * localName;
  const String * namespaceURI;
  
  find_node_ns_named(const String * ln, const String * ns)
    : localName(ln), namespaceURI(ns) {}
  bool operator()(const Node * x) const {
    return x->getLocalName()->equals(localName) &&
      x->getNamespaceURI()->equals(namespaceURI);
  }
};
#endif

NamedNodeMapImpl::NamedNodeMapImpl(Node * n)
  : flags(0),
    ownerNode(n)
{}

NamedNodeMapImpl::NamedNodeMapImpl(const NamedNodeMapImpl& other)
  : flags(0),
    ownerNode(other.ownerNode)
{
  for (const_iterator i = other.begin(); i != other.end(); i++) {
    Node * n = it(i);
    Node * ni = n->cloneNode(true);
    ni->isSpecified(n->isSpecified());
    map.insert(Map::value_type(n->getNodeName(), ni));
  }
}

NamedNodeMapImpl::~NamedNodeMapImpl() {}

NamedNodeMapImpl::iterator
NamedNodeMapImpl::find(String * name)
{
#if 0
  return std::find_if(begin(), end(),
		      find_node_named(name));
#else
  return map.find(name);
#endif
}

NamedNodeMapImpl::const_iterator
NamedNodeMapImpl::find(String * name) const
{
#if 0
  return std::find_if(begin(), end(),
		      find_node_named(name));
#else
  return map.find(name);
#endif
}

NamedNodeMapImpl::iterator
NamedNodeMapImpl::find(String * localName, String * namespaceURI)
{
#if 0
  return std::find_if(begin(), end(),
		      find_node_ns_named(localName, namespaceURI));
#else
  iterator i = map.find(localName);
  //bool found_no_ns = (i != map.end());
  if (namespaceURI != 0) {
    while (i != map.end() &&
	   localName->equals(it(i)->getNodeName()) &&
	   (it(i)->getNamespaceURI() != null &&
	    ! namespaceURI->equals(it(i)->getNamespaceURI())))
      i++;
  }
  if (i == map.end())
    return i;
  if (! localName->equals(it(i)->getNodeName()))
    return map.end();
  return i;
#endif
}

NamedNodeMapImpl::const_iterator
NamedNodeMapImpl::find(String * localName, String * namespaceURI) const
{
#if 0
  return std::find_if(begin(), end(),
		      find_node_ns_named(localName, namespaceURI));
#else
  const_iterator i = map.find(localName);
  return i;
  if (namespaceURI != 0) {
    while (i != map.end() &&
	   localName->equals(it(i)->getNodeName()) &&
	   (it(i)->getNamespaceURI() != null &&
	    ! namespaceURI->equals(it(i)->getNamespaceURI())))
      i++;
  }
  if (i == map.end())
    return i;
  if (! localName->equals(it(i)->getNodeName()))
    return map.end();
  return i;
#endif
}

Node *
NamedNodeMapImpl::getNamedItem(String * name) const
{
  const_iterator i = find(name);
  if (i == end())
    return 0;
  else
    return it(i);
}

Node *
NamedNodeMapImpl::setNamedItem(Node * arg)
    throw(DOMException)
{
  if (arg == 0)
    throw DOMException(DOMException::NOT_SUPPORTED_ERR);
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  if (ownerNode != 0 &&
      arg->getOwnerDocument() != ownerNode->getOwnerDocument())
    throw DOMException(DOMException::WRONG_DOCUMENT_ERR);
  
  String * name = arg->getNodeName();
  iterator i = find(name);
  if (i == end()) {
#if 0
    push_back(arg);
#else
    map.insert(Map::value_type(arg->getNodeName(), arg));
#endif
    return 0;
  }
  Node * ret = it(i);
  it(i) = arg;
  return ret;
}

Node *
NamedNodeMapImpl::removeNamedItem(String * name)
  throw(DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);

  iterator i = find(name);
  if (i == end()) {
    throw DOMException(DOMException::NOT_FOUND_ERR);
  }
  Node * ret = it(i);
  map.erase(i);
  return ret;
}

unsigned int
NamedNodeMapImpl::getLength() const
{
#if 0
  return NodeListImpl::getLength();
#else
  return map.size(); 
#endif
}

Node *
NamedNodeMapImpl::item(unsigned int index) const
{
#if 0
  return NodeListImpl::item(index);
#else
  if (index > map.size())
    throw DOMException(DOMException::INDEX_SIZE_ERR);
  const_iterator i = map.begin();
  std::advance(i, index);
  return it(i);
#endif
}

Node *
NamedNodeMapImpl::getNamedItemNS(String * namespaceURI, 
				 String * localName) const
{
  const_iterator i = find(localName, namespaceURI);
  if (i == end())
    return 0;
  return it(i);
}

Node *
NamedNodeMapImpl::setNamedItemNS(Node * arg)
    throw(DOMException)
{
  if (arg == 0)
    throw DOMException(DOMException::NOT_SUPPORTED_ERR);
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  if (ownerNode != 0 &&
      arg->getOwnerDocument() != ownerNode->getOwnerDocument())
    throw DOMException(DOMException::WRONG_DOCUMENT_ERR);
  if (arg->getNodeType() == Node::ATTRIBUTE_NODE &&
      arg->getParentNode() != 0)
    throw DOMException(DOMException::INUSE_ATTRIBUTE_ERR);

  String * localName = arg->getLocalName();
  String * namespaceURI = arg->getNamespaceURI();
  iterator i = find(localName, namespaceURI);
  if (i == end()) {
#if 0
    push_back(arg);
#else
    map.insert(Map::value_type(localName, arg));
#endif
    return 0;
  }
  Node * ret = it(i);
  it(i) = arg;
  return ret;
}

Node *
NamedNodeMapImpl::removeNamedItemNS(String * namespaceURI, 
				    String * localName)
  throw(DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  iterator i = find(localName, namespaceURI);
  if (i == end())
    throw DOMException(DOMException::NOT_FOUND_ERR);
  Node * ret = it(i);
  map.erase(i);
  return ret;
}

NamedNodeMapImpl *
NamedNodeMapImpl::cloneMap(Node * ownerNode) const
{
  NamedNodeMapImpl * newmap = new NamedNodeMapImpl(ownerNode);
  newmap->cloneContent(this);
  return newmap;
}

void
NamedNodeMapImpl::cloneContent(const NamedNodeMapImpl * srcmap)
{
  if (srcmap->getLength() != 0) {
    map.clear();
    for (Map::const_iterator i = srcmap->begin(); i != srcmap->end(); i++) {
      Node * n = dynamic_cast<Node*>(it(i));
      Node * clone = n->cloneNode(true);
      clone->isSpecified(n->isSpecified());
      map.insert(Map::value_type(clone->getNodeName(), clone));
    }
  }
}

void
NamedNodeMapImpl::setReadOnly(bool readOnly, bool deep)
{
  isReadOnly(readOnly);
  if (deep) {
    for (iterator i = begin(); i != end(); i++) {
      Node * n = it(i);
      n->setReadOnly(readOnly, deep);
    }
  }
}

bool
NamedNodeMapImpl::getReadOnly() const
{
  return isReadOnly();
}

void
NamedNodeMapImpl::setOwnerDocument(Document * doc)
{
  for (iterator i = begin(); i != end(); i++) {
    Node * n = it(i);
    n->setOwnerDocument(doc);
  }
}

}
