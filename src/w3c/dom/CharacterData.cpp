#include <w3c/dom/CharacterData.hpp>
#include <w3c/dom/impl/NodeListImpl.hpp>
#include <utils/StringBuffer.hpp>

namespace dom {

CharacterData::CharacterData(Document * ownerDocument,
			     String * d)
  : ChildNode(ownerDocument),
    data(d)
{
}

CharacterData::CharacterData(const CharacterData& other)
  : ChildNode(other),
    data(other.data)
{
}

const NodeList *
CharacterData::getChildNodes() const
{
  return EmptyNodeList::instance();
}

String *
CharacterData::getNodeValue() const throw (dom::DOMException)
{
  return data;
}

void
CharacterData::setNodeValueInternal(String * value)
{
  setValueCalled(true);
  setNodeValue(value);
  setValueCalled(false);
}

void
CharacterData::setNodeValue(String * value) throw (dom::DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  data = value;
//   if (!setValueCalled())
//     ownerDocument()->replacedText(this);
}

String *
CharacterData::getData() const throw (dom::DOMException)
{
  return data;
}

unsigned int
CharacterData::getLength() const
{
  return data->getLength();
}

void
CharacterData::appendData(String * newData) throw (dom::DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  setNodeValue(concat(data,newData));
}

void
CharacterData::deleteData(unsigned int offset,
			      unsigned int count)
    throw(DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  int tailLength = data->getLength() - count - offset;
  if (tailLength < 0)
    tailLength = 0;
  try {
    setNodeValueInternal(concat(data->substring(0, offset),
				(tailLength > 0
				 ? data->substring(offset + count,
						   offset + count + tailLength)
				 : null)));
  }
  catch(std::out_of_range&) {
    throw DOMException(DOMException::INDEX_SIZE_ERR);
  }
}

void
CharacterData::insertData(unsigned int offset,
			      String * d)
  throw(DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  try {
    unicode::StringBuffer buf(data);
    buf.insert(offset, d);
    setNodeValueInternal(buf.createString());
    //ownerDocument()->insertedText(this, offset, d->getLength());
  }
  catch(std::out_of_range&) {
    throw DOMException(DOMException::INDEX_SIZE_ERR);
  }
}

void
CharacterData::replaceData(unsigned int offset,
			       unsigned int count,
			       String * data)
  throw(DOMException)
{
  deleteData(offset, count);
  insertData(offset, data);
}

void
CharacterData::setData(String * value)
  throw(DOMException)
{
  setNodeValue(value);
}

String *
CharacterData::substringData(unsigned int offset,
				 unsigned int count)
    throw(DOMException)
{
  unsigned int len = data->getLength();
  if (offset > len-1)
    throw DOMException(DOMException::INDEX_SIZE_ERR);
  unsigned int tailIndex = offset + count;
  if (tailIndex > len)
    tailIndex = len;
  return data->substring(offset, tailIndex);
}

}
