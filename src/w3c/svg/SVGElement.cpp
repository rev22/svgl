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
#include <svgl/getNextSVGElement.hpp>
//#include <w3c/dom/DOM.hpp>
#include <w3c/svg/SVGElement.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/svg/SVGSymbolElement.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/AnimationInfo.hpp>
#include <svgl/ExternalEntityManager.hpp>
#include <w3c/css/CSSStyleDeclaration.hpp>

#include <svgl/xmlostream.hpp>
#include <typeinfo>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

namespace svg {

#ifdef WITH_XERCESC

  void
  SVGElement::parse(const DOM_Node& node, SVGSVGElement* owner, SVGElement* viewport)
  {
    ownerSVGElement.setValue(owner);
    viewportElement.setValue(viewport);

    // attributes
    DOM_NamedNodeMap attributes = node.getAttributes();
    int attrCount = attributes.getLength();
  
    for (int i = 0; i < attrCount; ++i) {
      DOM_Node  attribute = attributes.item(i);
      const String attrName = attribute.getNodeName();
      const String attrValue = attribute.getNodeValue();
      parse_attributes(attrName,attrValue);
    }

    //ownerSVGElement.getValue()->setElementById(getId().getValue(), this);
    getOwnerDocument()->setElementById(getId().getValue(), this);

    // children
    DOM_Node child = node.getFirstChild();
    SVGElement *prev=0;
    SVGElement *firstChild=0;

    while(child!=0) {
      DOM_Node next;
      SVGElement *elt = getNextSVGElement(child, next, owner, viewport);

      if (elt) {
	if (!firstChild) {
	  firstChild = elt;
	  setFirstChild(elt);
	}
	if (prev) {
	  prev->setNextSibling(elt);
	  elt->setPreviousSibling(prev);
	}
	elt->setParent(this);
	prev=elt;
      }
    
      if (next!=0) {      
	child=next.getNextSibling();
      }
      else
	child=0;
    }
  }

#endif

  void
  SVGElement::setOwnerAndViewPort(SVGSVGElement* owner, SVGElement* viewport)
  {
    ownerSVGElement.setValue(owner);
    viewportElement.setValue(viewport);

#if 0
    // attributes
    DOM_NamedNodeMap attributes = node.getAttributes();
    int attrCount = attributes.getLength();
  
    for (int i = 0; i < attrCount; ++i) {
      DOM_Node  attribute = attributes.item(i);
      const String attrName = attribute.getNodeName();
      const String attrValue = attribute.getNodeValue();
      parse_attributes(attrName,attrValue);
    }
#endif

    //std::cerr << getId().getValue() << " " << this << __FL__;
    GET_SIMPLE_VAL(OwnerSVGElement)->setElementById(GET_SIMPLE_VAL(Id), this);

    // children

    for(dom::Node * node = getFirstChild(); node; node=node->getNextSibling() ) {
      SVGElement * svgchild = dynamic_cast<SVGElement *>(node);
      if(svgchild)
	svgchild->setOwnerAndViewPort(owner, viewport);
    }

  }


  SVGElement::SVGElement(dom::Document * doc, unicode::String * tagName)
    : Element(doc, tagName)
  {
    _glList = 0;
    _inGlList = false;
    _askForCompileGlTraverse = false;
    _damaged = true;
    _childrenDamaged = true;
		_snap = 0;
		_compileAsATexture = 0;
  }

  SVGElement::~SVGElement() {}

  void
  SVGElement::print(xmlostream& out)
  {
    out << "<" << getDtdElement();
    print_attributes(out);
    dom::Node *domchild = getFirstChild();
    SVGElement *child = dynamic_cast<SVGElement*>(domchild);
    if (child) {
#ifdef WIN32
      out << ">"  << endl;
#else
      out << ">" << reltabs(1) << endl;
#endif
      while (child) {
	child->print(out);
	domchild = child->getNextSibling();
	child = dynamic_cast<SVGElement*>(domchild);
      }
#ifdef WIN32
      out << "</" << getDtdElement() << ">" << endl;
#else
      out << reltabs(-1) << "</" << getDtdElement() << ">" << endl;
#endif
    }
    else {
      out << "/>" << endl;
    }

  }

