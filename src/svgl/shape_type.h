#ifndef __shape_type__
#define __shape_type__

#include <agg/agg_path_storage.h>
#include <agg/agg_conv_stroke.h>
#include <agg/agg_conv_dash.h>
#include <agg/agg_conv_curve.h>
//#include "agg_basics.h"

struct shape_type {
	typedef agg::conv_curve<agg::path_storage> contour_type;
	typedef agg::conv_stroke<contour_type> plainstroke_type;
	typedef agg::conv_dash<contour_type> dash_type;
	typedef agg::conv_stroke<dash_type> dashstroke_type;	
	
	agg::path_storage * _path;
	contour_type * _contour;
	plainstroke_type * _plain;
	dashstroke_type * _dash;
	
	shape_type() : _path(0), _contour(0), _plain(0), _dash(0) {}
	
	void line_join(agg::gen_stroke::line_join_e join) {
		if(_plain)
			_plain->line_join(join);
		if(_dash)
			_dash->line_join(join);
	}
	
	void line_cap(agg::gen_stroke::line_cap_e cap) {
		if(_plain)
			_plain->line_cap(cap);
		if(_dash)
			_dash->line_cap(cap);
	}
	
	void miter_limit(double v) {
		if(_plain)
			_plain->miter_limit(v);
		if(_dash)
			_dash->miter_limit(v);
	}
	
  void width(double v) {
		if(_plain)
			_plain->width(v);
		if(_dash)
			_dash->width(v);
	}
	
	void rewind(unsigned int v) {
		if(_plain)
			_plain->rewind(v);
		if(_dash)
			_dash->rewind(v);
	}
	
	unsigned vertex(double *x, double *y) {
		if(_plain)
			return _plain->vertex(x,y);
		if(_dash)
			return _dash->vertex(x,y);
		throw 1;
	}
	
	
};


#endif // __shape_type__

