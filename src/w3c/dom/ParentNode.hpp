#ifndef _dom_ParentNode_hpp_
#define _dom_ParentNode_hpp_

#include <w3c/dom/ChildNode.hpp>
#include <w3c/dom/NodeList.hpp>

namespace dom {
class ParentNode : public ChildNode,
		   public NodeList {
protected:
  Document *		ownerDoc;
  ChildNode *		firstChild;
  			ParentNode(Document * ownerDocument);
  			ParentNode(const ParentNode& other);
public:
  virtual Document *	getOwnerDocument() const;
  virtual void		setOwnerDocument(Document * doc);
  virtual bool		hasChildNodes() const;
  virtual const NodeList *	getChildNodes() const;
  virtual Node *	getFirstChild() const;
  virtual Node *	getLastChild() const;
  ChildNode *		lastChild() const {
    return firstChild != 0 ? firstChild->_previousSibling() : 0;
  }
  void			lastChild(ChildNode * node) {
    if (firstChild != 0) {
      firstChild->previousSibling = node;
    }
  }
  virtual Node *	insertBefore(Node * newChild, Node * refChild)
    throw(DOMException);
  virtual Node *	internalInsertBefore(Node * newChild, Node * refChild)
    throw(DOMException);
  virtual Node *	removeChild(Node * oldChild)
    throw(DOMException);
  virtual Node *	internalRemoveChild(Node * oldChild)
    throw(DOMException);
  virtual Node *	replaceChild(Node * newChild, Node * oldChild)
    throw(DOMException);
  virtual unsigned int	getLength() const;
  virtual Node *	item(unsigned int index) const;
  virtual void		normalize();
  virtual void		setReadOnly(bool readOnly, bool deep);
};

}

#endif