  // #define STDDBG   std::cerr << this << " "__FILE__ << ":" << __LINE__ << std::endl;

  void
  SVGElement::updateStyle(SVGStylable* parent_ste)
  {
    //  SVGStylable * parent_ste = dynamic_cast<SVGStylable*>(parent);
    SVGStylable * meAsStylable = dynamic_cast<SVGStylable*>(this);

    //std::cerr << this << " " << getDtdElement() << __FL__;

    if (meAsStylable) {
      css::CSSStyleDeclaration& cssstyle = meAsStylable->getStyle().getValue();
      if (parent_ste) {
	cssstyle.updateStyle( &(parent_ste->getStyle().getValue()) );
      }
      else {
	cssstyle.updateStyle(0);
      }
      parent_ste = meAsStylable;
    }

    dom::Node *domchild = getFirstChild();
    SVGElement *child = dynamic_cast<SVGElement*>(domchild);
    while (domchild) {
      if(child) {
	child->updateStyle(parent_ste);
      }
      domchild = domchild->getNextSibling();
      child = dynamic_cast<SVGElement*>(domchild);
    }
  }


  void
  SVGElement::glTraverseChildren(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    dom::Node *domchild = getFirstChild();
    SVGElement *child = dynamic_cast<SVGElement*>(domchild);
    while (domchild) {
      if(child) {
#if 1
	child->glTraverse(svglContext, glinfo);
#else
	svgl::GLInfo glinfonew = *glinfo;
	child->glTraverse(svglContext, &glinfonew);
#endif
      }
      domchild = domchild->getNextSibling();
      child = dynamic_cast<SVGElement*>(domchild);
    }
  }

