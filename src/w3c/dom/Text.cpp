#include <w3c/dom/Document.hpp>
#include <w3c/dom/Text.hpp>

namespace dom {

Text::Text(Document * ownerDoc, String * data)
  : CharacterData(ownerDoc, data)
{
}

Text::Text(const Text& other)
  : CharacterData(other)
{
}

Node::NodeType
Text::getNodeType() const
{
  return Node::TEXT_NODE;
}

Node *
Text::cloneNode(bool deep) const
{
  return new Text(*this);
}

String *
Text::getNodeName() const
{
  static String * text_string;
  if (text_string == 0)
    text_string = String::internString("#text");
  return text_string;
}

void
Text::setIgnorableWhitespace(bool ignore)
{
  Node::isIgnorableWhitespace(ignore);
}

bool
Text::isIgnorableWhitespace() const
{
  return internalIsIgnorableWhitespace();
}

Text *
Text::splitText(unsigned int offset)
  throw(DOMException)
{
  if (isReadOnly())
    throw DOMException(DOMException::NO_MODIFICATION_ALLOWED_ERR);
  if (offset > data->getLength())
    throw DOMException(DOMException::INDEX_SIZE_ERR);
  Text * newText =
    getOwnerDocument()->createTextNode(data->substring(offset));
  setNodeValue(data->substring(0, offset));
  Node * parentNode = getParentNode();
  if (parentNode != null)
    parentNode->insertBefore(newText, nextSibling);
  return newText;
}

}
