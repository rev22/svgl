#include <w3c/dom/AttrDef.hpp>

namespace dom {
AttrDef::AttrDef(Document * ownerDocument,
			 String * name,
			 AttributeType type,
			 bool req,
			 const StringList& value)
  : Attr(ownerDocument, name),
    attrType(type),
    enumValues(value),
    required(req)
{
}

AttrDef::AttrDef(Document * ownerDocument,
			 String * namespaceURI,
			 String * qualifiedName,
			 AttributeType type,
			 bool req,
			 const StringList& value)
  : Attr(ownerDocument, namespaceURI, qualifiedName),
    attrType(type),
    enumValues(value),
    required(req)
{
}

AttrDef::AttrDef(const AttrDef& other)
  : Attr(other),
    attrType(other.attrType),
    enumValues(other.enumValues)
{
}

AttrDef::~AttrDef()
{
}

Node *
AttrDef::cloneNode(bool deep) const
{
  return new AttrDef(*this);
}


AttrDef::AttributeType
AttrDef::getAttributeType() const
{
  return attrType;
}

unsigned int
AttrDef::getEnumLength() const
{
  return enumValues.getLength();
}

String *
AttrDef::getEnumValue(unsigned int index) const
{
  return enumValues.item(index);
}

bool
AttrDef::isRequired() const
{
  return required;
}

}
