#include <agg2glPoly.h>


void
draw(GLUtesselator * gluTess, const contour_type& contour, contour_type::size_type& it, const contour_type::size_type& end)
{
  gluTessBeginContour(gluTess);
  //std::cerr << "gluTessBeginContour" << std::endl;
  //  for(contour_type::const_iterator it = contour.begin(); it != contour.end(); ++it) {
  for(; it!=end; ++++++it) {
    //std::cerr << contour[it] << " " << contour[it+1] << std::endl;
    gluTessVertex(gluTess, const_cast<GLdouble*>(&contour[it]), const_cast<GLdouble*>(&contour[it]));
  }
  //std::cerr << "gluTessEndContour" << std::endl;
  gluTessEndContour(gluTess);
	
	}

