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
the file license.txt and at the following URL:
http://www.emn.fr/info/image/Themes/Indigo/licence.html

 */
#include <w3c/svg/SVGAnimateElement.hpp>
#include <svgl/AnimationInfo.hpp>
#include <svgl/AnimateAdapter.hpp>
#include <svgl/AnimateAdapterDouble.hpp>
#include <utils/StringUtils.hpp>

#include <iostream>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

#include <w3c/css/CSSStyleDeclaration.hpp>

namespace svg {

  extern unicode::String* height_attribute_string;

  void
  SVGAnimateElement::animationTraverse(svgl::AnimationInfo* animinfo)
  {
    unicode::String* attrName = GET_SIMPLE_VAL(AttributeName);
    SVG_ANIMATION_ATTRIBUTE_TYPE attrType = GET_SIMPLE_VAL(AttributeType);

    _elementToAnimate = animinfo->element;
    _attributeToAnimate=0;

    switch (attrType) {
    case SVG_ANIMATION_ATTRIBUTE_TYPE_CSS:
      std::cerr << "CSS animation not supported" << __FL__;
      return;
      break;

    default:
      attributeType.setValue(SVG_ANIMATION_ATTRIBUTE_TYPE_AUTO);
    case SVG_ANIMATION_ATTRIBUTE_TYPE_AUTO:
    case SVG_ANIMATION_ATTRIBUTE_TYPE_XML:
      {
      _attributeToAnimate = _elementToAnimate->getAttributeByName(attrName);
      }
      break;
#if 0
    default:
      std::cerr << "SVG_ANIMATION_ATTRIBUTE is set to UNKNOWN" << __FL__;
      break;
#endif
    }

    if(_attributeToAnimate==0) {
      if(css::CSSStyleDeclaration::isStyleAttribute(attrName)) {
	std::cerr << "attribute '" << attrName << "' for element '" << _elementToAnimate->getDtdElement() << "' is a style attribute and is not yet supported yet '<animate>'" << __FL__;
      }
      else {
	std::cerr << "attribute '" << attrName << "' does not exist for element " << _elementToAnimate->getDtdElement() << __FL__;
      }
      return;
    }

    if(!_attributeToAnimate->canBeSetAsDouble()) {
      _attributeToAnimate=0;
      std::cerr << "attribute '" << attrName << "' in element '" << _elementToAnimate->getDtdElement() << "' can not be set as double" << __FL__;
      return;
    }

    const SVGLengthList& values = GET_SIMPLE_VAL(Values);
#if 0
    std::cerr << "-- values" << __FL__;
    for(unsigned int t=0; t<values.size(); ++t)
      std::cerr << values[t] << " ";
    std::cerr << std::endl;
#endif
    if(values.numberOfItems()) {

      const SVGNumberList& keytimes = GET_SIMPLE_VAL(KeyTimes);
#if 0
      std::cerr << "** keytimes " << __FL__;
#endif
      if(keytimes.numberOfItems()) {
	if(keytimes.numberOfItems()!=values.numberOfItems()) {
	  std::cerr << "'keyTimes' and 'values' have different size" << __FL__;
	  return;
	}
	for(std::vector<float>::size_type i=0;
	    i<values.numberOfItems();
	    ++i) {
	  SVGLength l = values.getItem(i);
	  float v = l.computeValue();
	  _keyFrames.push_back(KeyFrame(v, keytimes.getItem(i).GET_SIMPLE_VAL(Value)));
	  //	  std::cerr << values[i] << " " << keytimes[i];
	}
	//	std::cerr << std::endl;

      }
      else {
	for(std::vector<float>::size_type i=0;
	    i<values.numberOfItems();
	    ++i) {
	  float t=i/((float)values.numberOfItems()-1);
	  SVGLength l = values.getItem(i);
	  float v = l.computeValue();
	  _keyFrames.push_back(KeyFrame(v, t));
	}
      }
      
    }
    else {
      unicode::String* fromstr = GET_SIMPLE_VAL(From);
      unicode::String* tostr = GET_SIMPLE_VAL(To);
      unicode::String* bystr = GET_SIMPLE_VAL(By);
      //std::cerr << DBGVAR(attrName) << DBGVAR(attrType) << DBGVAR(fromstr) << DBGVAR(tostr) << __FL__;

      SVGLength froml, tol, byl;
      bool fromb=false, tob=false, byb=false;

      froml.dom_setValueAsString(fromstr);
      tol.dom_setValueAsString(tostr);
      byl.dom_setValueAsString(bystr);

      fromb = (froml.GET_SIMPLE_VAL(UnitType) != SVGLength::SVG_LENGTHTYPE_UNKNOWN);
      tob = (tol.GET_SIMPLE_VAL(UnitType) != SVGLength::SVG_LENGTHTYPE_UNKNOWN);
      byb = (byl.GET_SIMPLE_VAL(UnitType) != SVGLength::SVG_LENGTHTYPE_UNKNOWN);

    float from = froml.GET_SIMPLE_VAL(Value);
      float to = tol.GET_SIMPLE_VAL(Value);
      float by = byl.GET_SIMPLE_VAL(Value);

    // std::cerr << DBGVAR(fromb) << DBGVAR(tob) << DBGVAR(byb) << __FL__;

      if( !(fromb||tob||byb) ) {
	std::cerr << "'from', 'to' and 'by' are unset" << __FL__;
	return;
      }

      // FIXME
      // from is not computed with right dpi...
      if(!fromb) {
	fromb=true;
	from = _attributeToAnimate->getBaseVal();
      }
      if(!tob) {
	if(byb) {
	  to=from+by;
	}
	else {
	  STDDBG;
	  return;
	}
      }

      if(fromb)
	_keyFrames.push_back(KeyFrame(from, 0));
      _keyFrames.push_back(KeyFrame(to, 1));

    }

#if 0
    DBG;
    for(unsigned int j=0; j<_keyFrames.numberOfItems(); ++j) {
      std::cerr << "(" << _keyFrames[j].value << "," << _keyFrames[j].time << ")";
    }
    std::cerr << std::endl;
#endif

    SVGAnimationElement::animationTraverse(animinfo);

  }

