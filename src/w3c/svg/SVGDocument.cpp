/*
 * $Id$
 *
Copyright (c) 2001 Stephane Conversy, Jean-Daniel Fekete and Ecole des
Mines de Nantes.
All rights reserved.
 
This software is proprietary information of Stephane Conversy,
Jean-Daniel Fekete and Ecole des Mines de Nantes.  You shall use it
only in accordance with the terms of the license agreement you
accepted when downloading this software.  The license is available in
the file licence.txt and at the following URL:
http://www.emn.fr/info/image/Themes/Indigo/licence.html

 */
#include <w3c/svg/SVGDocument.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <svgl/xmlostream.hpp>
#include <iostream>

#include <utils/String.hpp>
#include <w3c/dom/Attr.hpp>
#include <w3c/dom/impl/AttributeMap.hpp>
#include <w3c/svg/strings.hpp>
#include <w3c/svg/svg.hpp>

//#include <svgl/Parser.hpp>
#include <svgl/debug.hpp>

#include <cassert>
#include <exception>

namespace svg {

  class SVGAttr : public dom::Attr {
    typedef dom::Attr super;
  public:
    SVGAttr(dom::Document* doc, unicode::String * name)
      : super(doc, name) {}

    void
    setValue(unicode::String* v) throw(dom::DOMException) {
      try {
	super::setValue(v);
	SVGElement * element = dynamic_cast<SVGElement*>(getOwnerElement());
	if(element) {
	  //std::cerr << getName() << " " << (v ? v->transcode() : "") << __FL__;
	  element->parse_attributes(getName(), v);
	}
      }
      catch(const std::exception& e) {
	std::cerr << getName() << " " << (v ? v->transcode() : "") << " exception " << e.what() << __FL__;
	throw;
      }
      catch(...) {
	std::cerr << getName() << " " << (v ? v->transcode() : "") << " exception" << __FL__;
	throw;
      }
    }

  private:
    SVGElement* _element;
  };

  class SVGAttributeMap : public dom::AttributeMap {
    typedef dom::AttributeMap super;
  public:
    SVGAttributeMap(dom::Element * ownerNode, const dom::NamedNodeMapImpl * defaults)
      : super(ownerNode, defaults) {}

    dom::Node *
    setNamedItem(dom::Node * arg) throw(dom::DOMException)
    {
      dom::Node * res = super::setNamedItem(arg);
      dom::Attr * attr = dynamic_cast<dom::Attr*>(arg);
      if(attr) {
	dom::String * val = attr->getValue();
	if(val)
	  attr->setValue(val);
      }
      return res;
    }
    
    dom::Node *
    setNamedItemNS(dom::Node * arg) throw(dom::DOMException)
    {
      return setNamedItem(arg);
    }

  };

  SVGDocument::SVGDocument()
  {
    rootElement.setValue(0);
  }

  dom::Element *
  SVGDocument::createElement(unicode::String * tagName)
    throw(dom::DOMException)
  {
    //std::cerr << DBGVAR(tagName) << __FL__;
    SVGElement * res=0;
#if 1
    #define STRDEF(name, value) \
    if(tagName==SVG##name##Element_element_string) { \
      res = new SVG##name##Element(this); \
    } \
    else
    #include <w3c/svg/strings_def.hpp>
    #undef STRDEF
    // else
    {
    }
#endif    

    if(!res) {
      std::cerr << "unknown SVG element " << tagName << __FL__;
      return Document::createElement(tagName);
    }

    return res;
  }

  dom::Attr *
  SVGDocument::createAttribute(unicode::String * name)
    throw(dom::DOMException)
  {
    return new SVGAttr(this, name);
  }

  dom::AttributeMap *
  SVGDocument::createAttributeMap(dom::Element * e, dom::NamedNodeMapImpl * defaults)
  {
    return new SVGAttributeMap(e, defaults);
  }


  dom::Element *
  SVGDocument::createElementNS(unicode::String * namespaceURI, unicode::String * qualifiedName) throw(dom::DOMException)
  {
    //return dom::Document::createElementNS(namespaceURI, qualifiedName);
    return createElement(qualifiedName);
  }

  dom::Attr *
  SVGDocument::createAttributeNS(unicode::String * namespaceURI, unicode::String * qualifiedName) throw(dom::DOMException)
  {
    return createAttribute(qualifiedName);
  }

  dom::Node *
  SVGDocument::insertBefore(dom::Node * newChild, dom::Node * refChild)
    throw(dom::DOMException)
  {
    //std::cerr << DBGVAR(newChild->getNodeName()) << __FL__;
    dom::Node * ret = Document::insertBefore(newChild, refChild);
    if(!rootElement.getValue()) {
      SVGSVGElement * ressvg = dynamic_cast <SVGSVGElement*>(newChild);
      if(ressvg) {
	rootElement.setValue(ressvg);
	ressvg->setOwnerAndViewPort(ressvg, ressvg);
      }
    }
    //std::cerr << DBGVAR(newChild) << DBGVAR(refChild) << DBGVAR(ret) << __FL__;
    return ret;
  }

  dom::Node *
  SVGDocument::removeChild(dom::Node * oldChild)
    throw(dom::DOMException)
  {
    return Document::removeChild(oldChild);
  }

  dom::Node *
  SVGDocument::replaceChild(dom::Node * newChild, dom::Node * oldChild)
    throw(dom::DOMException)
  {
    return Document::replaceChild(newChild, oldChild);
  }

  dom::Node *
  SVGDocument::cloneNode(bool deep) const
  {
    return new SVGDocument();
  }

  void
  SVGDocument::updateStyle()
  {
    SVGSVGElement* svgsvg = rootElement.getValue();
    if(svgsvg)
      svgsvg->updateStyle(0);
  }

  void
  SVGDocument::print(xmlostream& out)
  {
    SVGSVGElement *e = rootElement.getValue();
    if (e)
      e->print(out);
  }

}




