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
#include <w3c/svg/SVGLineElement.hpp>
#include <svgl/GLInfo.hpp>
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <w3c/svg/SVGTransforms.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

//#include <svgl/printHelper.hpp>

namespace svg {

  void
  SVGTransformable::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo) const
  {
    const SVGTransformList& transforms = GETVAL(Transform);
    for(unsigned int i=0; i<transforms.numberOfItems(); ++i) {
      transforms.getItem(i)->glTraverse(svglContext, glinfo);
    }
    
    //  std::cerr << "SVGTransformable::glTraverse() const" << std::endl;
    
  }

  void
  SVGTransformable::scale(double sx, double sy)
  {
    SVGScaleTransform * scale = getScale(true);
    scale->sx = sx;
    scale->sy = sy;
    scale->updateMatrix();
    SVGElement* meAsSVGElement = dynamic_cast<SVGElement*>(this);
    if(meAsSVGElement) meAsSVGElement->setDamaged(true);
  }

  void
  SVGTransformable::scale(double sx)
  {
    scale(sx,sx);
  }

  void
  SVGTransformable::dscale(double dsx, double dsy)
{
    SVGScaleTransform * scale = getScale(true);
    scale->sx *= dsx;
    scale->sy *= dsy;
    scale->updateMatrix();
    SVGElement* meAsSVGElement = dynamic_cast<SVGElement*>(this);
    if(meAsSVGElement) meAsSVGElement->setDamaged(true);
}

void
SVGTransformable::dscale(double dsx)
{
	scale(dsx,dsx);
}

  void
  SVGTransformable::rotate(double angle=0)
  {
    SVGRotateTransform * rotate = getRotate(true);
    rotate->angle = angle;
    rotate->cx = 0;
    rotate->cy = 0;
    rotate->updateMatrix();
    SVGElement* meAsSVGElement = dynamic_cast<SVGElement*>(this);
    if(meAsSVGElement) meAsSVGElement->setDamaged(true);
  }

void
SVGTransformable::drotate(double dangle=0)
{
	SVGRotateTransform * rotate = getRotate(true);
	rotate->angle += dangle;
	rotate->cx = 0;
	rotate->cy = 0;
	rotate->updateMatrix();
	SVGElement* meAsSVGElement = dynamic_cast<SVGElement*>(this);
	if(meAsSVGElement) meAsSVGElement->setDamaged(true);
}

  void
  SVGTransformable::translate(double tx, double ty)
  {
    SVGTranslateTransform * translate = getTranslate(true);
    translate->tx = tx;
    translate->ty = ty;
    translate->updateMatrix();
    SVGElement* meAsSVGElement = dynamic_cast<SVGElement*>(this);
    if(meAsSVGElement) meAsSVGElement->setDamaged(true);
  }

void
SVGTransformable::dtranslate(double dtx, double dty)
{
	SVGTranslateTransform * translate = getTranslate(true);
	translate->tx += dtx;
	translate->ty += dty;
	translate->updateMatrix();
	SVGElement* meAsSVGElement = dynamic_cast<SVGElement*>(this);
	if(meAsSVGElement) meAsSVGElement->setDamaged(true);
}

void
SVGTransformable::getConsolidation(SVGMatrix * m)
{
	static bool warnonce=true;
	if(warnonce) {
		std::cerr << "warning: consolidate is completely inefficient memory usage-wise" << __FL__;
		warnonce=false;
	}
	const svg::SVGTransformList &transforms = GET_ANIMATED_VAL(Transform);
	//std::cerr << e->getTagName() << __FL__;
	for(int i=transforms.numberOfItems()-1; i>=0; --i) {
		const svg::SVGMatrix& tmp = transforms.getItem(i)->getMatrix();
		//std::cerr << i << std::endl << tmp << __FL__;
		*m = *(m->multiply(tmp));
		//std::cerr << *m << __FL__;
	}
			
	dom::Element * meAsElement = dynamic_cast<dom::Element*>(this);
	if(!meAsElement)
		return;
	for(dom::Element *e = dynamic_cast<dom::Element*>(meAsElement->getParentNode());
			e;
			e = dynamic_cast<dom::Element*>(e->getParentNode())) {
		SVGTransformable * t = dynamic_cast<SVGTransformable*>(e);
		if(t)
			t->getConsolidation(m);
	}
}