  void
  SVGAnimateElement::tick(svgl::Animation::TickInfo* tickinfo)
  {

    if(!_attributeToAnimate) {
      std::cerr << "no attribute to animate" << __FL__;
      return;
    }

    if(!tickinfo->dur) {
      std::cerr << "invalid tickinfo->dur " << tickinfo->dur << __FL__;
    }

    float timeRatio = (tickinfo->totalElapsed/(float)tickinfo->dur);

    if(_keyFrames.size()==0 ) {
      // from is not set
      // not very reliable FIXME
      std::cerr << "invalid attribute 'keyTimes'" << __FL__;
      return;
    }

#if 0
    if(_keyFrames.size()==1 ) {
      // from is not set
      // not very reliable FIXME
      double from = _attributeToAnimate->getBaseVal(); // now it has been computed
      KeyFrame toframe = _keyFrames[0];
      _keyFrames[0]=KeyFrame(from, 0);
      _keyFrames.push_back(toframe);
    }
#endif

    std::vector<float>::size_type i=0;
    for(;
	i<_keyFrames.size();
	++i) {
      if( _keyFrames[i].time>=timeRatio) {	
	break;
      }
    }

    // if the first one is non-zero
    // is it possible ??
    if(i==0)
      return;

    float from = _keyFrames[i-1].value;
    float to = _keyFrames[i].value;
    
    timeRatio -= _keyFrames[i-1].time;

    double newvalue;
    float deltatime = _keyFrames[i].time -_keyFrames[i-1].time;
    if(deltatime<=.00001)
      newvalue=to;
    else {
      timeRatio /= deltatime;
      newvalue = from + (to-from)*timeRatio;
    }
      
    // std::cerr << DBGVAR(from) << DBGVAR(to) << DBGVAR(newvalue) << DBGVAR(deltatime) << __FL__;

    _attributeToAnimate->setAsDouble(newvalue);
  }

  void
  SVGAnimateElement::reset()
  {
    if(_attributeToAnimate) {      
      double d = _attributeToAnimate->getBaseVal();
      _attributeToAnimate->setAsDouble(d);
    }

  }


}

