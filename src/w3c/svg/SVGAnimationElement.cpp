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
#include <w3c/svg/SVGAnimationElement.hpp>
#include <svgl/AnimationInfo.hpp>
#include <svgl/AnimationManager.hpp>

#include <utils/StringUtils.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

#include <cassert>

namespace svg {

  void
  SVGAnimationElement::animationTraverse(svgl::AnimationInfo* animinfo)
  {
    _elementToAnimate = animinfo->element;
		//_elementToAnimate = dynamic_cast<SVGElement*>(getParentNode());
		assert(_elementToAnimate);

    // -1 -> special value for 'indefinite', the default
    _repeatCount = -1;
    unicode::String* repeatstr_ptr = GET_SIMPLE_VAL(RepeatCount);
    unicode::String& repeatstr = *repeatstr_ptr;
    if(repeatstr_ptr && repeatstr.getLength()) {
      if(unicode::get_float(repeatstr, &_repeatCount)) {
				if(_repeatCount<0) {
					std::cerr << "attribute 'repeatCount' is invalid" __FL__;
					return;
				}
      }
      else {
				static unicode::String& indefinite_string = *unicode::String::internString("indefinite");
				if(repeatstr!=indefinite_string) {
					std::cerr << "attribute 'repeatCount' is invalid" __FL__;
					return;
				}
      }
    }
		
		_repeatCounter=_repeatCount;

    animinfo->animationManager->subscribe(this);
  }

  void
  SVGAnimationElement::tick(svgl::Animation::TickInfo*)
  {
  }

  void
  SVGAnimationElement::started(svgl::Animation::Manager*)
  {
    reset();
  }

  bool
  SVGAnimationElement::finished(svgl::Animation::Manager* animationManager)
  {
    bool restart=false;

    switch(GET_SIMPLE_VAL(Fill)) {
    case SVG_ANIMATION_FILL_REMOVE:
      reset();
      break;
    default:
      break;
    }

    if(_repeatCounter>0.01) {
      _repeatCounter-=1;
      if(_repeatCounter>=1) {
				restart=true;
      }
      else {
				if(_repeatCounter>0.01) {
					std::cerr << "fractional repeat of animation is not handled" << DBGVAR(_repeatCounter) << __FL__;
				}
      }
    }
    else {
      // it's -1 -> special value for 'indefinite'
      restart=true;
    }

    return restart;
  }

  void
  SVGAnimationElement::reset()
  {
		_repeatCounter=_repeatCount;
  }

float
SVGAnimationElement::getStartTime (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

float
SVGAnimationElement::getCurrentTime (  ) 
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

float
SVGAnimationElement::getSimpleDuration (  ) 
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

}
