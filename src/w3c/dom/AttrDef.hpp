#ifndef _dom_AttrDefImpl_hpp
#define _dom_AttrDefImpl_hpp

#include <utils/StringList.hpp>
#include <w3c/dom/Attr.hpp>

namespace dom {

class AttributeMap;
class Document;
class Text;
class ChildNode;

class AttrDef : public Attr
{
public:
    enum AttributeType {
	CDATA_ATTR_TYPE,
	ID_ATTR_TYPE,
	IDREF_ATTR_TYPE,
	IDREFS_ATTR_TYPE,
	NMTOKEN_ATTR_TYPE,
	NMTOKENS_ATTR_TYPE,
	ENTITY__ATTR_TYPE,
	ENTITIES_ATTR_TYPE,
	NOTATION_ATTR_TYPE,
	ENUM_ATTR_TYPE
    };
  				AttrDef(Document * ownerDocument,
					String * name,
					AttributeType type,
					bool required,
					const StringList& value = StringList());
  				AttrDef(Document * ownerDocument,
					String * namespaceURI,
					String * qualifiedName,
					AttributeType type,
					bool required,
					const StringList& value = StringList());
  				AttrDef(const AttrDef& other);
    virtual			~AttrDef();

    virtual Node *		cloneNode(bool deep) const;
    virtual AttributeType	getAttributeType() const;
    virtual unsigned int	getEnumLength() const;
    virtual String *		getEnumValue(unsigned int index) const;
    virtual bool		isRequired() const;
protected:
    AttributeType		attrType;
    StringList			enumValues;
    bool			required;
};
}

#endif
