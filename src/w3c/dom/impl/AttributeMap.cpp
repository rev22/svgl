#include <w3c/dom/Attr.hpp>
#include <w3c/dom/impl/AttributeMap.hpp>
#include <w3c/dom/Document.hpp>
#include <w3c/dom/AttrDef.hpp>
#include <w3c/dom/Element.hpp>

#include <cassert>

namespace dom {

AttributeMap::AttributeMap(Element * ownerNode, const NamedNodeMapImpl * defaults)
  : NamedNodeMapImpl(ownerNode),
    ownerElement(ownerNode)
{
  if (defaults != null) {
    cloneContent(defaults);
    if (getLength() != 0)
      hasDefaults(true);
  }
}

AttributeMap::AttributeMap(const AttributeMap& other)
  : NamedNodeMapImpl(other),
    ownerElement(other.ownerElement)
{
}

Node *
AttributeMap::setNamedItem(Node * arg)
  throw(DOMException)
{
  if (arg->getNodeType() != Node::ATTRIBUTE_NODE)
    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
  Attr * attr = dynamic_cast<Attr*>(arg);
  attr->setOwnerElement(ownerElement);
  attr->addIdRef();
  return NamedNodeMapImpl::setNamedItem(arg);
}

Node *
AttributeMap::setNamedItemNS(Node * arg)
  throw(DOMException)
{
  Attr * attr = dynamic_cast<Attr*>(arg);
  if (attr == null)
    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
  attr->setOwnerElement(ownerElement);
  attr->addIdRef();
  return NamedNodeMapImpl::setNamedItemNS(arg);
}

Node *
AttributeMap::removeNamedItem(String * name)
  throw(DOMException)
{
  return internalRemoveNamedItem(name, true);
}

Node *
AttributeMap::safeRemoveNamedItem(String * name)
{
  return internalRemoveNamedItem(name, false);
}

Node *
AttributeMap::removeNamedItemNS(String * namespaceURI, String * name)
  throw(DOMException)
{
  return internalRemoveNamedItemNS(namespaceURI, name, true);
}

Node *
AttributeMap::safeRemoveNamedItemNS(String * namespaceURI, String * name)
{
    return internalRemoveNamedItemNS(namespaceURI, name, false);
}

NamedNodeMapImpl *
AttributeMap::cloneMap(Node * ownerNode) const
{
  Element * impl = dynamic_cast<Element*>(ownerNode);
  AttributeMap * newmap = impl->getOwnerDocument()->createAttributeMap(impl, null);
  newmap->hasDefaults(hasDefaults());
  newmap->cloneContent(this);
  return newmap;
}
  
void
AttributeMap::reconcileDefaults(NamedNodeMapImpl * defaults)
{
  for (const_iterator i = defaults->begin(); i != defaults->end(); i++) {
    AttrDef * def = dynamic_cast<AttrDef*>(it(i));
#if 0
    assert(def!=0);
#else
    if(!def)
      continue;
#endif
    if (def->getValue() != null) {
      Node * attr = getNamedItemNS(def->getNamespaceURI(),
				   def->getLocalName());
      if (attr == null) {
	attr = ownerNode->getOwnerDocument()->createAttributeNS(def->getNamespaceURI(),
						  def->getLocalName());
	attr->setNodeValue(def->getValue());
	setNamedItemNS(attr);
      }
    }
  }
}

Node *
AttributeMap::internalRemoveNamedItem(String * name, bool raiseEx)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);

  iterator i = find(name);
  if (i == end()) {
    if (raiseEx)
      throw DOMException(DOMException::NOT_FOUND_ERR);
    else
      return null;
  }
  Attr * n = dynamic_cast<Attr*>(it(i));
  if (hasDefaults()) {
    //+++ FIXME
  }
  else {
    map.erase(i);
  }
  n->removeIdRef();
  n->setOwnerElement(null);
  return n;
}

Node *
AttributeMap::internalRemoveNamedItemNS(String * namespaceURI, String * localName, bool raiseEx)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);

  iterator i = find(localName, namespaceURI);
  if (i == end()) {
    if (raiseEx)
      throw DOMException(DOMException::NOT_FOUND_ERR);
    else
      return null;
  }
  Attr * n = dynamic_cast<Attr*>(it(i));
  if (hasDefaults()) {
    //+++ FIXME
  }
  else {
    map.erase(i);
  }
  n->removeIdRef();
  n->setOwnerElement(null);
  return n;
}

void
AttributeMap::cloneContent(const NamedNodeMapImpl * srcmap)
{
  if (srcmap->getLength() == 0)
    return;
  for (const_iterator i = srcmap->begin(); i != srcmap->end(); i++) {
    Attr * n = dynamic_cast<Attr*>(it(i));
    Attr * clone = dynamic_cast<Attr*>(n->cloneNode(true));
    clone->isSpecified(n->isSpecified());
    map.insert(Map::value_type(clone->getNodeName(), clone));
    clone->setOwnerElement(ownerElement);
  }
}

}
