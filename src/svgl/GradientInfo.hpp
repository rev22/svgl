#ifndef __svgl_GradientInfo__
#define __svgl_GradientInfo__

#include <w3c/svg/SVGGradientElement.hpp>

namespace svgl {

	struct GradientInfo {
		svg::SVGUnitTypes::SVG_UNIT_TYPE units;
		const svg::SVGTransformList *transforms;
		svg::SVGGradientElement::SVG_SPREADMETHOD spreadMethod;
		svgl::StopElements stops;
		
		GradientInfo() {
			units = svg::SVGUnitTypes::SVG_UNIT_TYPE_UNKNOWN;
			transforms = 0;
			spreadMethod = svg::SVGGradientElement::SVG_SPREADMETHOD_UNKNOWN;
		}
};

void
buildGradientInfo(svgl::Context* svglContext, svg::SVGGradientElement *gradient, GradientInfo& gradientInfo);


}

#endif
