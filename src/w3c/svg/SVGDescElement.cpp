#include <w3c/svg/SVGDescElement.hpp>

#include <w3c/dom/Text.hpp>

namespace svg {

  void
  SVGDescElement::setText(const char* s)
  {
    
    for(dom::Node *n = getFirstChild(); n; n = getFirstChild())
      removeChild(n);

    dom::Text * t = new dom::Text(getOwnerDocument(), unicode::String::internString(s));
    insertBefore(t,0);
  }

  unicode::String*
  SVGDescElement::getText()
  {
    dom::Text * t = dynamic_cast<dom::Text*>(getFirstChild());
    if(t) {
      return t->getData();
    }
    return 0;
  }

}
