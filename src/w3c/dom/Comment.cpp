#include <w3c/dom/Comment.hpp>

namespace dom {

Comment::Comment(Document * ownerDoc, String * data)
  : CharacterData(ownerDoc, data)
{
}

Comment::Comment(const Comment& other)
  : CharacterData(other)
{
}

Node::NodeType
Comment::getNodeType() const
{
  return Node::COMMENT_NODE;
}

Node *
Comment::cloneNode(bool deep) const
{
  return new Comment(*this);
}

String *
Comment::getNodeName() const
{
  static String * comment_string;
  if (comment_string == null)
    comment_string = String::internString("#comment");
  return comment_string;
}

}
