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
// conv_generator
//
//----------------------------------------------------------------------------
#ifndef AGG_CONV_GENERATOR_INCLUDED
#define AGG_CONV_GENERATOR_INCLUDED

#include "agg_basics.h"
#include "agg_vertex_iterator.h"

namespace agg
{
    //------------------------------------------------------------------------
    struct null_markers
    {
        void remove_all() {}
        void add_vertex(double, double, unsigned) {}
        void prepare_src() {}

        void rewind(unsigned) {}
        unsigned vertex(double*, double*) { return path_cmd_stop; }

        typedef null_markers source_type;
        typedef vertex_iterator<source_type> iterator;
        iterator begin(unsigned id) { return iterator(*this, id); }
        iterator end() { return iterator(path_cmd_stop); }
    };


    //------------------------------------------------------------------------
    template<class VertexSource, 
             class Generator, 
             class Markers=null_markers> class conv_generator
    {
        enum status
        {
            initial,
            accumulate,
            generate
        };

    public:
        conv_generator(VertexSource& source) :
            m_source(&source), 
            m_status(initial)
        {
        }

        Generator& generator() { return m_generator; }
        const Generator& generator() const { return m_generator; }

        Markers& markers() { return m_markers; }
        const Markers& markers() const { return m_markers; }
        
        void rewind(unsigned id) 
        { 
            m_source->rewind(id); 
            m_status = initial;
        }

        unsigned vertex(double* x, double* y);

        typedef conv_generator<VertexSource, Generator, Markers> source_type;
        typedef vertex_iterator<source_type> iterator;
        iterator begin(unsigned id) { return iterator(*this, id); }
        iterator end() { return iterator(path_cmd_stop); }

    private:
        VertexSource* m_source;
        Generator     m_generator;
        Markers       m_markers;
        status        m_status;
        unsigned      m_last_cmd;
        double        m_start_x;
        double        m_start_y;
    };





    //------------------------------------------------------------------------
    template<class VertexSource, class Generator, class Markers> 
    unsigned conv_generator<VertexSource, Generator, Markers>::vertex(double* x, double* y)
    {
        unsigned cmd = path_cmd_stop;
        for(;;)
        {
            switch(m_status)
            {
            case initial:
                m_last_cmd = m_source->vertex(&m_start_x, &m_start_y);
                m_status = accumulate;
                m_markers.remove_all();

            case accumulate:
                if(is_stop(m_last_cmd)) return path_cmd_stop;

                m_generator.remove_all();
                m_generator.add_vertex(m_start_x, m_start_y, m_last_cmd);
                m_markers.add_vertex(m_start_x, m_start_y, path_cmd_move_to);

                for(;;)
                {
                    cmd = m_source->vertex(x, y);
                    if(is_vertex(cmd))
                    {
                        m_last_cmd = cmd;
                        if(is_move_to(cmd))
                        {
                            m_start_x = *x;
                            m_start_y = *y;
                            break;
                        }
                        m_generator.add_vertex(*x, *y, cmd);
                        m_markers.add_vertex(*x, *y, path_cmd_line_to);
                    }
                    else
                    {
                        if(is_stop(cmd))
                        {
                            m_last_cmd = path_cmd_stop;
                            break;
                        }
                        if(is_end_poly(cmd))
                        {
                            m_generator.add_vertex(*x, *y, cmd);
                            break;
                        }
                    }
                }
                m_generator.rewind(0);
                m_status = generate;

            case generate:
                cmd = m_generator.vertex(x, y);
                if(is_stop(cmd))
                {
                    m_status = accumulate;
                    break;
                }
                return cmd;
            }
        }
        return cmd;
    }

}

#endif
