#include <w3c/dom/Document.hpp>
#include <w3c/dom/DocumentFragment.hpp>

namespace dom {

DocumentFragment::DocumentFragment(Document * ownerDoc)
  : Node(ownerDoc) {}

DocumentFragment::DocumentFragment(const DocumentFragment& other)
  : Node(other) {}

Node::NodeType
DocumentFragment::getNodeType() const
{
  return Node::DOCUMENT_FRAGMENT_NODE;
}

Node *
DocumentFragment::cloneNode(bool deep) const
{
  return null;// new DocumentFragment(*this);
}

}
