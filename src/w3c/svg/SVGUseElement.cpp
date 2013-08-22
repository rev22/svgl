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
#include <w3c/svg/SVGUseElement.hpp>
#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/ExternalEntityManager.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/svg/SVGSymbolElement.hpp>

#include <w3c/svg/SVGFilterElement.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

namespace svg {

  void
  SVGUseElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    bool reallyTraverse = preManageGlCompilation();

    if(!reallyTraverse)
      return;
		
    dom::string ref = GETVAL(Href);
    svg::SVGLength& xl = MYGETVAL(x);
    svg::SVGLength& yl = MYGETVAL(y);
    svg::SVGLength& widthl = MYGETVAL(width);
    svg::SVGLength& heightl = MYGETVAL(height);

    bool widthb = (widthl.GET_SIMPLE_VAL(UnitType) != SVGLength::SVG_LENGTHTYPE_UNKNOWN);
    bool heightb = (heightl.GET_SIMPLE_VAL(UnitType) != SVGLength::SVG_LENGTHTYPE_UNKNOWN);

    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);
    float width = svglContext->viewport_width, height=svglContext->viewport_height;
    if(widthb)
      width = svglContext->computeWidthLength(widthl);
    if(heightb)
      height = svglContext->computeHeightLength(heightl);

    //std::cerr << DBGVAR(widthb) << DBGVAR(width) << DBGVAR(heightb) << DBGVAR(height) << __FL__;

    if (ref) {
      dom::Element* de = svglContext->externalEntityManager->get(ref, getOwnerDocument());
      //std::cerr << this << " " << de << __FL__;
      SVGSymbolElement *svgsym = dynamic_cast<SVGSymbolElement*>(de);
      SVGSVGElement *svgsvg = dynamic_cast<SVGSVGElement*>(de);
      SVGElement* svge = dynamic_cast<SVGElement*>(de);
      if(!svge) {
				std::cerr << "unknown reference " << ref << " in '<use>'" __FL__;
				postManageGlCompilation();
				return;
      }

      SVGStylable * svgstylable = dynamic_cast<SVGStylable*>(de);
      const css::CSSStyleDeclaration * saveParentStyle;
      if(svgstylable) {
				css::CSSStyleDeclaration& style = const_cast<css::CSSStyleDeclaration&>(svgstylable->GET_SIMPLE_VAL(Style));
				saveParentStyle = style.getParent();
				style.setParent(&GET_SIMPLE_VAL(Style));
      }

      SVGFilterElement* filter_element = 0;
      filter_element = getSVGFilterElement(svglContext->externalEntityManager, getOwnerDocument());
      if(filter_element)
				filter_element->glPreTraverse(svglContext, glinfo, this);
	
      glPushMatrix();
      //svgl::GLInfo glinfonew = *glinfo;
      //SVGTransformable::glTraverse(svglContext, &glinfonew);
      SVGTransformable::glTraverse(svglContext, glinfo);
      glTranslatef(x,y,0);

      if(svgsym || svgsvg) {
				float oldwidth = svglContext->viewport_width;
				float oldheight = svglContext->viewport_height;
				if(svgsym) {
					SVGLength tmp;
					tmp.newValueSpecifiedUnits(SVGLength::SVG_LENGTHTYPE_PERCENTAGE,100);
					SVGLength wsvgl = tmp;
					SVGLength hsvgl = tmp;
					if(widthb)
						width = svglContext->computeWidthLength(widthl);
					else
						width = svglContext->computeWidthLength(wsvgl);
					if(heightb)
						height = svglContext->computeHeightLength(heightl);
					else
						height = svglContext->computeHeightLength(hsvgl);
					svglContext->setViewportWidthHeight(width, height);
					
					svgsym->glTraverseFromUse(svglContext, glinfo, width, height);
				}
				else {
					SVGLength wsvgl = svgsvg->GETVAL(Width);
					SVGLength hsvgl = svgsvg->GETVAL(Height);
					if(widthb)
						width = svglContext->computeWidthLength(widthl);
					else
						width = svglContext->computeWidthLength(wsvgl);
					if(heightb)
						height = svglContext->computeHeightLength(heightl);
					else
						height = svglContext->computeHeightLength(hsvgl);
					svglContext->setViewportWidthHeight(width, height);
					svgsvg->glTraverseFromUse(svglContext, glinfo, width, height);
				}
				svglContext->setViewportWidthHeight(oldwidth, oldheight);
      }
      else
				svge->glTraverse(svglContext, glinfo);

      glPopMatrix();

      if(filter_element)
				filter_element->glPostTraverse(svglContext, glinfo, this);

      if(svgstylable) {
				css::CSSStyleDeclaration& style = const_cast<css::CSSStyleDeclaration&>(svgstylable->GET_SIMPLE_VAL(Style));
				style.setParent(saveParentStyle);
      }
      postManageGlCompilation();

    }
  }


  void
  SVGUseElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    dom::string ref = GETVAL(Href);
    svg::SVGLength& xl = MYGETVAL(x);
    svg::SVGLength& yl = MYGETVAL(y);
    svg::SVGLength& widthl = MYGETVAL(width);
    svg::SVGLength& heightl = MYGETVAL(height);

    bool widthb = (widthl.GET_SIMPLE_VAL(UnitType) != SVGLength::SVG_LENGTHTYPE_UNKNOWN);
    bool heightb = (heightl.GET_SIMPLE_VAL(UnitType) != SVGLength::SVG_LENGTHTYPE_UNKNOWN);

    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);
    float width = svglContext->viewport_width, height=svglContext->viewport_height;
    if(widthb)
      width = svglContext->computeWidthLength(widthl);
    if(heightb)
      height = svglContext->computeHeightLength(heightl);

    if (ref) {
      dom::Element* de = svglContext->externalEntityManager->get(ref, getOwnerDocument());
      SVGSymbolElement *svgsym = dynamic_cast<SVGSymbolElement*>(de);
      SVGSVGElement *svgsvg = dynamic_cast<SVGSVGElement*>(de);
      SVGElement* svge = dynamic_cast<SVGElement*>(de);
      if(!svge) {
	std::cerr << "unknown reference " << ref << " in '<use>'" __FL__;
	return;
      }

      SVGStylable * svgstylable = dynamic_cast<SVGStylable*>(de);
      const css::CSSStyleDeclaration * saveParentStyle;
      if(svgstylable) {
	css::CSSStyleDeclaration& style = const_cast<css::CSSStyleDeclaration&>(svgstylable->GET_SIMPLE_VAL(Style));
	saveParentStyle = style.getParent();
	style.setParent(&GET_SIMPLE_VAL(Style));
      }
     
      glPushMatrix();
      //svgl::GLInfo glinfonew = *glinfo;
      //SVGTransformable::glTraverse(svglContext, &glinfonew);
      SVGTransformable::glTraverse(svglContext, glinfo);
      glTranslatef(x,y,0);
      glinfo->pushPickInfo(this);
      if(svgsym || svgsvg) {
	float oldwidth = svglContext->viewport_width;
	float oldheight = svglContext->viewport_height;
	svglContext->setViewportWidthHeight(width, height);
	if(svgsym)
	  svgsym->pickTraverseFromUse(svglContext, glinfo, width, height);
	else
	  svgsvg->pickTraverseFromUse(svglContext, glinfo, width, height);
	svglContext->setViewportWidthHeight(oldwidth, oldheight);
      }
      else
	svge->pickTraverse(svglContext, glinfo);
      glinfo->popPickInfo();
      glPopMatrix();

      if(svgstylable) {
	css::CSSStyleDeclaration& style = const_cast<css::CSSStyleDeclaration&>(svgstylable->GET_SIMPLE_VAL(Style));
	style.setParent(saveParentStyle);
      }
    }
  }

  void
  SVGUseElement::glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    dom::string ref = GETVAL(Href);
    svg::SVGLength& xl = MYGETVAL(x);
    svg::SVGLength& yl = MYGETVAL(y);
    svg::SVGLength& widthl = MYGETVAL(width);
    svg::SVGLength& heightl = MYGETVAL(height);

    bool widthb = (widthl.GET_SIMPLE_VAL(UnitType) != SVGLength::SVG_LENGTHTYPE_UNKNOWN);
    bool heightb = (heightl.GET_SIMPLE_VAL(UnitType) != SVGLength::SVG_LENGTHTYPE_UNKNOWN);

    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);
    float width = svglContext->viewport_width, height=svglContext->viewport_height;
    if(widthb)
      width = svglContext->computeWidthLength(widthl);
    if(heightb)
      height = svglContext->computeHeightLength(heightl);

    if (ref) {
      dom::Element* de = svglContext->externalEntityManager->get(ref, getOwnerDocument());
      SVGSymbolElement *svgsym = dynamic_cast<SVGSymbolElement*>(de);
      SVGElement* svge=0;      
      if(svgsym) {
      }
      else {
	svge = dynamic_cast<SVGElement*>(de);
	if(!svge) {
	  std::cerr << "unknown reference " << ref << " in '<use>'" __FL__;
	  return;
	}
      }
	
      glPushMatrix();
      SVGTransformable::glTraverse(svglContext, glinfo);
      glTranslatef(x,y,0);

      if(svgsym) {
	float oldwidth = svglContext->viewport_width;
	float oldheight = svglContext->viewport_height;
	svglContext->setViewportWidthHeight(width, height);
	//svgsym->pickTraverseFromUse(svglContext, &glinfonew, width, height);
	svglContext->setViewportWidthHeight(oldwidth, oldheight);
      }
      else
	svge->glTraverseBoundingBox(svglContext, glinfo);
      glPopMatrix();

    }
  }


  void
  SVGUseElement::registerUse(svgl::Context * svglContext)
  {
    dom::string ref = GETVAL(Href);
    if (ref) {
      dom::Element* de = svgl::ExternalEntityManager::getDefault()->get(ref, getOwnerDocument());
      svgl::ExternalEntityManager::getDefault()->addUsedBy(de, this);
    }
    
    // no child in a <use>
    //SVGElement::registerUse(svglContext);
  }

  bool
  SVGUseElement::areChildrenDamaged()
  {
    dom::string ref = GETVAL(Href);
    if (ref) {
      dom::Element* de = svgl::ExternalEntityManager::getDefault()->get(ref, getOwnerDocument());
      //std::cerr << this << " " << de << __FL__;
      SVGElement* svge = dynamic_cast<SVGElement*>(de);
      if(svge) {
	return svge->areChildrenDamaged();
      }
    }
    return false;
  }

  void
  SVGUseElement::damageChildren()
  {
    _damaged=true;

    dom::string ref = GETVAL(Href);
    if (ref) {
      dom::Element* de = svgl::ExternalEntityManager::getDefault()->get(ref, getOwnerDocument());
      //std::cerr << this << " " << de << __FL__;
      SVGElement* svge = dynamic_cast<SVGElement*>(de);
      if(svge) {
	svge->damageChildren();
      }
    }
  }


}