// order: translate, rotation, scale
// applied order: reverse

#if 0

#define SVGThirdTransform SVGScaleTransform
#define getThird getScale

#define SVGFirstTransform SVGTranslateTransform
#define getFirst getTranslate

#else

#define SVGFirstTransform SVGScaleTransform
#define getFirst getScale

#define SVGThirdTransform SVGTranslateTransform
#define getThird getTranslate

#endif

#define SVGSecondTransform SVGRotateTransform
#define getSecond getRotate


#if 0 // initial implementation

  SVGFirstTransform *
  SVGTransformable::getFirst(bool create)
  {
    const SVGTransformList& transforms = GETVAL(Transform);
    unsigned int numTransform = transforms.numberOfItems();
    SVGFirstTransform * first = 0;
    if(!numTransform) {
      if(create) {
	first = new SVGFirstTransform;
	SVGTransformList& t = const_cast<SVGTransformList&>(transforms);
	t.appendItem(first);
      }
    }
    else {
      SVGTransform * transform = transforms.getItem(numTransform-1);
      first = dynamic_cast<SVGFirstTransform*>(transform);
      if(!first) {
	first = new SVGFirstTransform;
	SVGTransformList& t = const_cast<SVGTransformList&>(transforms);
	t.appendItem(first);
      }
    }
    //std::cerr << transforms << __FL__;
    return first;
  }


  SVGSecondTransform*
  SVGTransformable::getSecond(bool create)
  {
    const SVGTransformList& transforms = GETVAL(Transform);
    unsigned int numTransform = transforms.numberOfItems();
    SVGSecondTransform * second = 0;
    if(!numTransform) {
      if(create) {
	second = new SVGSecondTransform;
	SVGTransformList& t = const_cast<SVGTransformList&>(transforms);
	t.appendItem(second);
      }
    }
    else {
      unsigned int i=numTransform-1;
      SVGTransform * transform = transforms.getItem(i);
      second = dynamic_cast<SVGSecondTransform*>(transform);
      if(!second) {
	SVGFirstTransform * first = dynamic_cast<SVGFirstTransform*>(transform);
	if(first && (i-1)>=0) {
	  SVGTransform * transform = transforms.getItem(i-1);
	  second = dynamic_cast<SVGSecondTransform*>(transform);
	}
	else {
	  second = 0;
	  ++i;
	}
      }
      if(!second) {
	if(create) {
	  second = new SVGSecondTransform;
	  SVGTransformList& t = const_cast<SVGTransformList&>(transforms);
	  t.insertItemBefore(second,i);
	}
      }
    }
    //std::cerr << transforms << __FL__;
    return second;
  }


  SVGThirdTransform*
  SVGTransformable::getThird(bool create)
  {
    const SVGTransformList& transforms = GETVAL(Transform);
    unsigned int numTransform = transforms.numberOfItems();
    SVGThirdTransform * third = 0;
    if(!numTransform) {
      if(create) {
	third = new SVGThirdTransform;
	SVGTransformList& t = const_cast<SVGTransformList&>(transforms);
	t.appendItem(third);
      }
    }
    else {
      int i=numTransform-1;
      SVGTransform * transform = transforms.getItem(i);
      third = dynamic_cast<SVGThirdTransform*>(transform);
      if(!third) {
	SVGFirstTransform * first = dynamic_cast<SVGFirstTransform*>(transform);
	if(first) {
	  if ((i-1)>=0) {
	    --i;
	    SVGTransform * transform = transforms.getItem(i);
	    SVGSecondTransform * second = dynamic_cast<SVGSecondTransform*>(transform);
	    if(second) {
	      if ((i-1)>=0) {
		--i;
		SVGTransform * transform = transforms.getItem(i);
		third = dynamic_cast<SVGThirdTransform*>(transform);
	      }
	    } else {
	      ++i;
	    }
	  }
	}
	else {
	  ++i;
	}
      }
      if(!third) {
	if(create) {
	  third = new SVGThirdTransform;
	  SVGTransformList& t = const_cast<SVGTransformList&>(transforms);
	  t.insertItemBefore(third,i);
	}
      }
    }
    //std::cerr << transforms << __FL__;
    return third;
  }

