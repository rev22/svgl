#ifndef _dom_ChildNode_hpp_
#define _dom_ChildNode_hpp_

#include <w3c/dom/Node.hpp>

namespace dom {
class Attr;
class ParentNode;

class ChildNode : public Node {
protected:
  ChildNode *		previousSibling;
  ChildNode *		nextSibling;

  			ChildNode(Document * ownerDocument);
  			ChildNode(const ChildNode& other);
public:
  virtual Node *	getParentNode() const;
  virtual Node *	getNextSibling() const;
  virtual Node *	getPreviousSibling() const;
  ChildNode *		_previousSibling() const {
    return isFirstChild() ? 0 : previousSibling;
  }

  void setFirst();
  void setLast();
  void setBefore(ChildNode*) throw (DOMException);
  void setAfter(ChildNode*) throw (DOMException);

  friend class ParentNode;
  friend class Attr;
  friend class Element;
};

}

#endif
