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
#include <w3c/svg/SVGAnimateColorElement.hpp>
#include <w3c/svg/SVGStylable.hpp>
#include <svgl/AnimationInfo.hpp>
#include <svgl/AnimateAdapter.hpp>
#include <w3c/css/RGBColor.hpp>
#include <utils/StringUtils.hpp>
#include <vector>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

namespace svg {
	
  extern
  bool
  getRGBColor(const unicode::String* s_ptr, css::Color** pcolor, unicode::String::size_type beg=0, unicode::String::size_type end=unicode::String::npos);
	
	
  extern unicode::String* fill_attribute_string;
	
  bool
		get_color_list(const unicode::String* in_ptr, std::vector<css::RGBColor*>* v)
  {
			if(!in_ptr)
				return false;
			const unicode::String& in =  *in_ptr;
			unicode::String::size_type len = in.getLength();
			if (!len)
				return false;
			
			unicode::String::size_type beg = unicode::eat_whitespaces(in);
			static const unicode::String& sep = *unicode::String::internString(" \t\n\r,;");
			
			while(beg<len) {
				unicode::String::size_type pos = unicode::find_among(in, sep, beg);
				css::Color* c=0;
				if(!getRGBColor(&in, &c, beg, pos)) {
					return false;
				}
				css::RGBColor* crgb=dynamic_cast<css::RGBColor*>(c);
				if(!crgb) {
					std::cerr << "not an RGB color " << __FL__;
					return false;
				}
				v->push_back(crgb);
				beg = unicode::eat_whitespaces(in, pos);
			}
			return true;
  }
	
  inline
		const css::RGBColor
		operator + (const css::RGBColor& c1, const css::RGBColor& c2)
  {
			css::RGBColor c=c1;
			c.r+=c2.r;
			c.g+=c2.g;
			c.b+=c2.b;
			return c;
  }
	
  inline
		const css::RGBColor
		operator - (const css::RGBColor& c1, const css::RGBColor& c2)
  {
			css::RGBColor c=c1;
			c.r-=c2.r;
			c.g-=c2.g;
			c.b-=c2.b;
			return c;
  }
	
  inline
		const css::RGBColor
		operator * (const css::RGBColor& c1, float s)
  {
			css::RGBColor c=c1;
			c.r*=s;
			c.g*=s;
			c.b*=s;
			return c;
  }
	
