#include "glPoly.h"
#include <agg/agg_basics.h>

typedef std::vector<GLdouble> contour_type;

void
draw(GLUtesselator * gluTess, const contour_type& contour, contour_type::size_type& it, const contour_type::size_type& end);

struct toto {
	contour_type::size_type beg,end;
};

template<class agg_stroke>
void
agg2glPoly(agg_stroke& stroke, glPoly& gglPoly, GLUtesselator * gluTess)
{
  double x,y;
  unsigned int cmd;
  stroke.rewind(0);

  combineTab_index=0;
	
  contour_type contour;
	std::vector<toto> begcontours;
  contour_type::size_type beg = 0, end=0;

  do {
    cmd = stroke.vertex(&x,&y);

		if(agg::is_next_poly(cmd)) {
			toto t;
			t.beg=beg;
			t.end=end;
			begcontours.push_back(t);
			beg=end;
			//std::cerr << beg << " " << end << std::endl;
    }
		
    if(agg::is_move_to(cmd & agg::path_cmd_mask) || agg::is_line_to(cmd & agg::path_cmd_mask)) {
#if 1
      contour.push_back(x);
      contour.push_back(y);
			contour.push_back(0);
			++++++end;
#else
      contour[end++] = x;
      contour[end++] = y;
      contour[end++] = 0;
#endif
			
    } else { //stop_cmd
			//std::cerr << "unexpected agg_path command " <<  cmd << std::endl;
		}
  } while(cmd != agg::path_cmd_stop);
	
	gluTessBeginPolygon(gluTess, &gglPoly);
	for(std::vector<toto>::iterator it = begcontours.begin();
			it != begcontours.end();
			++it) {
		draw(gluTess, contour, it->beg, it->end);		
	}
	//std::cerr << "gluTessEndPolygon" << std::endl;
  gluTessEndPolygon(gluTess);
}
