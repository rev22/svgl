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
#include <w3c/svg/SVGAnimateTransformElement.hpp>
#include <w3c/svg/SVGAnimatedTransformList.hpp>
#include <w3c/svg/SVGTransforms.hpp>
#include <w3c/svg/SVGTransformable.hpp>

#include <w3c/svg/SVGSVGElement.hpp>
#include <svgl/AnimationInfo.hpp>
#include <svgl/AnimateAdapter.hpp>
#include <utils/StringUtils.hpp>
#include <svgl/parseHelper.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

namespace svg {

  typedef std::vector< float > ValueList;
  typedef std::vector< ValueList > ValueListList;

  bool
  get_value_list_list(unicode::String* in_ptr, ValueListList* v)
  {
    unicode::String& in = *in_ptr;
    unicode::String::size_type len = in.getLength();
    if (!len)
      return false;

    unicode::String::size_type beg = unicode::eat_whitespaces(in);
    // static const unicode::string sep = " \t\n\r,";
    static const unicode::String::char_type semi_column = ';';

    while(beg<len) {
      unicode::String::size_type pos = in.indexOf(semi_column, beg);
      v->push_back(ValueList());
      ValueList& vtmp = v->back();
      if(!unicode::get_float_list(in, &vtmp, beg, pos)) {
	return false;
      }
      if(pos==unicode::String::npos)
	break;
      ++pos;
      beg = unicode::eat_whitespaces(in, pos);
    }
    return true;

  }

  typedef SVGAnimateTransformElement::KeyFrameValueType KeyFrameValueType;

  inline
  const KeyFrameValueType
  operator + (const KeyFrameValueType& c1, const KeyFrameValueType& c2)
  {
    KeyFrameValueType c=c1;
    c.first+=c2.first;
    c.second+=c2.second;
    c.third+=c2.third;
    return c;
  }

  inline
  const KeyFrameValueType
  operator - (const KeyFrameValueType& c1, const KeyFrameValueType& c2)
  {
    KeyFrameValueType c=c1;
    c.first-=c2.first;
    c.second-=c2.second;
    c.third-=c2.third;
    return c;
  }

  inline
  const KeyFrameValueType
  operator * (const KeyFrameValueType& c1, float s)
  {
    KeyFrameValueType c=c1;
    c.first*=s;
    c.second*=s;
    c.third*=s;
    return c;
  }

  inline
  std::ostream&
  operator << (std::ostream& out, const KeyFrameValueType& c)
  {
    out << "(" << c.first << "," << c.second << "," << c.third << ")";
    return out;
  }


