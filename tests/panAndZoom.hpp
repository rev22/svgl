#ifndef __panAndZoom__
#define __panAndZoom__

namespace svg {
  class SVGTranslateTransform;
  class SVGScaleTransform;
	class SVGGElement;
	class SVGDocument;
}

namespace panAndZoom {

  extern svg::SVGTranslateTransform * panTransform;
  extern svg::SVGScaleTransform * zoomTransform;
	
	svg::SVGGElement*
	setup(svg::SVGDocument *doc);

  void
	motion(int x, int y);

  void
  button(int num, int state, int x, int y);
	
	void
	updatePan(int x, int y);
	
	void
	updateZoom(int x, int y);
}


#endif //__panAndZoom__


