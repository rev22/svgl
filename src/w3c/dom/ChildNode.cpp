#include <w3c/dom/ChildNode.hpp>

namespace dom {

ChildNode::ChildNode(Document * ownerDocument)
  : Node(ownerDocument),
    previousSibling(0),
    nextSibling(0)
{
}

ChildNode::ChildNode(const ChildNode& other)
  : Node(other),
    previousSibling(0),
    nextSibling(0)
{
  isFirstChild(false);
}

Node *
ChildNode::getParentNode() const
{
  return isOwned() ? ownerNode : null;
}

Node *
ChildNode::getNextSibling() const
{
  return nextSibling;
}

Node *
ChildNode::getPreviousSibling() const
{
  return isFirstChild() ? 0 : previousSibling;
}

void
ChildNode::setFirst()
{
  Node * parent = getParentNode();
  if(!parent)
    return;
  parent->removeChild(this);
  parent->insertBefore(this, parent->getFirstChild());
}

void
ChildNode::setLast()
{
  Node * parent = getParentNode();
  if(!parent)
    return;
  parent->removeChild(this);
  parent->insertBefore(this, 0);
}

void
ChildNode::setBefore(ChildNode* n) throw (DOMException)
{
  if(this==n)
    return;
  Node * parent = getParentNode();
  if(!parent)
    return;
  Node * parentn = n->getParentNode();
  if(parent != parentn)
    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);

  parent->removeChild(this);
  parent->insertBefore(this,n);  
}

void
ChildNode::setAfter(ChildNode* n) throw (DOMException)
{
  if(this==n)
    return;
  Node * parent = getParentNode();
  if(!parent)
    return;
  Node * parentn = n->getParentNode();
  if(parent != parentn)
    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);

  parent->removeChild(this);
  Node * n1 = n->getNextSibling();
  parent->insertBefore(this,n1);  
}


}