  inline
		std::ostream&
		operator << (std::ostream& out, const css::RGBColor& c)
  {
			out << "(" << c.r << "," << c.g << "," << c.b << ")";
			return out;
  }
	
	
  void
		SVGAnimateColorElement::animationTraverse(svgl::AnimationInfo* animinfo)
  {
			unicode::String* attrName = GET_SIMPLE_VAL(AttributeName);
			SVG_ANIMATION_ATTRIBUTE_TYPE attrType = GET_SIMPLE_VAL(AttributeType);
			
			_elementToAnimate = animinfo->element;
			_attributeToAnimate=0;
			css::CSSStyle::const_PaintType savedColor;
			
			switch (attrType) {
				case SVG_ANIMATION_ATTRIBUTE_TYPE_AUTO:
				case SVG_ANIMATION_ATTRIBUTE_TYPE_XML:
				{
					_attributeToAnimate = _elementToAnimate->getAttributeByName(attrName);
				}
					break;
				case SVG_ANIMATION_ATTRIBUTE_TYPE_CSS:
				{
					SVGStylable * svgstylable = dynamic_cast<SVGStylable*>(_elementToAnimate);
					if(svgstylable) {
						savedColor = svgstylable->getStyle().getValue().getconst_PaintType(attrName);	    
						_attributeToAnimate = svgstylable->getStyle().getValue().getNewPatchStyle_const_PaintType(attrName);
					}
				}
				default:
					break;
			}
			
			if(_attributeToAnimate==0) {
				std::cerr << "attribute '" << attrName << "' does not exist for element " << _elementToAnimate->getDtdElement() << __FL__;
				return;
			}
			
			if(!_attributeToAnimate->canBeSetAsColor()) {
				_attributeToAnimate=0;
				std::cerr << "attribute '" << attrName << "' in element '" << _elementToAnimate->getDtdElement() << "' can not be set as color" << __FL__;
				return;
			}
			
			const unicode::String* values = GET_SIMPLE_VAL(Values);
			std::vector<css::RGBColor*> colors;
			bool valuesb = get_color_list(values, &colors);
			if(values && values->getLength() && !valuesb) {
				std::cerr << "invalid color values \"" << values << "\" in 'animateColor'" __FL__;
			}
#if 0
			std::cerr << "-- values" << __FL__;
			for(unsigned int t=0; t<values.size(); ++t)
				std::cerr << values[t] << " ";
			std::cerr << std::endl;
#endif
			
			if(colors.size()) {
				
				const SVGNumberList& keytimes = GET_SIMPLE_VAL(KeyTimes);
#if 0
				std::cerr << "** keytimes " << __FL__;
#endif
				if(keytimes.numberOfItems()) {
					if(keytimes.numberOfItems()!=colors.size()) {
						std::cerr << "'keyTimes' and 'values' have different size" << __FL__;
						return;
					}
					for(std::vector<float>::size_type i=0;
							i<colors.size();
							++i) {
						_keyFrames.push_back(KeyFrame(*(colors[i]), keytimes.getItem(i).GET_SIMPLE_VAL(Value)));
						//	  std::cerr << values[i] << " " << keytimes[i];
					}
					//	std::cerr << std::endl;
					
				}
				else {
					for(std::vector<float>::size_type i=0;
							i<colors.size();
							++i) {
						float t=i/((float)colors.size()-1);
						_keyFrames.push_back(KeyFrame(*(colors[i]), t));
					}
				}
				
			}
			else {
				const unicode::String* fromstr = GET_SIMPLE_VAL(From);
				const unicode::String* tostr = GET_SIMPLE_VAL(To);
				const unicode::String* bystr = GET_SIMPLE_VAL(By);
				//std::cerr << DBGVAR(attrName) << DBGVAR(attrType) << DBGVAR(fromstr) << DBGVAR(tostr) << __FL__;
				
				css::RGBColor from, to, by;
				bool fromb=false, tob=false, byb=false;
				
				css::Color *c;
				if(fromstr) {
					fromb = getRGBColor(fromstr, &c);
					css::RGBColor *tmp = dynamic_cast<css::RGBColor*>(c);
					if(tmp)
						from = *tmp;
					else
						fromb = false;
				}
				if(tostr) {
					tob = getRGBColor(tostr, &c);
					css::RGBColor *tmp = dynamic_cast<css::RGBColor*>(c);
					if(tmp)
						to = *tmp;
					else
						tob = false;
				}
				if(bystr) {
					byb = getRGBColor(bystr, &c);
					css::RGBColor *tmp = dynamic_cast<css::RGBColor*>(c);
					if(tmp)
						by = *tmp;
					else
						byb = false;
				}
				
				// std::cerr << DBGVAR(fromb) << DBGVAR(tob) << DBGVAR(byb) << __FL__;
				
				if( !(fromb||tob||byb) ) {
					std::cerr << "'from', 'to' and 'by' are unset" << __FL__;
					return;
				}
				
				if(!fromb) {
					fromb=true;
					// FIXME
					const css::RGBColor * tmp = dynamic_cast<const css::RGBColor*>(savedColor);
					if(tmp)
						from = *tmp;
					else {
						std::cerr << "'from' is not rgb, set it to black" << __FL__;
						from.r=from.g=from.b=1;
					}
				}
				if(!tob) {
					if(byb) {
						to=from+by;
					}
#if 0
					else {
						STDDBG;
						return;
					}
#endif
				}
				
				if(fromb)
					_keyFrames.push_back(KeyFrame(from, 0));
				_keyFrames.push_back(KeyFrame(to, 1));
				
			}
			
#if 0
			STDDBG;
			for(unsigned int j=0; j<_keyFrames.size(); ++j) {
				std::cerr << "(" << _keyFrames[j].value << "," << _keyFrames[j].time << ")";
			}
			std::cerr << std::endl;
#endif

						//std::cerr << DBGVAR(from) << DBGVAR(to) << DBGVAR(newvalue) << DBGVAR(timeRatio) << DBGVAR(deltatime) << __FL__;
			if(!_keyFrames.empty()) {
				css::RGBColor & tmp = _keyFrames.front().value;
				// std::cerr << DBGVAR(tmp) << std::endl;
				_attributeToAnimate->setAsColor(&tmp);
			}
			
			SVGAnimationElement::animationTraverse(animinfo);
			
  }
	
