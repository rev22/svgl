#include <w3c/dom/Document.hpp>
#include <w3c/dom/Entity.hpp>

namespace dom {

Entity::Entity(Document * ownerDoc)
  : Node(ownerDoc),
    publicId(null),
    systemId(null),
    notationName(null)
{}

Entity::Entity(const Entity& other)
  : Node(other),
    publicId(other.publicId),
    systemId(other.systemId),
    notationName(other.notationName)
{}

String *
Entity::getPublicId() const
{
  return publicId;
}

String *
Entity::getSystemId() const
{
  return systemId;
}

String *
Entity::getNotationName() const
{
  return notationName;
}

Node::NodeType
Entity::getNodeType() const
{
  return Node::ENTITY_NODE;
}

Node *
Entity::cloneNode(bool deep) const
{
  return null;// new Entity(*this);
}


}
