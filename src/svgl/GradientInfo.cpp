#include "GradientInfo.hpp"
#include "ExternalEntityManager.hpp"
#include "Context.hpp"
#include "getattr.hpp"

namespace svgl {

void
buildGradientInfo(svgl::Context* svglContext, svg::SVGGradientElement *gradient, GradientInfo& gradientInfo)
{
		if(gradientInfo.units == svg::SVGUnitTypes::SVG_UNIT_TYPE_UNKNOWN)
			gradientInfo.units = gradient->GETVAL(GradientUnits);
		if(gradientInfo.spreadMethod == svg::SVGGradientElement::SVG_SPREADMETHOD_UNKNOWN)
			gradientInfo.spreadMethod = gradient->GETVAL(SpreadMethod);
		
		if(!gradientInfo.transforms)
			gradientInfo.transforms = &gradient->GETVAL(GradientTransform);
				
		if(gradientInfo.stops.empty())
			gradient->getStopElements(&gradientInfo.stops);
		
		dom::string ref = gradient->GETVAL(Href);
		if (ref) {
			dom::Element* de = svglContext->externalEntityManager->get(ref, gradient->getOwnerDocument());
			//std::cerr << this << " " << de << __FL__;
			svg::SVGGradientElement *e = dynamic_cast<svg::SVGGradientElement*>(de);
			if(e) {
				buildGradientInfo(svglContext, e, gradientInfo);
			}
		}
}

}