  void
		SVGAnimateColorElement::tick(svgl::Animation::TickInfo* tickinfo)
  {
			if(!_attributeToAnimate) {
				std::cerr << "no attribute to animate" << __FL__;
				return;
			}
			
			if(!tickinfo->dur) {
				std::cerr << "invalid tickinfo->dur " << tickinfo->dur << __FL__;
				return;
			}
			// std::cerr << DBGVAR(tickinfo->totalElapsed) << DBGVAR(tickinfo->dur) << __FL__;
			float timeRatio = (tickinfo->totalElapsed/(float)tickinfo->dur);
			
			if(_keyFrames.size()<2 ) {
				// from is not set
				// not very reliable FIXME
				std::cerr << "invalid keyFrames" << __FL__;
				return;
			}
			
#if 0
			// useless, update in other anim...
			// FIXME
			if(_keyFrames.size()==1 ) {
				// from is not set
				// not very reliable FIXME
				//double from = _attributeToAnimate->getBaseVal(); // now it has been computed
				css::RGBColor from;
				KeyFrame toframe = _keyFrames[0];
				_keyFrames[0]=KeyFrame(from, 0);
				_keyFrames.push_back(toframe);
			}
#endif
			
			std::vector<float>::size_type i=1;
			for(;
					i<_keyFrames.size();
					++i) {
				if( _keyFrames[i].time>=timeRatio) {	
					break;
				}
			}
			// if the first one is non-zero
			// is it possible ??
			if(i==0) {
				std::cerr << "_keyFrames[0].time (" << _keyFrames[0].time <<") > timeRatio (" << timeRatio << ")" __FL__;
				return;
			}
			
			css::RGBColor from = _keyFrames[i-1].value;
			css::RGBColor to = _keyFrames[i].value;
			
			timeRatio -= _keyFrames[i-1].time;
			
			css::RGBColor newvalue;
			float deltatime = _keyFrames[i].time -_keyFrames[i-1].time;
			if(deltatime<=.00001) {
				STDDBG;
				newvalue=to;
			}
			else {
				timeRatio /= deltatime;
				newvalue = from + (to-from)*timeRatio;
			}
			
			//std::cerr << DBGVAR(from) << DBGVAR(to) << DBGVAR(newvalue) << DBGVAR(timeRatio) << DBGVAR(deltatime) << __FL__;
			{
				css::RGBColor * tmp = new css::RGBColor(newvalue);
				// std::cerr << DBGVAR(tmp) << std::endl;
				_attributeToAnimate->setAsColor(tmp);
			}
  }
	
  void
		SVGAnimateColorElement::reset()
  {
			if(_attributeToAnimate) {
				css::RGBColor * tmp = new css::RGBColor;
				// FIXME
				tmp->r=tmp->g=tmp->b=0;
				// std::cerr << DBGVAR(tmp) << std::endl;
				_attributeToAnimate->setAsColor(tmp);
			}
  }
	
	
}

