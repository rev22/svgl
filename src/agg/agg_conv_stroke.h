//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.0 
// Copyright (C) 2002 Maxim Shemanarev (McSeem)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// conv_stroke
//
//----------------------------------------------------------------------------
#ifndef AGG_CONV_STROKE_INCLUDED
#define AGG_CONV_STROKE_INCLUDED

#include "agg_basics.h"
#include "agg_gen_stroke.h"
#include "agg_conv_generator.h"

namespace agg
{

    //------------------------------------------------------------------------
    template<class VertexSource, class Markers=null_markers> 
    struct conv_stroke : 
    public conv_generator<VertexSource, gen_stroke, Markers>
    {
        typedef Markers marker_type;

        conv_stroke(VertexSource& vs) : 
            conv_generator<VertexSource, gen_stroke, Markers>(vs)
        {
        }

        void line_cap(gen_stroke::line_cap_e lc)   { generator().line_cap(lc);  }
        void line_join(gen_stroke::line_join_e lj) { generator().line_join(lj); }

        gen_stroke::line_cap_e  line_cap()  const { return generator().line_cap();  }
        gen_stroke::line_join_e line_join() const { return generator().line_join(); }

        void width(double w) { generator().width(w); }
        void miter_limit(double ml) { generator().miter_limit(ml); }
        void miter_limit_theta(double t) { generator().miter_limit_theta(t); }
        void approximation_scale(double as) { generator().approximation_scale(as); }

        double width() const { return generator().width(); }
        double miter_limit() const { return generator().miter_limit(); }
        double approximation_scale() const { return generator().approximation_scale(); }

        void shorten(double s) { generator().shorten(s); }
        double shorten() const { return generator().shorten(); }

    };

}

#endif
