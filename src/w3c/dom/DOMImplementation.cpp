#include <ctype.h>
#include <w3c/dom/DOMImplementation.hpp>
#include <w3c/dom/Document.hpp>
#include <w3c/dom/DocumentType.hpp>

namespace dom {

DOMImplementation::DOMImplementation()
{
}

DOMImplementation::~DOMImplementation()
{
}

DOMImplementation *DOMImplementation::singleton;

void
DOMImplementation::setDOMImplementation(DOMImplementation * impl)
{
  singleton = impl;
}

DOMImplementation *
DOMImplementation::getDOMImplementation()
{
  if (singleton == null)
    setDOMImplementation(new DOMImplementation());
  return singleton;
}

static bool equalsIgnoreCase(const char * str1, String * str2)
{
  unsigned int i;
  for (i = 0; i < str2->getLength() && *str1 != 0; i++, str1++) {
    String::char_type c1 = str2->charAt(i);
    String::char_type c2 = *str1;
    if (c1 == c2 ||
	(islower(c2) && c1 == String::char_type(toupper(c2))) ||
	(isupper(c2) &&	c1 == String::char_type(tolower(c2))))
      ;
    else
      return false;
  }
  if (*str1 != 0 || i != str2->getLength())
    return false;
  return true;
}

bool
DOMImplementation::hasFeature(String * feature,
				  String * version) const
{
  bool anyVersion = ((version == null) || (version->getLength() == 0));
  return 
    (equalsIgnoreCase("Core", feature) 
     && (anyVersion
	 || version->equals("1.0")
	 || version->equals("2.0")))
    || (equalsIgnoreCase("XML", feature)
	&& (anyVersion
	    || version->equals("1.0")
	    || version->equals("2.0")))
//     || (equalsIgnoreCase("Events",feature) 
// 	&& (anyVersion
// 	    || version->equals("2.0")))
//     || (equalsIgnoreCase("MutationEvents",feature) 
// 	&& (anyVersion
// 	    || version->equals("2.0")))
//     || (equalsIgnoreCase("Traversal",feature) 
// 	&& (anyVersion
// 	    || version->equals("2.0")))
    ;
}

static inline bool isXMLName(String * ) { return true; }// ++FIXME

DocumentType *
DOMImplementation::createDocumentType(String * qualifiedName, 
					  String * publicId, 
					  String * systemId)
  throw(DOMException)
{
  if (! isXMLName(qualifiedName))
    throw DOMException(DOMException::INVALID_CHARACTER_ERR);
  String::size_type index = qualifiedName->indexOf(':');
  if (index == 0 || index == qualifiedName->getLength() - 1)
    throw DOMException(DOMException::NAMESPACE_ERR);
  for (ProtoList::const_iterator i = prototypes.begin();
       i != prototypes.end(); i++) {
    DocumentType * doctype = (*i)->getDoctype();
    if (doctype->getName()->equals(qualifiedName) &&
	doctype->getSystemId()->equals(systemId) &&
	(doctype->getPublicId() == 0 ||
	 doctype->getPublicId()->equals(publicId))) {
      doctype = dynamic_cast<DocumentType*>(doctype->cloneNode(false));
      doctype->setOwnerDocument(0);
      return doctype;
    }
  }
  return new DocumentType(null, qualifiedName, publicId, systemId);
}

DocumentType *
DOMImplementation::createDocumentType(String * qualifiedName )
  throw(DOMException)
{
  if (! isXMLName(qualifiedName))
    throw DOMException(DOMException::INVALID_CHARACTER_ERR);
  String::size_type index = qualifiedName->indexOf(':');
  if (index == 0 || index == qualifiedName->getLength() - 1)
    throw DOMException(DOMException::NAMESPACE_ERR);
  for (ProtoList::const_iterator i = prototypes.begin();
       i != prototypes.end(); i++) {
    DocumentType * doctype = (*i)->getDoctype();
    if (doctype->getName()->equals(qualifiedName)) {
      doctype = dynamic_cast<DocumentType*>(doctype->cloneNode(false));
      doctype->setOwnerDocument(0);
      return doctype;
    }
  }
  return new DocumentType(null, qualifiedName, 0, 0);
}

#if 0
Document *
DOMImplementation::createDocument(String * namespaceURI, 
				      String * qualifiedName, 
				      DocumentType * doctype)
    throw(DOMException)
{
  if (doctype != null && doctype->getOwnerDocument() != null)
    throw DOMException(DOMException::WRONG_DOCUMENT_ERR);
  Document * doc = null;
  for (ProtoList::const_iterator i = prototypes.begin();
       doctype != null && i != prototypes.end(); i++) {
    Document * proto = *i;
    if (proto->getDoctype()->getSystemId() == doctype->getSystemId() &&
	proto->getDoctype()->getPublicId() == doctype->getPublicId()) {
      doc = dynamic_cast<Document*>(proto->cloneNode(false));
      break;
    }
  }
  if (doc == null) {
    doc = new Document();
  }
  if (doctype != null)
    doc->appendChild(doctype);
  return doc;
}
#endif

Document *
DOMImplementation::createDocument(String * namespaceURI, 
				      String * qualifiedName, 
				      DocumentType * doctype)
    throw(DOMException)
{
  if (doctype != null && doctype->getOwnerDocument() != null)
    throw DOMException(DOMException::WRONG_DOCUMENT_ERR);
  Document * doc = null;
  for (ProtoList::const_iterator i = prototypes.begin();
       doctype != null && i != prototypes.end(); i++) {
    Document * proto = *i;
    {
      doc = dynamic_cast<Document*>(proto->cloneNode(false));
      break;
    }
  }
  if (doc == null) {
    doc = new Document();
  }
  if (doctype != null)
    doc->appendChild(doctype);
  return doc;
}

void
DOMImplementation::registerPrototype(Document * proto)
{
  if (proto != null || proto->getDoctype() == null) {
    prototypes.push_back(proto);
  }
}

}