  void
  SVGElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    glTraverseChildren(svglContext, glinfo);
  }


  void
  SVGElement::glTraverseBranch(svgl::Context * svglContext, svgl::GLInfo* glinfo, Branch::iterator& beg, Branch::iterator& end)
  {
    glTraverse(svglContext, glinfo);  
    SVGElement * elem = *beg;
    ++beg;
    if(beg!=end)
      elem->glTraverseBranch(svglContext, glinfo, beg, end);
  }


  void
  SVGElement::glTraverseBoundingBoxChildren(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    dom::Node *domchild = getFirstChild();
    SVGElement *child = dynamic_cast<SVGElement*>(domchild);
    while (domchild) {
      if(child) {
#if 1
	child->glTraverseBoundingBox(svglContext, glinfo);
#else
	svgl::GLInfo glinfonew = *glinfo;
	child->glTraverseBoundingBox(svglContext, &glinfonew);
#endif
      }
      domchild = domchild->getNextSibling();
      child = dynamic_cast<SVGElement*>(domchild);
    }
  }


  void
  SVGElement::glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    glTraverseBoundingBoxChildren(svglContext, glinfo);
  }

  void
  SVGElement::pickTraverseChildren(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    dom::Node *domchild = getFirstChild();
    SVGElement *child = dynamic_cast<SVGElement*>(domchild);
    while (domchild) {
      if(child) {
#if 1
	child->pickTraverse(svglContext, glinfo);
#else
	svgl::GLInfo glinfonew = *glinfo;
	child->pickTraverse(svglContext, &glinfonew);
#endif
      }
      domchild = domchild->getNextSibling();
      child = dynamic_cast<SVGElement*>(domchild);
    }
  }

  void
  SVGElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    pickTraverseChildren(svglContext, glinfo);
  }



  void
  SVGElement::animationTraverseChildren(svgl::AnimationInfo* animinfo)
  {
    dom::Node *domchild = getFirstChild();
    if(domchild) {
      while(domchild->getNextSibling())
	domchild=domchild->getNextSibling();
    }
    else
      return;

    SVGElement *child = dynamic_cast<SVGElement*>(domchild);
    while (domchild) {
      if(child) {
	svgl::AnimationInfo animinfonew = *animinfo;
	animinfonew.element = this;
	child->animationTraverse(&animinfonew);
      }
      domchild = domchild->getPreviousSibling();
      child = dynamic_cast<SVGElement*>(domchild);
    }
  }

  void
  SVGElement::animationTraverse(svgl::AnimationInfo* animinfo)
  {
    animationTraverseChildren(animinfo);
  }


  void
  SVGElement::getPercentageWidthHeight(svgl::GLInfo* glinfo, float *percentage_width, float *percentage_height)
  {
#if 1
    std::cerr << "error SVGElement::getPercentageWidthHeight is deprecated"  __FL__;
    *percentage_width = *percentage_height = 0;
#else
    SVGElement* vp = getViewportElement().getValue();
    {
      SVGSVGElement* e = dynamic_cast<SVGSVGElement*>(vp);
      if(e) {
	if (vp==this) {
	  SVGLength a;
	  a = e->GETVAL(Width);
	  *percentage_width = a.computeValue(glinfo->dpi);
	  a = e->GETVAL(Height);
	  *percentage_height = a.computeValue(glinfo->dpi);
	}
	else {
	  float pw,ph;
	  e->getPercentageWidthHeight(glinfo, &pw,&ph);
	  SVGLength a;
	  a = e->GETVAL(Width);
	  *percentage_width = a.computeValue(glinfo->dpi, pw);
	  a = e->GETVAL(Height);
	  *percentage_height = a.computeValue(glinfo->dpi,ph);
	}
	return;
      } else {
	SVGSymbolElement* e = dynamic_cast<SVGSymbolElement*>(vp);
	if(e) {
	  *percentage_height = e->_useWidth;
	  *percentage_width = e->_useHeight;
	  return;
	}
      }
    }

    std::cerr << "error SVGElement::getPercentageWidthHeight"  __FL__;
#endif
  }


  dom::Node *
  SVGElement::insertBefore(dom::Node * newChild, dom::Node * refChild) throw(dom::DOMException)
  {
    dom::Node * res = Element::insertBefore(newChild, refChild);
    SVGStylable * meAsStylable = dynamic_cast<SVGStylable*>(this);
    SVGStylable * newAsStylable = dynamic_cast<SVGStylable*>(newChild);
    if(meAsStylable && newAsStylable) {
      css::CSSStyleDeclaration& styleDecl = const_cast<css::CSSStyleDeclaration&>(meAsStylable->GET_SIMPLE_VAL(Style));
      css::CSSStyle * style = const_cast<css::CSSStyle*>(styleDecl.getStyle());
      if(style) {
	meAsStylable->updateChildrenStyle();
      }
    }
    setDamaged(true);
    return res;
  }

  void
  SVGElement::raise()
  {
    setLast();
  }

  void
  SVGElement::lower()
  {
    setFirst();
  }

  void
  SVGElement::above(SVGElement* e) throw(dom::DOMException)
  {
    setAfter(e);
  }

  void
  SVGElement::below(SVGElement* e) throw(dom::DOMException)
  {
    setBefore(e);
  }

	
	void
  SVGElement::setId(const char* id)
{
    //dom::string ref = GETVAL(Id);
		
		unicode::String * ids = unicode::String::createString(id);
		setId(ids);
}

const char*
SVGElement::getIdcc()
{
	const dom::string& ref = getId();
	return ref->transcode();
}
		
	


  dom::Node * 
  SVGElement::internalInsertBefore(dom::Node * newChild, dom::Node * refChild) throw(dom::DOMException)
  {
    setDamaged(true);
    return dom::ParentNode::internalInsertBefore(newChild, refChild);
  }

  dom::Node * 
  SVGElement::removeChild(dom::Node * oldChild) throw(dom::DOMException)
  {
    setDamaged(true);
    return dom::ParentNode::removeChild(oldChild);
  }

  dom::Node *
  SVGElement::internalRemoveChild(dom::Node * oldChild) throw(dom::DOMException)
  {
    setDamaged(true);
    return dom::ParentNode::internalRemoveChild(oldChild);
  }

  dom::Node * 
  SVGElement::replaceChild(dom::Node * newChild, dom::Node * oldChild) throw(dom::DOMException)
  {
    setDamaged(true);
    return dom::ParentNode::replaceChild(newChild, oldChild);
  }

