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
// basic definitions
//
//----------------------------------------------------------------------------

#ifndef AGG_BASICS_INCLUDED
#define AGG_BASICS_INCLUDED

//#include "dbg_new/agg_dbg_new.h"

namespace agg
{
    //------------------------------------------------------------------------
    typedef signed char    int8;
    typedef unsigned char  int8u;
    typedef signed short   int16;
    typedef unsigned short int16u;
    typedef signed int     int32;
    typedef unsigned int   int32u;


    //------------------------------------------------------------------------
    const double pi = 3.14159265358979323846;

    //------------------------------------------------------------------------
    inline double deg2rad(double deg)
    {
        return deg * pi / 180.0;
    }

    //------------------------------------------------------------------------
    inline double rad2deg(double rad)
    {
        return rad * 180.0 / pi;
    }
 
    //------------------------------------------------------------------------
    struct rect
    {
        int x1;
        int y1;
        int x2;
        int y2;

        rect() : x1(0), y1(0), x2(0), y2(0) {}
        rect(int x1_, int y1_, int x2_, int y2_) : 
           x1(x1_), y1(y1_), x2(x2_), y2(y2_) {}

        const rect& normalize()
        {
            int t;
            if(x1 > x2) { t = x1; x1 = x2; x2 = t; }
            if(y1 > y2) { t = y1; y1 = y2; y2 = t; }
            return *this;
        }

        bool clip(const rect& r)
        {
            if(x1 < r.x1) x1 = r.x1;
            if(y1 < r.y1) y1 = r.y1;
            if(x2 > r.x2) x2 = r.x2;
            if(y2 > r.y2) y2 = r.y2;
            return x1 <= x2 && y1 <= y2;
        }
    };

    //------------------------------------------------------------------------
    enum path_commands_e
    {
        path_cmd_stop     = 0,
        path_cmd_move_to  = 1,
        path_cmd_line_to  = 2,
        path_cmd_curve3   = 3,
        path_cmd_curve4   = 4,
        path_cmd_end_poly = 6,
        path_cmd_mask     = 0x0F
    };

    //------------------------------------------------------------------------
    enum path_flags_e
    {
        path_flags_none  = 0,
        path_flags_ccw   = 0x10,
        path_flags_cw    = 0x20,
        path_flags_close = 0x40,
        path_flags_mask  = 0xF0
    };

    //------------------------------------------------------------------------
    inline bool is_vertex(unsigned c)   
    { 
        return c >= path_cmd_move_to && c < path_cmd_end_poly; 
    }
    inline bool is_stop(unsigned c)     { return c == path_cmd_stop;    }
    inline bool is_move_to(unsigned c)  { return c == path_cmd_move_to; }
    inline bool is_line_to(unsigned c)  { return c == path_cmd_line_to; }
    inline bool is_curve3(unsigned c)   { return c == path_cmd_curve3;  }
    inline bool is_curve4(unsigned c)   { return c == path_cmd_curve4;  }
    inline bool is_end_poly(unsigned c) { return (c & path_cmd_mask) == path_cmd_end_poly; }
    inline bool is_close(unsigned c)    
    { 
        return (c & ~(path_flags_cw | path_flags_ccw)) == 
               (path_cmd_end_poly | path_flags_close); 
    }
    inline bool is_next_poly(unsigned c)
    {
        return is_stop(c) || is_move_to(c) || is_end_poly(c);
    }

    //------------------------------------------------------------------------
    inline bool is_cw(unsigned c)       { return (c & path_flags_cw) != 0;  }
    inline bool is_ccw(unsigned c)      { return (c & path_flags_ccw) != 0; }
    inline bool is_oriented(unsigned c) { return (c & (path_flags_cw | path_flags_ccw)) != 0; }
    inline bool is_closed(unsigned c)   { return (c & path_flags_close) != 0; }
    inline unsigned get_close_flag(unsigned c)  { return c & path_flags_close; }
    inline unsigned clear_orientation(unsigned c) { return c & ~(path_flags_cw | path_flags_ccw); }
    inline unsigned get_orientation(unsigned c) { return c & (path_flags_cw | path_flags_ccw); }
    inline unsigned set_orientation(unsigned c, unsigned o) { return clear_orientation(c) | o; }


//    //------------------------------------------------------------------------
//    inline unsigned path_cmd(unsigned c)    { return c & path_cmd_mask;   }
//    inline unsigned path_flags(unsigned c)  { return c & path_flags_mask; }
//    inline bool     is_arc(unsigned c)      { return path_cmd(c) == path_cmd_arc;     }
//    inline bool     is_curve(unsigned c)    { return path_cmd(c) >= path_cmd_curve3 &&  path_cmd(c) <= path_cmd_arc; }
//
//    inline unsigned close_polygon(unsigned c)     { return c | path_flags_close; }
//    inline unsigned unclose_polygon(unsigned c)   { return c & ~path_flags_close; }
//    inline unsigned get_close_flag(unsigned c)    { return c & path_flags_close; }
//
//    inline unsigned set_cw(unsigned c)            { return (c & ~path_flags_ccw) | path_flags_cw; }
//    inline unsigned set_ccw(unsigned c)           { return (c & ~path_flags_cw) | path_flags_ccw; }
//    inline path_flags_e get_orientation(unsigned c) { return path_flags_e(c & (path_flags_cw | path_flags_ccw));  }



    //------------------------------------------------------------------------
    struct vertex_type
    {
        double   x, y;
        unsigned cmd;
    };


}


#endif