  void
  SVGAnimateTransformElement::animationTraverse(svgl::AnimationInfo* animinfo)
  {
    _elementToAnimate = animinfo->element;
    _translateToAnimate = 0;
    _rotateToAnimate = 0;
    _scaleToAnimate = 0;
    _skewXToAnimate = 0;
    _skewYToAnimate = 0;

    SVGTransformable* toAnimate = dynamic_cast<SVGTransformable*>(_elementToAnimate);
    if(!toAnimate) {
      std::cerr << "element refered by 'animateTransform' is not a SVGTransformable" << __FL__;
      return;
    }
    Attribute< SVGAnimatedTransformList >& transform = toAnimate->transform;

    unicode::String* values = GET_SIMPLE_VAL(Values);
    ValueListList v;
    bool inFactToIsBy=false;

    if(values) {
      bool valuesb = get_value_list_list(values, &v);
      if (!valuesb) {
	std::cerr << "invalid 'values' attribute \"" << values << "\" for element 'animateTransform'" __FL__;
	return;
      }
    }
    else {
      unicode::String* fromstr = GET_SIMPLE_VAL(From);
      unicode::String* tostr = GET_SIMPLE_VAL(To);
      unicode::String* bystr = GET_SIMPLE_VAL(By);

      // std::cerr << DBGVAR(fromstr) << DBGVAR(tostr) << DBGVAR(bystr) << __FL__;

      ValueList from, to, by;
      bool fromb=false, tob=false, byb=false;

      if(fromstr) {
	fromb = unicode::get_float_list(*fromstr, &from);
      }
      if(tostr) {
	tob = unicode::get_float_list(*tostr, &to);
      }
      if(bystr) {
	byb = unicode::get_float_list(*bystr, &by);
      }

      if( !(fromb||tob||byb) ) {
	std::cerr << "'from', 'to' and 'by' are unset" << __FL__;
	return;
      }


      // FIXME
      // from is not computed with right dpi...
      if(!fromb) {
	std::cerr << "don't know what to do with a missing 'from' attribute in 'animateTransform'" << __FL__;
	return;
      }
      v.push_back(from);
      if(!tob) {
	if(byb) {	  
	  v.push_back(by);
	  inFactToIsBy=true;
	}
	else {
	  std::cerr << "'to' and 'by' attributes are not set in 'animateTransform'" << __FL__;
	  return;
	}
      }
      else {
	v.push_back(to);
      }
    }

    // now build _keyFrames
    if(!v.size()) {
      STDDBG;
      return;
    }

    // for each transform type, try build the corresponding transform
    // if the passed vector<float> is correct, we can use default
    // values set by the constructor

    switch(GET_SIMPLE_VAL(Type)) {

    case SVG_ANIMATETRANSFORM_TYPE_TRANSLATE: {
      for(ValueListList::iterator i=v.begin();
	  i!=v.end();
	  ++i) {
	try {
	  SVGTranslateTransform t((*i));
	  i->clear();
	  i->push_back(t.tx);
	  i->push_back(t.ty);
	}
	catch (const SVGTransform::WrongNumberOfArguments& e) {
	  std::cerr << "WrongNumberOfArguments (" << i->size() << ") for 'translate' in 'animateTransform'" __FL__;
	  return;
	}				  
      }
      _translateToAnimate = new SVGTranslateTransform(v[0]);
      _translateToAnimate->setNeutral();
      MYGET_ANIMATED_VAL(transform).insertItemBefore(_translateToAnimate, 0);

      break;
    }
    case SVG_ANIMATETRANSFORM_TYPE_SCALE: {
      for(ValueListList::iterator i=v.begin();
	  i!=v.end();
	  ++i) {
	try {
	  SVGScaleTransform t((*i));
	  i->clear();
	  i->push_back(t.sx);
	  i->push_back(t.sy);
	}
	catch (const SVGTransform::WrongNumberOfArguments& e) {
	  std::cerr << "WrongNumberOfArguments (" << i->size() << ") for 'scale' in 'animateTransform'" __FL__;
	  return;
	}				  
      }
      _scaleToAnimate = new SVGScaleTransform(v[0]);
      _scaleToAnimate->setNeutral();
      MYGET_ANIMATED_VAL(transform).insertItemBefore(_scaleToAnimate, 0);
      break;
    }
    case SVG_ANIMATETRANSFORM_TYPE_ROTATE: {
      for(ValueListList::iterator i=v.begin();
	  i!=v.end();
	  ++i) {
	try {
	  SVGRotateTransform t((*i));
	  i->clear();
	  i->push_back(t.angle);
	  i->push_back(t.cx);
	  i->push_back(t.cy);
	}
	catch (const SVGTransform::WrongNumberOfArguments& e) {
	  std::cerr << "WrongNumberOfArguments (" << i->size() << ") for 'rotate' in 'animateTransform'" __FL__;
	  return;
	}				  
      }
      _rotateToAnimate = new SVGRotateTransform(v[0]);
      _rotateToAnimate->setNeutral();
      MYGET_ANIMATED_VAL(transform).insertItemBefore(_rotateToAnimate, 0);
      break;
    }
    case SVG_ANIMATETRANSFORM_TYPE_SKEWX: {
      for(ValueListList::iterator i=v.begin();
	  i!=v.end();
	  ++i) {
	try {
	  SVGSkewXTransform t((*i));
	  i->clear();
	  i->push_back(t.angle);
	}
	catch (const SVGTransform::WrongNumberOfArguments& e) {
	  std::cerr << "WrongNumberOfArguments (" << i->size() << ") for 'skewx' in 'animateTransform'" __FL__;
	  return;
	}				  
      }

      _skewXToAnimate = new SVGSkewXTransform(v[0]);
      _skewXToAnimate->setNeutral();
      MYGET_ANIMATED_VAL(transform).insertItemBefore(_skewXToAnimate, 0);
      break;
    }
    case SVG_ANIMATETRANSFORM_TYPE_SKEWY: {
      for(ValueListList::iterator i=v.begin();
	  i!=v.end();
	  ++i) {
	try {
	  SVGSkewYTransform t((*i));
	  i->clear();
	  i->push_back(t.angle);
	}
	catch (const SVGTransform::WrongNumberOfArguments& e) {
	  std::cerr << "WrongNumberOfArguments (" << i->size() << ") for 'skewy' in 'animateTransform'" __FL__;
	  return;
	}				  
      }

      _skewYToAnimate = new SVGSkewYTransform(v[0]);
      _skewYToAnimate->setNeutral();
      MYGET_ANIMATED_VAL(transform).insertItemBefore(_skewYToAnimate, 0);
      break;
    }
    default:
      std::cerr << "unknown transform type " << GET_SIMPLE_VAL(Type) << " in 'animateTransform'" __FL__;
      return;
      break;
    }

    // only 2 values are in v (using 'from' and 'by'), but we put 'by' in place of 'to'
    // now that default values have been set, we can do
    // the transformation on 'by' into 'to'
    if( inFactToIsBy ) {
      ValueList& from = v[0];
      ValueList& by = v[1];
      for(ValueList::size_type i = 0; i<from.size(); ++i) {
	by[i] = from[i]-by[i];
      }
    }

    unsigned int keyFrameSize = v[0].size();

    float t=0,dt=1./(v.size()-1);
    for(ValueListList::iterator i=v.begin();
	i!=v.end();
	++i) {
      _keyFrames.push_back(KeyFrame());
      KeyFrame& k = _keyFrames.back();
      k.time = t;
      t+=dt;
      switch(keyFrameSize) {
      case 3:
	k.value.third = (*i)[2];
      case 2:
	k.value.second = (*i)[1];
      case 1:
	k.value.first = (*i)[0];
      default:
	break;
      }
      
    }

    //    const SVGNumberList& keytimes = getKeyTimes().getValue();

#if 0
    else {
      const SVGLengthList& values = getValues().getValue();
      unsigned int valuesSize = values.numberOfItems();
      if(valuesSize) {
	if(valuesSize % 2) {
	  std::cerr << "'values' for 'animateTransform' must be a list of pair of length" << __FL__;
	  return;
	}
	if(keytimes.numberOfItems()) {
	  if(keytimes.numberOfItems()!=values.numberOfItems()) {
	    std::cerr << "'keyTimes' and 'values' have different size" << __FL__;
	    return;
	  }
	  for(unsigned int i=0;
	      i<values.numberOfItems();
	      ++++i) {
	    SVGLength l = values.getItem(i);
	    float v1 = l.computeValue();
	    l = values.getItem(i+1);
	    float v2 = l.computeValue();
	    _keyFrames.push_back(KeyFrame(KeyFrame::ValueType<float,float>(v1,v2), keytimes.getItem(i).getValue().getValue()));
	  }
	}
	else {
	  unsigned int num = values.numberOfItems();
	  float t=0;
	  float dt=1./((num>>1)-1);
	  for(unsigned int i=0;
	      i<num;
	      ++++i) {
	    SVGLength l = values.getItem(i);
	    float v1 = l.computeValue();
	    l = values.getItem(i+1);
	    float v2 = l.computeValue();
	    _keyFrames.push_back(KeyFrame(KeyFrame::ValueType(v1,v2), t));
	    t+=dt;
	  }
	}
      
      }
      else {
      }
#endif
#if 0
      STDDBG;
      for(unsigned int j=0; j<_keyFrames.size(); ++j) {
	std::cerr << "( " << _keyFrames[j].value << ", " << _keyFrames[j].time << ")";
      }
      std::cerr << std::endl;
#endif

    SVGAnimationElement::animationTraverse(animinfo);

  }

  void
  SVGAnimateTransformElement::tick(svgl::Animation::TickInfo* tickinfo)
  {
    if(!tickinfo->dur) {
      std::cerr << "invalid tickinfo->dur " << tickinfo->dur << __FL__;
      return;
    }

    float timeRatio = (tickinfo->totalElapsed/(float)tickinfo->dur);

    if(_keyFrames.size()==0 ) {
      // from is not set
      // not very reliable FIXME
      std::cerr << "invalid attribute 'keyTimes'" << __FL__;
      return;
    }

    KeyFrames::size_type i=0;
    for(;
	i<_keyFrames.size()-1;
	++i) {
      if( _keyFrames[i].time>=timeRatio) {	
	break;
      }
    }

    // if the first one is non-zero
    // is it possible ??
    if(i==0)
      return;

    float deltatime = _keyFrames[i].time -_keyFrames[i-1].time;
    // std::cerr << DBGVAR(deltatime) << std::endl;
    timeRatio -= _keyFrames[i-1].time;

    KeyFrameValueType newvalue;
    if(deltatime<=.00001) {
      newvalue = _keyFrames[i].value;
    }
    else {
      timeRatio /= deltatime;
      const KeyFrameValueType& from = _keyFrames[i-1].value;
      const KeyFrameValueType& to = _keyFrames[i].value;
      newvalue = from + (to-from)*timeRatio;
    }

    //std::cerr << DBGVAR(newvalue) << __FL__;

    switch(GET_SIMPLE_VAL(Type)) {

    case SVG_ANIMATETRANSFORM_TYPE_TRANSLATE:
      _translateToAnimate->tx = newvalue.first;
      _translateToAnimate->ty = newvalue.second;
      break;

    case SVG_ANIMATETRANSFORM_TYPE_SCALE:
      _scaleToAnimate->sx = newvalue.first;
      _scaleToAnimate->sy = newvalue.second;
      break;

    case SVG_ANIMATETRANSFORM_TYPE_ROTATE:
      _rotateToAnimate->angle = newvalue.first;
      _rotateToAnimate->cx = newvalue.second;
      _rotateToAnimate->cy = newvalue.third;
      break;

    case SVG_ANIMATETRANSFORM_TYPE_SKEWX:
      _skewXToAnimate->angle = newvalue.first;
      break;

    case SVG_ANIMATETRANSFORM_TYPE_SKEWY:
      _skewYToAnimate->angle = newvalue.first;
      break;

    default:
      std::cerr << "unknown transform type in 'animateTransform'" __FL__;
      return;
    }
  }

  void
  SVGAnimateTransformElement::reset()
  {
    switch(GET_SIMPLE_VAL(Type)) {

    case SVG_ANIMATETRANSFORM_TYPE_TRANSLATE:
      _translateToAnimate->setNeutral();
      break;

    case SVG_ANIMATETRANSFORM_TYPE_SCALE:
      _scaleToAnimate->setNeutral();
      break;

    case SVG_ANIMATETRANSFORM_TYPE_ROTATE:
      _rotateToAnimate->setNeutral();
      break;

    case SVG_ANIMATETRANSFORM_TYPE_SKEWX:
      _skewXToAnimate->setNeutral();
      break;

    case SVG_ANIMATETRANSFORM_TYPE_SKEWY:
      _skewYToAnimate->setNeutral();
      break;

    default:
      std::cerr << "unknown transform type in 'animateTransform'" __FL__;
      return;
    }
  }


}

