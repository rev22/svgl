#include <w3c/dom/CDATASection.hpp>

namespace dom {

CDATASection::CDATASection(Document * ownerDoc, String * data)
  : Text(ownerDoc, data)
{
}

CDATASection::CDATASection(const CDATASection& other)
  : Text(other)
{
}

Node::NodeType
CDATASection::getNodeType() const
{
  return Node::CDATA_SECTION_NODE;
}

String *
CDATASection::getNodeName() const
{
  static String * cdata_section_string;
  if (cdata_section_string == null)
    cdata_section_string = String::internString("#cdata-section");
  return cdata_section_string;
}

}
