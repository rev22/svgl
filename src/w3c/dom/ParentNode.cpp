#include <w3c/dom/ParentNode.hpp>
#include <w3c/dom/Document.hpp>

namespace dom {
ParentNode::ParentNode(Document * o)
  : ChildNode(o),
    ownerDoc(o),
    firstChild(null)
{}

ParentNode::ParentNode(const ParentNode& other)
  : ChildNode(other),
    ownerDoc(other.ownerDoc),
    firstChild(null)
{
}

Document *
ParentNode::getOwnerDocument() const
{
  return ownerDoc;
}

void
ParentNode::setOwnerDocument(Document * doc)
{
  ChildNode::setOwnerDocument(doc);
  ownerDoc = doc;
  for (ChildNode * child = firstChild;
       child != null; child = child->nextSibling) {
    child->setOwnerDocument(doc);
  }
}

bool
ParentNode::hasChildNodes() const
{
  return firstChild != null;
}

const NodeList *
ParentNode::getChildNodes() const
{
  return this;
}

Node *
ParentNode::getFirstChild() const
{
  return firstChild;
}

Node *
ParentNode::getLastChild() const
{
  return lastChild();
}

Node *
ParentNode::insertBefore(Node * newChild, Node * refChild)
    throw(DOMException)
{
  return internalInsertBefore(newChild,refChild);
}

Node *
ParentNode::internalInsertBefore(Node * newChild, Node * refChild)
    throw(DOMException)
{
  if (newChild->getNodeType() == Node::DOCUMENT_FRAGMENT_NODE) {
    while (newChild->hasChildNodes()) {
      insertBefore(newChild->getFirstChild(), refChild);
    }
    return newChild;
  }
  if (newChild == refChild) {
    // stupid case that must be handled as a no-op triggering events...
    refChild = refChild->getNextSibling();
    removeChild(newChild);
    insertBefore(newChild, refChild);
    return newChild;
  }
  // Convert to internal type, to avoid repeated casting
  ChildNode * newInternal = dynamic_cast<ChildNode*>(newChild);

  Node * oldparent = newInternal->getParentNode();
  if (oldparent != null) {
    oldparent->removeChild(newInternal);
  }

  // Convert to internal type, to avoid repeated casting
  ChildNode * refInternal = dynamic_cast<ChildNode*>(refChild);

  // Attach up
  newInternal->ownerNode = this;
  newInternal->isOwned(true);

  // Attach before and after
  // Note: firstChild.previousSibling == lastChild!!
  if (firstChild == null) {
    // this our first and only child
    firstChild = newInternal;
    newInternal->isFirstChild(true);
    newInternal->previousSibling = newInternal;
  }
  else {
    if (refInternal == null) {
      // this is an append
      ChildNode * lastChild = firstChild->previousSibling;
      lastChild->nextSibling = newInternal;
      newInternal->previousSibling = lastChild;
      firstChild->previousSibling = newInternal;
    }
    else {
      // this is an insert
      if (refChild == firstChild) {
	// at the head of the list
	firstChild->isFirstChild(false);
	newInternal->nextSibling = firstChild;
	newInternal->previousSibling = firstChild->previousSibling;
	firstChild->previousSibling = newInternal;
	firstChild = newInternal;
	newInternal->isFirstChild(true);
      }
      else {
	// somewhere in the middle
	ChildNode * prev = refInternal->previousSibling;
	newInternal->nextSibling = refInternal;
	prev->nextSibling = newInternal;
	refInternal->previousSibling = newInternal;
	newInternal->previousSibling = prev;
      }
    }
  }
  if (getOwnerDocument())
    getOwnerDocument()->putIdentifiers(newInternal);
  changed();
  return newChild;
}

Node *
ParentNode::removeChild(Node * oldChild)
  throw(DOMException)
{
  return internalRemoveChild(oldChild);
}
  
Node *
ParentNode::internalRemoveChild(Node * oldChild)
  throw(DOMException)
{

  Document * ownerDoc = getOwnerDocument();

  ChildNode * oldInternal = dynamic_cast<ChildNode*>(oldChild);

  // Patch linked list around oldChild
  // Note: lastChild == firstChild.previousSibling
  if (oldInternal == firstChild) {
    // removing first child
    oldInternal->isFirstChild(false);
    firstChild = oldInternal->nextSibling;
    if (firstChild != null) {
      firstChild->isFirstChild(true);
      firstChild->previousSibling = oldInternal->previousSibling;
    }
  } else {
    ChildNode * prev = oldInternal->previousSibling;
    ChildNode * next = oldInternal->nextSibling;
    prev->nextSibling = next;
    if (next == null) {
      // removing last child
      firstChild->previousSibling = prev;
    } else {
      // removing some other child in the middle
      next->previousSibling = prev;
    }
  }

  // Save previous sibling for normalization checking.
  //ChildNode * oldPreviousSibling = oldInternal->_previousSibling();

  // Remove oldInternal's references to tree
  oldInternal->ownerNode       = ownerDoc;
  oldInternal->isOwned(false);
  oldInternal->nextSibling     = null;
  oldInternal->previousSibling = null;
  Document * ownerDocTmp = getOwnerDocument();

  if(ownerDocTmp)
    ownerDocTmp->removeIdentifiers(oldInternal);
  changed();

  return oldInternal;
}

Node *
ParentNode::replaceChild(Node * newChild, Node * oldChild)
  throw(DOMException)
{
  internalInsertBefore(newChild, oldChild);
  if (newChild != oldChild) {
    internalRemoveChild(oldChild);
  }
  return oldChild;
}

unsigned int
ParentNode::getLength() const
{
  unsigned int length = 0;
  for (ChildNode * node = firstChild; node != null; node = node->nextSibling) {
    length++;
  }
  return length;
}

Node *
ParentNode::item(unsigned int index) const
{
  ChildNode * node;
  for (node = firstChild; node != null && index-- != 0; node = node->nextSibling)
    ;
  return node;
}

void
ParentNode::normalize()
{
  for (ChildNode * kid = firstChild; kid != null; kid = kid->nextSibling) {
    kid->normalize();
  }
}

void
ParentNode::setReadOnly(bool readOnly, bool deep)
{
  ChildNode::setReadOnly(readOnly, deep);

  if (deep) {
    for (ChildNode * mykid = firstChild;
	 mykid != null;
	 mykid = mykid->nextSibling) {
      if (mykid->getNodeType() != Node::ENTITY_REFERENCE_NODE) {
	mykid->setReadOnly(readOnly,true);
      }
    }
  }
}
}
