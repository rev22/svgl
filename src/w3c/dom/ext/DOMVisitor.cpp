#include <w3c/dom/ext/DOMVisitor.hpp>

namespace dom {

DOMVisitor::DOMVisitor()
{
}

DOMVisitor::~DOMVisitor()
{
}

void 
DOMVisitor::visitNode(Node * node)
{
  switch(node->getNodeType()) {
#define defineNode(TYPE,CLASS) \
  case Node::TYPE:\
    visit##CLASS(dynamic_cast<CLASS*>(node));\
    break;
#include <w3c/dom/NODES>
#undef defineNode
  default:
    visitUnknown(node);
    break;
  }
}

void
DOMVisitor::visitChild(Node * node)
{
  visitNode(node);
}

void
DOMVisitor::visitChildren(const NodeList * nodeList)
{
  if (nodeList == null)
    return;
  for (unsigned int i = 0; i < nodeList->getLength(); i++)
    visitChild(nodeList->item(i));
}

void
DOMVisitor::visitNamedNodeMap(NamedNodeMap * namedNodeMap)
{
  if (namedNodeMap == null)
    return;
  for (unsigned int i = 0; i < namedNodeMap->getLength(); i++)
    visitChild(namedNodeMap->item(i));
}

void 
DOMVisitor::visitElement(Element * node)
{
  visitNamedNodeMap(node->getAttributes());
  visitChildren(node->getChildNodes());
}

void 
DOMVisitor::visitAttr(Attr * node)
{
  visitChildren(node->getChildNodes());
}

void 
DOMVisitor::visitText(Text * node)
{
}

void
DOMVisitor::visitCDATASection(CDATASection * node)
{
}

void
DOMVisitor::visitEntityReference(EntityReference * node)
{
}

void
DOMVisitor::visitEntity(Entity * node)
{
}

void 
DOMVisitor::visitProcessingInstruction(ProcessingInstruction * node)
{
}

void 
DOMVisitor::visitComment(Comment * node)
{
}

void 
DOMVisitor::visitDocument(Document * node)
{
  visitChildren(node->getChildNodes());
}

void 
DOMVisitor::visitDocumentType(DocumentType * node)
{
}

void
DOMVisitor::visitDocumentFragment(DocumentFragment * node)
{
}

void
DOMVisitor::visitNotation(Notation * node)
{
}

void 
DOMVisitor::visitUnknown(Node * node)
{
}

}