#if 1

  // --------------------------------------------------
  // optimization with display lists

  void
  SVGElement::setDamaged(bool yesno)
  {
    _damaged = yesno;
    if(yesno) {
      bool sav = _childrenDamaged;
      setChildrenDamaged(true);
      _childrenDamaged=sav; // our own children are not damaged...
    }
  }

bool 
SVGElement::getDamaged() const
{
	return _damaged;
}

  void
  SVGElement::setStyleDamaged(bool yesno)
  {
    setDamaged(yesno);
    if(yesno) {
      //...except if this is a style damage (isn't it be true for geo as well ? viewport ?)
      damageChildren();
    }
  }

  void
  SVGElement::damageChildren()
  {
    _damaged=true;
    dom::Node *domchild = getFirstChild();
    SVGElement *child = dynamic_cast<SVGElement*>(domchild);
    while (domchild) {
      if(child) {
	child->damageChildren();
      }
      domchild = domchild->getNextSibling();
      child = dynamic_cast<SVGElement*>(domchild);
    }
  }


  bool
  SVGElement::areChildrenDamaged()
  {
    if(!_childrenDamaged)
      return false;
    else {
      bool res=false;
      dom::Node *domchild = getFirstChild();

      // setAChildIsNotDamagedAnymore will not be called with non-parent node eg circle
      if(domchild && !_aChildIsNotDamagedAnymore)
	return _childrenDamaged;

      SVGElement *child = dynamic_cast<SVGElement*>(domchild);
      while (domchild) {
	if(child) {
	  res = child->areChildrenDamaged();
	  if(res)
	    break;
	}
	domchild = domchild->getNextSibling();
	child = dynamic_cast<SVGElement*>(domchild);
      }
      _childrenDamaged=res;
      _aChildIsNotDamagedAnymore=false;
    }
    return _childrenDamaged;
  }

  bool
  SVGElement::canBeCompiled()
  {
    //return _canBeCompiled;
    return (!areChildrenDamaged());
  }

  void
  SVGElement::setGlCompiled(bool)
  {    
  }


  void
  SVGElement::setAChildIsNotDamagedAnymore()
  {
#define DBGMETH 0
    // avoid duplicated spreading
    if(_aChildIsNotDamagedAnymore)
      return;

    _aChildIsNotDamagedAnymore=true;
#if DBGMETH
    std::cerr << "** --> aChildIsNotDamagedAnymore (me) <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
    // parents
    SVGElement * svge = dynamic_cast<SVGElement*>(getParentNode());
    if(svge) {
#if DBGMETH
      std::cerr << "** aChildIsNotDamagedAnymore (parent) <" << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
      svge->setAChildIsNotDamagedAnymore();
    }
    
    // <use>rs
    std::pair<svgl::ExternalEntityManager::UsedBy::iterator, svgl::ExternalEntityManager::UsedBy::iterator> p  = svgl::ExternalEntityManager::getDefault()->getUsers(this);
    
    for(; p.first != p.second; ++(p.first)) {
      SVGElement * svge = dynamic_cast<SVGElement*>(p.first->second);
      if(svge) {
#if DBGMETH
	std::cerr << "** aChildIsNotDamagedAnymore (user)  <" << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
	svge->setAChildIsNotDamagedAnymore();
      }
    }
#if DBGMETH
    std::cerr << "** <-- aChildIsNotDamagedAnymore (me) <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif

#undef DBGMETH
  }

  void
  SVGElement::setChildrenDamaged(bool yesno)
  {
#define DBGMETH 0
    _childrenDamaged = yesno;

    if(yesno) {
#if DBGMETH
      std::cerr << "** --> setChildrenDamaged (me) <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
      // parents
      SVGElement * svge = dynamic_cast<SVGElement*>(getParentNode());
      if(svge) {
#if DBGMETH
	std::cerr << "** setChildrenDamaged (parent) <" << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
	svge->setChildrenDamaged(true);
      }

      // <use>rs
      std::pair<svgl::ExternalEntityManager::UsedBy::iterator, svgl::ExternalEntityManager::UsedBy::iterator> p  = svgl::ExternalEntityManager::getDefault()->getUsers(this);

      for(; p.first != p.second; ++(p.first)) {
	SVGElement * svge = dynamic_cast<SVGElement*>(p.first->second);
	if(svge) {
#if DBGMETH
	  std::cerr << "** setChildrenDamaged (user) <" << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
	  svge->setChildrenDamaged(true);
	}
      }
#if DBGMETH
      std::cerr << "** --> setChildrenDamaged (me) <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
    }
    else {
      setAChildIsNotDamagedAnymore();
      _aChildIsNotDamagedAnymore=false;
    }
#undef DBGMETH
  }

  void
  SVGElement::setCanBeCompiled(bool yesno)
  {
    _canBeCompiled = yesno;
  }


  void
  SVGElement::compile(bool yesno)
  {
    _askForCompileGlTraverse = yesno;
    if(_glList)
      glDeleteLists(_glList, 1); // not mandatory
    _glList = 0;
    setDamaged(true);
  }

  bool
  SVGElement::glCompiled()
  {
    //return (_damaged==false) && ((_inGlList==0) && (_glList != 0));
    return (_damaged==false) && (areChildrenDamaged()==false) && (_glList!=0);
  }

  bool
  SVGElement::preManageGlCompilation()
  {
    // remember if it was damaged...
    bool mayRecompile = _childrenDamaged && _aChildIsNotDamagedAnymore;

    if(0 && _askForCompileGlTraverse) {
      std::cerr << "--> preManageGlCompilation " << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
      std::cerr << DBGVAR(_askForCompileGlTraverse) << DBGVAR(_glList) << DBGVAR(_inGlList) << DBGVAR(_damaged) << DBGVAR(_childrenDamaged) << DBGVAR(_aChildIsNotDamagedAnymore) << DBGVAR(_canBeCompiled) << DBGVAR(mayRecompile) << __FL__;      
      std::cerr << DBGVAR(glCompiled()) << DBGVAR(areChildrenDamaged()) << DBGVAR(canBeCompiled()) << __FL__;
      std::cerr << DBGVAR(_askForCompileGlTraverse) << DBGVAR(_glList) << DBGVAR(_inGlList) << DBGVAR(_damaged) << DBGVAR(_childrenDamaged) << DBGVAR(_aChildIsNotDamagedAnymore) << DBGVAR(_canBeCompiled) << DBGVAR(mayRecompile) << __FL__;
    }

    if(glCompiled() && !mayRecompile) {
      //std::cerr << "calling list for <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
      glCallList(_glList);
      return false;
    }

    bool doGlList = _askForCompileGlTraverse && canBeCompiled();
    if(doGlList) {
      //std::cerr << "$$$$$$ creating list for <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
      if(!_glList)
	_glList = glGenLists(1);
      _inGlList = true;
      glNewList(_glList, GL_COMPILE_AND_EXECUTE);
    }

    if(0 && _askForCompileGlTraverse) {
      std::cerr << "<-- preManageGlCompilation " << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
    }

    _aChildIsNotDamagedAnymore=false;

    return true;
  }

  void
  SVGElement::postManageGlCompilation()
  {
    if(_inGlList) {
      //std::cerr << "ending list for <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
      glEndList();
      _inGlList=false;
      _damaged=false;
      setChildrenDamaged(false);
    }


    if(!_askForCompileGlTraverse && _damaged && !areChildrenDamaged()) {
      //std::cerr << "<-- postManageGlCompilation " << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
      _damaged = false;
      setChildrenDamaged(false);
    }

  }

	void
	SVGElement::compileAsATexture(bool yesno)
{
		_compileAsATexture=yesno;
}
#else   
    
	// --------------------------------------------------
	// optimization with textures

	void
	SVGElement::setDamaged(bool yesno) {
		_damaged = yesno;
		if(yesno) {
			bool sav = _childrenDamaged;
			setChildrenDamaged(true);
			_childrenDamaged=sav; // our own children are not damaged...
		}
	}
	
	
	void
	SVGElement::setStyleDamaged(bool yesno) {
		setDamaged(yesno);
		if(yesno) {
			//...except if this is a style damage (isn't it be true for geo as well ? viewport ?)
			damageChildren();
		}
	}
	
	void
	SVGElement::damageChildren() {
    _damaged=true;
    dom::Node *domchild = getFirstChild();
    SVGElement *child = dynamic_cast<SVGElement*>(domchild);
    while (domchild) {
			if(child) {
				child->damageChildren();
			}
			domchild = domchild->getNextSibling();
			child = dynamic_cast<SVGElement*>(domchild);
    }
	}


