#include <w3c/dom/ext/DOMWriter.hpp>

namespace dom {
DOMWriter::DOMWriter(std::ostream& o)
  : out(o)
{}

void
DOMWriter::write(Node * node)
{
  visitNode(node);
}

void
DOMWriter::writeAttrText(String * name)
{
  print_quoted(out, name);
}

void
DOMWriter::writeAttrString(String * value)
{
  if(!value) {
    out << "\"\"";
  }
  else if (value->indexOf('"') < 0) {
    out << '"';
    writeAttrText(value);
    out << '"';
  }
  else {
    out << '\'';
    writeAttrText(value);
    out << '\'';
  }
}

void
DOMWriter::writeDocString(String * name)
{
  print_quoted(out, name);
}



void 
DOMWriter::visitElement(Element * node)
{
  out << '<' << node->getNodeName();
  visitNamedNodeMap(node->getAttributes());
  if (node->hasChildNodes()) {
    out << '>';
    visitChildren(node->getChildNodes());
    out << '<' << '/' << node->getNodeName() << '>';
  }
  else {
    out << '/' << '>';
  }
}

void 
DOMWriter::visitAttr(Attr * node)
{
  if (node->getSpecified()) {
    out << ' ' << node->getNodeName() << '=';
    writeAttrString(node->getNodeValue());
  }
}

void 
DOMWriter::visitText(Text * node)
{
  writeDocString(node->getNodeValue());
}

void 
DOMWriter::visitProcessingInstruction(ProcessingInstruction * node)
{
  out << '<' << '?' << node->getNodeName() << ' ';
  String * value = node->getNodeValue();
  if (value != null) {
    out << "type=";
    writeAttrString(node->getNodeValue());
  }
  out << '?' << '>';
}

void 
DOMWriter::visitComment(Comment * node)
{
  out << "<!--" << node->getNodeValue() << "-->";
}

void 
DOMWriter::visitDocument(Document * node)
{
  //visitDocumentType(node->getDoctype());
  DOMVisitor::visitChildren(node->getChildNodes());
}

void 
DOMWriter::visitDocumentType(DocumentType * node)
{
  if (node == null)
    return;
  String * pubId = node->getPublicId();
  String * sysId = node->getSystemId();

  if (pubId != null && sysId != null) {
    out << "<!DOCTYPE " << node->getName();
    out << " PUBLIC ";
    writeAttrString(pubId);
    out << ' ';
    writeAttrString(sysId);
    out << '>' << std::endl;
  }
  else if (sysId != null) {
    out << "<!DOCTYPE " << node->getName();
    out << " SYSTEM ";
    writeAttrString(sysId);
    out << '>' << std::endl;
  }
}



}
