#include <w3c/svg/SVGURIReference.hpp>
#include <w3c/svg/SVGElement.hpp>
#include <svgl/ExternalEntityManager.hpp>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>


namespace svg {

  void
  SVGURIReference::setHref(const char* href)
  {
    dom::string ref = GETVAL(Href);
    svg::SVGElement* svge = dynamic_cast<svg::SVGElement*>(this);
    dom::string newref = unicode::String::createString(href);
    if(svge) {
       if(ref) {
	dom::Element* de = svgl::ExternalEntityManager::getDefault()->get(ref, svge->getOwnerDocument());
	svgl::ExternalEntityManager::getDefault()->removeUsedBy(de, svge);
      }
      dom::Element* de = svgl::ExternalEntityManager::getDefault()->get(newref, svge->getOwnerDocument());
      if(de)
	svgl::ExternalEntityManager::getDefault()->addUsedBy(de, svge);
    }
    setHref(newref);
  }


}
