#include <w3c/dom/Attr.hpp>
#include <w3c/dom/impl/AttributeMap.hpp>
#include <w3c/dom/CDATASection.hpp>
#include <w3c/dom/Comment.hpp>
#include <w3c/dom/Document.hpp>
#include <w3c/dom/DocumentType.hpp>
#include <w3c/dom/DOMImplementation.hpp>
#include <w3c/dom/Element.hpp>
#include <w3c/dom/ProcessingInstruction.hpp>
#include <w3c/dom/Text.hpp>

namespace dom {

Document::Document(const Document& other)
  : ParentNode(other),
    docType(null),
    docElement(null),
    encoding(other.encoding),
    changes_(0)
{
  ownerDoc = this;
  // docElement = other.docElement->cloneNode(true); +++ fixme
}
    
Document::Document()
  : ParentNode(0),
    docType(0),
    docElement(0),
    encoding(0),
    changes_(0)
{
  ownerDoc = this;
//   if (docType != null) {
//     docType->ownerDoc = this;
//   }
}

void
Document::changed()
{
  changes_++;
}

int
Document::changes() const
{
  return changes_;
}

Document *
Document::getOwnerDocument() const
{
  return null;
}

Node::NodeType
Document::getNodeType() const
{
  return Node::DOCUMENT_NODE;
}

String *
Document::getNodeName() const
{
  static String * document_string=0;
  if (document_string == 0)
    document_string = String::internString("#document");
  return document_string;
}

Node *
Document::cloneNode(bool deep) const
{
  if (deep) {
    Document * newdoc = new Document(*this);
    // ++ FIXME copy the nodes
    return newdoc;
  }
  else
    return new Document();
}

Node *
Document::insertBefore(Node * newChild, Node * refChild)
  throw(DOMException)
{
  NodeType type = newChild->getNodeType();
  if ((type == Node::ELEMENT_NODE && docElement != null) ||
      (type == Node::DOCUMENT_TYPE_NODE && docType != null))
    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
  ParentNode::insertBefore(newChild, refChild);
  if (type == Node::ELEMENT_NODE)
    docElement = dynamic_cast<Element*>(newChild);
  else if (type == Node::DOCUMENT_TYPE_NODE) {
    docType = dynamic_cast<DocumentType*>(newChild);
    docType->setOwnerDocument(this);
  }
  return newChild;
}

Node *
Document::removeChild(Node * oldChild)
    throw(DOMException)
{
  ParentNode::removeChild(oldChild);
  NodeType type = oldChild->getNodeType();
  if (type == Node::ELEMENT_NODE)
    docElement = null;
  else if (type == Node::DOCUMENT_TYPE_NODE)
    docType = null;
  return oldChild;
}

Node *
Document::replaceChild(Node * newChild, Node * oldChild)
    throw(DOMException)
{
  ParentNode::replaceChild(newChild, oldChild);

  NodeType type = oldChild->getNodeType();
  if (type == Node::ELEMENT_NODE)
    docElement = dynamic_cast<Element*>(newChild);
  else if (type == Node::DOCUMENT_TYPE_NODE)
    docType = dynamic_cast<DocumentType*>(newChild);
  return newChild;
}

DocumentType *
Document::getDoctype() const
{
  return docType;
}

DOMImplementation *
Document::getImplementation() const
{
  return DOMImplementation::getDOMImplementation();
}

Element *
Document::getDocumentElement() const
{
  return docElement;
}

Element *
Document::createElement(String * tagName)
    throw(DOMException)
{
  return new Element(this, tagName);
}

DocumentFragment *
Document::createDocumentFragment()
{
  throw DOMException(DOMException::NOT_SUPPORTED_ERR);
}

Text *
Document::createTextNode(String * data)
{
  return new Text(this, data);
}

Comment *
Document::createComment(String * data)
{
  return new Comment(this, data);
}

CDATASection *
Document::createCDATASection(String * data)
  throw(DOMException)
{
  return new CDATASection(this, data);
}

ProcessingInstruction *
Document::createProcessingInstruction(String * target, 
					  String * data)
  throw(DOMException)
{
  return new ProcessingInstruction(this, target, data);
}

Attr *
Document::createAttribute(String * name)
    throw(DOMException)
{
  return new Attr(this, name);
}

EntityReference *
Document::createEntityReference(String * name)
  throw(DOMException)
{
  throw DOMException(DOMException::NOT_SUPPORTED_ERR);
}

NodeList *
Document::getElementsByTagName(String * tagname) const
{
  throw DOMException(DOMException::NOT_SUPPORTED_ERR);
}

Node *
Document::importNode(Node * importedNode, 
			 bool deep)
    throw(DOMException)
{
  throw DOMException(DOMException::NOT_SUPPORTED_ERR);
}
Element *
Document::createElementNS(String * namespaceURI, 
					String * qualifiedName)
    throw(DOMException)
{
  return new Element(this, namespaceURI, qualifiedName);
}
Attr *
Document::createAttributeNS(String * namespaceURI, 
					  String * qualifiedName)
    throw(DOMException)
{
  return new Attr(this, namespaceURI, qualifiedName);
}

NodeList *
Document::getElementsByTagNameNS(String * namespaceURI, 
					       String * localName)
{
  throw DOMException(DOMException::NOT_SUPPORTED_ERR);
}

Element *
Document::getElementById(String * elementId) const
{
  //throw DOMException(DOMException::NOT_SUPPORTED_ERR);
  return getIdentifier(elementId);
}

void
Document::putIdentifier(String * idName, Element * element)
{
  if (idName == null)
    return;
  Ids::iterator i = identifiers.find(idName);
  if (i == identifiers.end())
    identifiers.insert(Ids::value_type(idName, element));
  else i->second = element;
}

Element *
Document::getIdentifier(String * idName) const
{
  Ids::const_iterator i = identifiers.find(idName);
  if (i == identifiers.end())
    return null;
  return i->second;
}

void
Document::removeIdentifier(String * idName)
{
  if (idName == null)
    return;
  Ids::iterator i = identifiers.find(idName);
  if (i != identifiers.end())
    identifiers.erase(i);
}

void
Document::rebuildIdentifiers()
{
  identifiers.erase(identifiers.begin(),identifiers.end());
  putIdentifiers(docElement);
}

void
Document::putIdentifiers(const Node * node)
{
  if (node->getNodeType() == ELEMENT_NODE) {
    const Element * e = dynamic_cast<const Element*>(node);
    AttributeMap * attributes = e->getAttributesImpl();
    for (AttributeMap::iterator i = attributes->begin();
	 i != attributes->end(); i++) {
      putIdentifiers(AttributeMap::it(i));
    }
    for (Node * child = node->getFirstChild();
	 child != null;
	 child = child->getNextSibling()) {
      putIdentifiers(child);
    }
  }
  else if (node->getNodeType() == ATTRIBUTE_NODE) {
    const Attr * attr = dynamic_cast<const Attr*>(node);
    attr->addIdRef();
  }
}

void
Document::removeIdentifiers(const Node * node)
{
  if (node->getNodeType() == ELEMENT_NODE) {
    const Element * e = dynamic_cast<const Element*>(node);
    AttributeMap * attributes = e->getAttributesImpl();
    for (AttributeMap::iterator i = attributes->begin();
	 i != attributes->end(); i++) {
      removeIdentifiers(AttributeMap::it(i));
    }
    for (Node * child = node->getFirstChild();
	 child != null;
	 child = child->getNextSibling()) {
      removeIdentifiers(child);
    }
  }
  else if (node->getNodeType() == ATTRIBUTE_NODE) {
    const Attr * attr = dynamic_cast<const Attr*>(node);
    attr->removeIdRef();
  }
}

void
Document::replacedText(Node * node)
{
}

void
Document::deletedText(Node * node,
			  unsigned int offset,
			  unsigned int count)
{
}

void
Document::insertedText(Node * node,
			   unsigned int offset,
			   unsigned int count)
{
}

void
Document::splitData(Node * node,
			Node * newNode,
			unsigned int offset)
{
}

void
Document::removedChildNode(Node * oldChild)
{
}

AttributeMap *
Document::createAttributeMap(Element * e,
			     NamedNodeMapImpl * defaults)
{
  return new AttributeMap(e, defaults);
}

}
