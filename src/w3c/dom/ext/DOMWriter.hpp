#ifndef _dom_DOMWriter_hpp_
#define _dom_DOMWriter_hpp_

#include <iostream>
#include <w3c/dom/ext/DOMVisitor.hpp>

namespace dom {

class DOMWriter : public DOMVisitor {
protected:
  std::ostream& out;
public:
  DOMWriter(std::ostream& out);

  virtual void write(Node * node);
  virtual void writeAttrText(String * str);
  virtual void writeAttrString(String * str);
  virtual void writeDocString(String * str);

  virtual void visitElement(Element * node);
  virtual void visitAttr(Attr * node);
  virtual void visitText(Text * node);
  virtual void visitProcessingInstruction(ProcessingInstruction * node);
  virtual void visitComment(Comment * node);
  virtual void visitDocument(Document * node);
  virtual void visitDocumentType(DocumentType * node);
};

}

#endif