#else // reimplemented

  SVGFirstTransform *SVGTransformable::getFirst(bool create) {
		const SVGTransformList &transforms = GETVAL(Transform);
		unsigned int numTransform = transforms.numberOfItems();
		SVGFirstTransform *first = 0;
		
		// place to insert the transform in the list if create is set
		unsigned int place = numTransform;

		// if the transform list is not empty, the only possible place for the
		// first transform is #0 (starting from the end)
		if(numTransform > 0) {
			first = dynamic_cast< SVGFirstTransform * >(transforms.getItem(numTransform - 1));
		}

		// creating & inserting the transform if needed
		if((first == 0) && create) {
			first = new SVGFirstTransform();
			SVGTransformList &t = const_cast< SVGTransformList & >(transforms);
			t.insertItemBefore(first, place);
		}

		return first;
	}

	SVGSecondTransform *SVGTransformable::getSecond(bool create) {
		const SVGTransformList &transforms = GETVAL(Transform);
		unsigned int numTransform = transforms.numberOfItems();
		SVGSecondTransform *second = 0;

		// place to insert the transform in the list if create is set
		unsigned int place = numTransform;

		// the transform #0 could be the one we are looking for
		if(numTransform > 0) {
			second = dynamic_cast< SVGSecondTransform * >(transforms.getItem(numTransform - 1));
			place -= 1;
		}
		// if it wasn't, it can be the #1 only if #0 is a SVGFirstTransform
		if((numTransform > 1) && (second == 0) &&
		   (dynamic_cast< SVGFirstTransform * >(transforms.getItem(numTransform - 1)) != 0)) {
			second = dynamic_cast< SVGSecondTransform * >(transforms.getItem(numTransform - 2));
		}
		
		// creating & inserting the transform if needed
		if((second == 0) && create) {
			second = new SVGSecondTransform();
			SVGTransformList &t = const_cast< SVGTransformList & >(transforms);
			t.insertItemBefore(second, place);
		}

		return second;
	}

	SVGThirdTransform *SVGTransformable::getThird(bool create) {
		const SVGTransformList &transforms = GETVAL(Transform);
		unsigned int numTransform = transforms.numberOfItems();
		SVGThirdTransform *third = 0;

		// place to insert the transform in the list if create is set
		unsigned int place = numTransform;

		// the transform #0 could be the one we are looking for
		if(numTransform > 0) {
			third = dynamic_cast< SVGThirdTransform * >(transforms.getItem(numTransform - 1));
			place -= 1;
		}
		// if it wasn't, it can be the #1 only if #0 is a SVGFirstTransform or a SVGSecondTransform
		if((numTransform > 1) && (third == 0) && 
			((dynamic_cast< SVGFirstTransform * >(transforms.getItem(numTransform - 1)) != 0) ||
			 (dynamic_cast< SVGSecondTransform * >(transforms.getItem(numTransform - 1)) != 0))) {
			third = dynamic_cast< SVGThirdTransform * >(transforms.getItem(numTransform - 2));
			place -= 1;
		}
		// if it wasn't, it can be the #2 only if #0 is a SVGFirstTransform and #1 is a SVGSecondTransform
		if((numTransform > 2) && (third == 0) && 
			(dynamic_cast< SVGFirstTransform * >(transforms.getItem(numTransform - 1)) != 0) &&
			(dynamic_cast< SVGSecondTransform * >(transforms.getItem(numTransform - 2)) != 0)) {
			third = dynamic_cast< SVGThirdTransform * >(transforms.getItem(numTransform - 3));
		}
		
		// creating & inserting the transform if needed
		if((third == 0) && create) {
			third = new SVGThirdTransform();
			SVGTransformList &t = const_cast< SVGTransformList & >(transforms);
			t.insertItemBefore(third, place);
		}

		return third;
	}

#endif

}
