#ifndef _dom_DOMVisitor_hpp_
#define _dom_DOMVisitor_hpp_

#include <w3c/dom/dom.hpp>

namespace dom {

class DOMVisitor {
public:
  DOMVisitor();
  virtual ~DOMVisitor();

  virtual void visitNode(Node * node);
  virtual void visitChild(Node * node);
  virtual void visitChildren(const NodeList * nodeList);
  virtual void visitNamedNodeMap(NamedNodeMap * namedNodeMap);

  virtual void visitElement(Element * node);
  virtual void visitAttr(Attr * node);
  virtual void visitText(Text * node);
  virtual void visitCDATASection(CDATASection * node);
  virtual void visitEntityReference(EntityReference * node);
  virtual void visitEntity(Entity * node);
  virtual void visitProcessingInstruction(ProcessingInstruction * node);
  virtual void visitComment(Comment * node);
  virtual void visitDocument(Document * node);
  virtual void visitDocumentType(DocumentType * node);
  virtual void visitDocumentFragment(DocumentFragment * node);
  virtual void visitNotation(Notation * node);
  virtual void visitUnknown(Node * node);
};

}

#endif