bool
SVGElement::areChildrenDamaged()
{
	if(!_childrenDamaged)
		return false;
	else {
		bool res=false;
		dom::Node *domchild = getFirstChild();
		
		// setAChildIsNotDamagedAnymore will not be called with non-parent node eg circle
		if(domchild && !_aChildIsNotDamagedAnymore)
			return _childrenDamaged;
		
		SVGElement *child = dynamic_cast<SVGElement*>(domchild);
		while (domchild) {
			if(child) {
				res = child->areChildrenDamaged();
				if(res)
					break;
			}
			domchild = domchild->getNextSibling();
			child = dynamic_cast<SVGElement*>(domchild);
		}
		_childrenDamaged=res;
		_aChildIsNotDamagedAnymore=false;
	}
	return _childrenDamaged;
}

bool
SVGElement::canBeCompiled()
{
	//return _canBeCompiled;
	return (!areChildrenDamaged());
}

void
SVGElement::setGlCompiled(bool)
{    
}


void
SVGElement::setAChildIsNotDamagedAnymore()
{
#define DBGMETH 0
	// avoid duplicated spreading
	if(_aChildIsNotDamagedAnymore)
		return;
	
	_aChildIsNotDamagedAnymore=true;
#if DBGMETH
	std::cerr << "** --> aChildIsNotDamagedAnymore (me) <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
	// parents
	SVGElement * svge = dynamic_cast<SVGElement*>(getParentNode());
	if(svge) {
#if DBGMETH
		std::cerr << "** aChildIsNotDamagedAnymore (parent) <" << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
		svge->setAChildIsNotDamagedAnymore();
	}
	
	// <use>rs
	std::pair<svgl::ExternalEntityManager::UsedBy::iterator, svgl::ExternalEntityManager::UsedBy::iterator> p  = svgl::ExternalEntityManager::getDefault()->getUsers(this);
	
	for(; p.first != p.second; ++(p.first)) {
		SVGElement * svge = dynamic_cast<SVGElement*>(p.first->second);
		if(svge) {
#if DBGMETH
			std::cerr << "** aChildIsNotDamagedAnymore (user)  <" << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
			svge->setAChildIsNotDamagedAnymore();
		}
	}
#if DBGMETH
	std::cerr << "** <-- aChildIsNotDamagedAnymore (me) <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
	
#undef DBGMETH
}

