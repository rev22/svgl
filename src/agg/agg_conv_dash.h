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
// conv_dash
//
//----------------------------------------------------------------------------
#ifndef AGG_CONV_DASH_INCLUDED
#define AGG_CONV_DASH_INCLUDED

#include "agg_basics.h"
#include "agg_gen_dash.h"
#include "agg_conv_generator.h"

namespace agg
{

    //------------------------------------------------------------------------
    template<class VertexSource, class Markers=null_markers> 
    struct conv_dash : public conv_generator<VertexSource, gen_dash, Markers>
    {
        typedef Markers marker_type;

        conv_dash(VertexSource& vs) : 
            conv_generator<VertexSource, gen_dash, Markers>(vs)
        {
        }

        void remove_all_dashes() 
        { 
            generator().remove_all_dashes(); 
        }

        void add_dash(double dash_len, double gap_len) 
        { 
            generator().add_dash(dash_len, gap_len); 
        }

        void dash_start(double ds) 
        { 
            generator().dash_start(ds); 
        }

        void shorten(double s) { generator().shorten(s); }
        double shorten() const { return generator().shorten(); }
    };


}

#endif
