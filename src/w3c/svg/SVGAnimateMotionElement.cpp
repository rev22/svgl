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
#include <w3c/svg/SVGAnimateMotionElement.hpp>
#include <w3c/svg/SVGAnimatedTransformList.hpp>
#include <w3c/svg/SVGTransforms.hpp>
#include <w3c/svg/SVGTransformable.hpp>
#include <w3c/svg/SVGMPathElement.hpp>
#include <w3c/svg/SVGPathElement.hpp>
//#include <w3c/svg/SVGSVGElement.hpp>
#include <svgl/AnimationInfo.hpp>
#include <svgl/AnimateAdapter.hpp>
#include <svgl/ExternalEntityManager.hpp>
#include <svgl/BezierPathDistanceIterator.hpp>
#include <svgl/BezierPathLength.hpp>
#include <utils/StringUtils.hpp>
#include <svgl/parseHelper.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

namespace svg {

  inline
  const SVGLength
  operator+(const SVGLength& l1, const SVGLength& l2)
  {
    float v = l1.GET_SIMPLE_VAL(Value)+l2.GET_SIMPLE_VAL(Value);
    SVGLength l;
    l.newValueSpecifiedUnits(SVGLength::SVG_LENGTHTYPE_NUMBER, v);
    return l;
  }

  void
  SVGAnimateMotionElement::animationTraverse(svgl::AnimationInfo* animinfo)
  {
    _elementToAnimate = animinfo->element;
    _translateToAnimate = 0;

    SVGTransformable* toAnimate = dynamic_cast<SVGTransformable*>(_elementToAnimate);
    if(!toAnimate) {
      std::cerr << "element refered by 'animateMotion' is not a SVGTransformable" << __FL__;
      return;
    }
    Attribute< SVGAnimatedTransformList >& transform = toAnimate->transform;
    {
      std::vector <float> v;
      v.push_back(0);
      v.push_back(0);
      _translateToAnimate = new SVGTranslateTransform(v);
    }
    MYGET_ANIMATED_VAL(transform).insertItemBefore(_translateToAnimate, 0);

    _doBezierAnim=false;
    _doRotateAnim = rotate_no;
    _rotateToAnimate=0;

    const SVGNumberList& keytimes = GET_SIMPLE_VAL(KeyTimes);
    const SVGPathSegList* pl=0;

    dom::Node *n = getFirstChild();
    if(n) {
      SVGMPathElement* mpath = 0;
      for(;n;n=n->getNextSibling()) {
	mpath = dynamic_cast<SVGMPathElement*>(n);
	if(mpath)
	  break;
      }
      if(mpath) {
	unicode::String* href = mpath->GET_SIMPLE_VAL(Href);
	if(href->getLength()) {
	  dom::Element* de = animinfo->externalEntityManager->get(href, getOwnerDocument());
	  //dom::Element* de = GET_SIMPLE_VAL(OwnerSVGElement)->getElementById(href->substring(1));
	  SVGPathElement* path = dynamic_cast<SVGPathElement*>(de);
	  if(de)
	    pl = &(path->GET_SIMPLE_VAL(PathSegList));
	}
      }
    }
    else
      pl = &(GET_SIMPLE_VAL(Path));

    if (pl && pl->numberOfItems()>=2) {
      BezierPath bezier;
      for(unsigned int i=0; i<pl->numberOfItems(); ++i) {
	const SVGPathSeg* p = pl->getItem(i);
	p->updateBezier(bezier);
      }
      _distanceIter = new svgl::BezierPathDistanceIterator(bezier,.1);
      _bezierLength = svgl::length(bezier);

      unicode::String* rotate_ptr = GET_SIMPLE_VAL(Rotate);
      unicode::String& rotate = *rotate_ptr;

      if (rotate_ptr && rotate.getLength()) {
	static unicode::String
	  &auto_string = *unicode::String::internString("auto"),
	  &auto_reverse_string = *unicode::String::internString("auto-reverse");
	float rot=0;
	if(rotate==auto_string) {
	  _doRotateAnim=rotate_auto;
	}
	else if(rotate==auto_reverse_string) {
	  _doRotateAnim=rotate_auto_reverse;
	}
	else {
	  if (unicode::get_float(rotate, &rot)) {
	    _doRotateAnim=rotate_angle;
	  }
	}
	if(_doRotateAnim!=rotate_no) {
	  std::vector<float> v;
	  v.push_back(rot);
	  v.push_back(0);
	  v.push_back(0);
	  _rotateToAnimate = new SVGRotateTransform(v);
	  MYGET_ANIMATED_VAL(transform).insertItemBefore(_rotateToAnimate, 1);
	}
      }
      _doBezierAnim = true;
    }

    else {
      const SVGLengthList& values = GET_SIMPLE_VAL(Values);
      unsigned int valuesSize = values.numberOfItems();
      if(valuesSize) {
	if(valuesSize % 2) {
	  std::cerr << "'values' for 'animateMotion' must be a list of pair of length" << __FL__;
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
	    //_keyFrames.push_back(KeyFrame(KeyFrame::ValueType<float,float>(v1,v2), keytimes.getItem(i).getValue().getValue()));
	    _keyFrames.push_back(KeyFrame(KeyFrame::ValueType(v1,v2), keytimes.getItem(i).GET_SIMPLE_VAL(Value)));
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
	unicode::String* fromstr = GET_SIMPLE_VAL(From);
	unicode::String* tostr = GET_SIMPLE_VAL(To);
	unicode::String* bystr = GET_SIMPLE_VAL(By);
	//std::cerr << DBGVAR(attrName) << DBGVAR(attrType) << DBGVAR(fromstr) << DBGVAR(tostr) << __FL__;

	SVGList<SVGLength> from, to, by;
	bool fromb=false, tob=false, byb=false;

	svgl::read(fromstr,from);
	svgl::read(tostr,to);
	svgl::read(bystr,by);

	fromb = (from.numberOfItems()>0);
	tob = (to.numberOfItems()>0);
	byb = (by.numberOfItems()>0);

	// std::cerr << DBGVAR(fromb) << DBGVAR(tob) << DBGVAR(byb) << __FL__;

	if( !(fromb||tob||byb) ) {
	  std::cerr << "'from', 'to' and 'by' are unset" << __FL__;
	  return;
	}

	if(fromb && from.numberOfItems()!=2) {
	  std::cerr << "'from' for 'animateMotion' must be a pair of length" << __FL__;
	  return;
	}

	if(tob && to.numberOfItems()!=2) {
	  std::cerr << "'to' for 'animateMotion' must be a pair of length" << __FL__;
	  return;
	}

	if(byb && by.numberOfItems()!=2) {
	  std::cerr << "'by' for 'animateMotion' must be a pair of length" << __FL__;
	  return;
	}

	// FIXME
	// from is not computed with right dpi...
	if(!fromb) {
	  static unicode::String
	    *x_string = unicode::String::internString("x"),
	    *y_string = unicode::String::internString("y");
	  svgl::AnimateAdapter* x = _elementToAnimate->getAttributeByName(x_string);
	  if(x==0 || !x->canBeSetAsDouble()) {
	    std::cerr << "attribute 'x' of the element referenced by 'animateMotion' can not be set as double" << __FL__;
	    return;
	  }

	  svgl::AnimateAdapter* y = _elementToAnimate->getAttributeByName(y_string);
	  if(y==0 || !y->canBeSetAsDouble()) {
	    std::cerr << "attribute 'y' of the element referenced by 'animateMotion' can not be set as double" << __FL__;
	    return;
	  }

	  SVGLength l;
	  l.newValueSpecifiedUnits(SVGLength::SVG_LENGTHTYPE_NUMBER, x->getBaseVal());
	  from.replaceItem(l,0);
	  l.newValueSpecifiedUnits(SVGLength::SVG_LENGTHTYPE_NUMBER, y->getBaseVal());
	  from.replaceItem(l,1);

	  fromb=true;
	}
	if(!tob) {
	  if(byb) {	  
	    to.replaceItem(from.getItem(0)+by.getItem(0), 0);
	    to.replaceItem(from.getItem(1)+by.getItem(1), 1);
	  }
#if 0
	  else {
	    STDDBG;
	    return;
	  }
#endif
	}

	if(fromb)
	  _keyFrames.push_back(KeyFrame( KeyFrame::ValueType(from.getItem(0).GET_SIMPLE_VAL(Value), from.getItem(1).GET_SIMPLE_VAL(Value)), 0));
	_keyFrames.push_back(KeyFrame( KeyFrame::ValueType(to.getItem(0).GET_SIMPLE_VAL(Value), to.getItem(1).GET_SIMPLE_VAL(Value)), 1));
      }

#if 0
      STDDBG;
      for(unsigned int j=0; j<_keyFrames.size(); ++j) {
	std::cerr << "( <" << _keyFrames[j].value.first << "," << _keyFrames[j].value.second << ">, " << _keyFrames[j].time << ")";
      }
      std::cerr << std::endl;
#endif
    }

    SVGAnimationElement::animationTraverse(animinfo);

  }

  void
  SVGAnimateMotionElement::tick(svgl::Animation::TickInfo* tickinfo)
  {
    if(!_translateToAnimate) {
      std::cerr << "no attribute to animate" << __FL__;
      return;
    }

    if(!tickinfo->dur) {
      std::cerr << "invalid tickinfo->dur " << tickinfo->dur << __FL__;
      return;
    }

    if(_doBezierAnim) {
      float timeRatio = (tickinfo->lastElapsed/(float)tickinfo->dur);
      float distanceRatio = timeRatio * _bezierLength;

      _distanceIter->advance(distanceRatio);
      _translateToAnimate->tx=_distanceIter->x;
      _translateToAnimate->ty=_distanceIter->y;
      switch(_doRotateAnim) {
      case rotate_auto:
	_rotateToAnimate->angle = -_distanceIter->getOrientation();
	break;
      case rotate_auto_reverse:
	_rotateToAnimate->angle = -_distanceIter->getOrientation()-180;
	break;
      case rotate_no:
      case rotate_angle:
      default:
	break;
      }
      //std::cerr << DBGVAR(tickinfo->lastElapsed) << DBGVAR(_bezierLength) << DBGVAR(distanceRatio) << DBGVAR(_distanceIter->x) << DBGVAR(_distanceIter->y) << __FL__;
    }
    else {
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
      timeRatio -= _keyFrames[i-1].time;

      double x;
      double y;
      if(deltatime<=.00001) {
	x=_keyFrames[i].value.first;
	y=_keyFrames[i].value.second;
      }
      else {
	timeRatio /= deltatime;
	float from = _keyFrames[i-1].value.first;
	float to = _keyFrames[i].value.first;
	x = from + (to-from)*timeRatio;
	from = _keyFrames[i-1].value.second;
	to = _keyFrames[i].value.second;
	y = from + (to-from)*timeRatio;
      }
      
      // std::cerr << DBGVAR(x) << DBGVAR(y) << __FL__;
      _translateToAnimate->tx=x;
      _translateToAnimate->ty=y;
    }
    //    _attributeToAnimate->setAsDouble(newvalue);
  }

  void
  SVGAnimateMotionElement::reset()
  {
    if(_translateToAnimate) {
      _translateToAnimate->tx=0;
      _translateToAnimate->ty=0;
    }

    if( _rotateToAnimate) {
      _rotateToAnimate->angle=0;
    }

    if(_doBezierAnim) {
      _distanceIter->reset();
    }
  }


}

