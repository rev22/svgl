#include <w3c/dom/Document.hpp>
#include <w3c/dom/EntityReference.hpp>

namespace dom {

EntityReference::EntityReference(Document * ownerDoc)
  : Node(ownerDoc) {}

EntityReference::EntityReference(const EntityReference& other)
  : Node(other) {}

Node::NodeType
EntityReference::getNodeType() const
{
  return Node::DOCUMENT_FRAGMENT_NODE;
}

Node *
EntityReference::cloneNode(bool deep) const
{
  return null; // new EntityReference(*this);
}


}
