#include <w3c/dom/impl/NodeListImpl.hpp>
#include <w3c/dom/DOMException.hpp>

namespace dom {

NodeList::~NodeList() {}

NodeListImpl::NodeListImpl()
{}

NodeListImpl::NodeListImpl(const NodeList& nl)
{
  append(&nl);
}

void
NodeListImpl::append(const NodeList * nl)
{
  for (unsigned int i = 0; i < nl->getLength(); i++)
    append(nl->item(i));
}

void
NodeListImpl::append(Node * node)
{
  if (node == 0)
    throw DOMException(DOMException::INVALID_MODIFICATION_ERR);
  push_back(node);
}

void
NodeListImpl::insert(unsigned int index, Node * node)
{
  if (node == 0 || index >= size())
    throw DOMException(DOMException::INVALID_MODIFICATION_ERR);
  super::insert(begin()+index,node);
}

void
NodeListImpl::remove(unsigned int index)
{
  erase(begin()+index);
}

EmptyNodeList * EmptyNodeList::singleton;

EmptyNodeList::EmptyNodeList()
{
}

unsigned int
EmptyNodeList::getLength() const
{
  return 0;
}
Node *
EmptyNodeList::item(unsigned int) const
{
  return null;
}

EmptyNodeList * 
EmptyNodeList::instance()
{
  if (singleton == null)
    singleton = new EmptyNodeList();
  return singleton;
}

}