void
SVGElement::setChildrenDamaged(bool yesno)
{
#define DBGMETH 0
	_childrenDamaged = yesno;
	
	if(yesno) {
#if DBGMETH
		std::cerr << "** --> setChildrenDamaged (me) <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
		// parents
		SVGElement * svge = dynamic_cast<SVGElement*>(getParentNode());
		if(svge) {
#if DBGMETH
			std::cerr << "** setChildrenDamaged (parent) <" << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
			svge->setChildrenDamaged(true);
		}
		
		// <use>rs
		std::pair<svgl::ExternalEntityManager::UsedBy::iterator, svgl::ExternalEntityManager::UsedBy::iterator> p  = svgl::ExternalEntityManager::getDefault()->getUsers(this);
		
		for(; p.first != p.second; ++(p.first)) {
			SVGElement * svge = dynamic_cast<SVGElement*>(p.first->second);
			if(svge) {
#if DBGMETH
				std::cerr << "** setChildrenDamaged (user) <" << svge->getDtdElement() << "> '" << svge->GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
				svge->setChildrenDamaged(true);
			}
		}
#if DBGMETH
		std::cerr << "** --> setChildrenDamaged (me) <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
#endif
	}
	else {
		setAChildIsNotDamagedAnymore();
		_aChildIsNotDamagedAnymore=false;
	}
#undef DBGMETH
}

