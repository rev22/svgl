#include <w3c/dom/ProcessingInstruction.hpp>

namespace dom {

ProcessingInstruction::ProcessingInstruction(Document * ownerDoc,
						     String * t, String * data)
  : CharacterData(ownerDoc, data),
    target(t)
{
}

ProcessingInstruction::ProcessingInstruction(const ProcessingInstruction& other)
  : CharacterData(other),
    target(other.target)
{
}

Node::NodeType
ProcessingInstruction::getNodeType() const
{
  return Node::PROCESSING_INSTRUCTION_NODE;
}

Node *
ProcessingInstruction::cloneNode(bool deep) const
{
  return new ProcessingInstruction(*this);
}

String *
ProcessingInstruction::getNodeName() const
{
  return target;
}

String *
ProcessingInstruction::getTarget() const
{
  return target;
}

String *
ProcessingInstruction::getData() const throw (dom::DOMException)
{
  return data;
}

void
ProcessingInstruction::setData(String * data)
  throw(DOMException)
{
  setNodeValue(data);
}

}
