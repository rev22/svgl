#include <w3c/dom/Document.hpp>
#include <w3c/dom/Notation.hpp>

namespace dom {

Notation::Notation(Document * ownerDoc)
  : Node(ownerDoc),
    publicId(null),
    systemId(null)
{}

Notation::Notation(const Notation& other)
  : Node(other),
    publicId(other.publicId),
    systemId(other.systemId)
{}

String *
Notation::getPublicId() const
{
  return publicId;
}

String *
Notation::getSystemId() const
{
  return systemId;
}

Node::NodeType
Notation::getNodeType() const
{
  return Node::NOTATION_NODE;
}

Node *
Notation::cloneNode(bool deep) const
{
  return null;// new Notation(*this);
}


}