void
SVGElement::setCanBeCompiled(bool yesno)
{
	_canBeCompiled = yesno;
}


void
SVGElement::compile(bool yesno)
{
	_askForCompileGlTraverse = yesno;
	if(_glList)
		glDeleteLists(_glList, 1); // not mandatory
	_glList = 0;
	setDamaged(true);
}

bool
SVGElement::glCompiled()
{
	//return (_damaged==false) && ((_inGlList==0) && (_glList != 0));
	return (_damaged==false) && (areChildrenDamaged()==false) && (_glList!=0);
}

bool
SVGElement::preManageGlCompilation()
{
	// remember if it was damaged...
	bool mayRecompile = _childrenDamaged && _aChildIsNotDamagedAnymore;
	
	if(0 && _askForCompileGlTraverse) {
		std::cerr << "--> preManageGlCompilation " << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
		std::cerr << DBGVAR(_askForCompileGlTraverse) << DBGVAR(_glList) << DBGVAR(_inGlList) << DBGVAR(_damaged) << DBGVAR(_childrenDamaged) << DBGVAR(_aChildIsNotDamagedAnymore) << DBGVAR(_canBeCompiled) << DBGVAR(mayRecompile) << __FL__;      
		std::cerr << DBGVAR(glCompiled()) << DBGVAR(areChildrenDamaged()) << DBGVAR(canBeCompiled()) << __FL__;
		std::cerr << DBGVAR(_askForCompileGlTraverse) << DBGVAR(_glList) << DBGVAR(_inGlList) << DBGVAR(_damaged) << DBGVAR(_childrenDamaged) << DBGVAR(_aChildIsNotDamagedAnymore) << DBGVAR(_canBeCompiled) << DBGVAR(mayRecompile) << __FL__;
	}
	
	if(glCompiled() && !mayRecompile) {
		//std::cerr << "calling list for <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
		glCallList(_glList);
		return false;
	}
	
	bool doGlList = _askForCompileGlTraverse && canBeCompiled();
	if(doGlList) {
		//std::cerr << "$$$$$$ creating list for <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
		if(!_glList)
			_glList = glGenLists(1);
		_inGlList = true;
		glNewList(_glList, GL_COMPILE_AND_EXECUTE);
	}
	
	if(0 && _askForCompileGlTraverse) {
		std::cerr << "<-- preManageGlCompilation " << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
	}
	
	_aChildIsNotDamagedAnymore=false;
	
	return true;
}

void
SVGElement::postManageGlCompilation()
{
	if(_inGlList) {
		//std::cerr << "ending list for <" << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
		glEndList();
		_inGlList=false;
		_damaged=false;
		setChildrenDamaged(false);
	}
	
	
	if(!_askForCompileGlTraverse && _damaged && !areChildrenDamaged()) {
		//std::cerr << "<-- postManageGlCompilation " << getDtdElement() << "> '" << GET_SIMPLE_VAL(Id) << "'" << __FL__;
		_damaged = false;
		setChildrenDamaged(false);
	}
	
}
#endif

  void
  SVGElement::registerUse(svgl::Context * svglContext)
  {
    dom::Node *domchild = getFirstChild();
    SVGElement *child = dynamic_cast<SVGElement*>(domchild);
    while (domchild) {
      if(child) {
	child->registerUse(svglContext);
      }
      domchild = domchild->getNextSibling();
      child = dynamic_cast<SVGElement*>(domchild);
    }
  }


} // namespace svg



