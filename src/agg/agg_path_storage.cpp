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
// Class path_storage
//
//----------------------------------------------------------------------------
#include <string.h>
#include <math.h>
#include "agg_path_storage.h"
#include "agg_math.h"


namespace agg
{

    //------------------------------------------------------------------------
    path_storage::~path_storage()
    {
        if(m_total_blocks)
        {
            double** coord_blk = m_coord_blocks + m_total_blocks - 1;
            while(m_total_blocks--)
            {
                delete [] *coord_blk;
                --coord_blk;
            }
            delete [] m_coord_blocks;
        }
    }


    //------------------------------------------------------------------------
    path_storage::path_storage() :
        m_total_vertices(0),
        m_total_blocks(0),
        m_max_blocks(0),
        m_coord_blocks(0),
        m_cmd_blocks(0),
        m_iterator(0)
    {
    }


    //------------------------------------------------------------------------
    path_storage::path_storage(const path_storage& ps) : 
        m_total_vertices(0),
        m_total_blocks(0),
        m_max_blocks(0),
        m_coord_blocks(0),
        m_cmd_blocks(0),
        m_iterator(0)
    {
        copy_from(ps);
    }


    //------------------------------------------------------------------------
    void path_storage::remove_all()
    {
        m_total_vertices = 0;
        m_iterator = 0;
    }


    //------------------------------------------------------------------------
    void path_storage::copy_from(const path_storage& ps)
    {
        remove_all();
        unsigned i;
        for(i = 0; i < ps.total_vertices(); i++)
        {
            double x, y;
            unsigned cmd = ps.vertex(i, &x, &y);
            add_vertex(x, y, cmd);
        }
    }


    //------------------------------------------------------------------------
    void path_storage::allocate_block(unsigned nb)
    {
        if(nb >= m_max_blocks) 
        {
            double** new_coords = 
                new double* [(m_max_blocks + block_pool) * 2];

            unsigned char** new_cmds = 
                (unsigned char**)(new_coords + m_max_blocks + block_pool);

            if(m_coord_blocks)
            {
                memcpy(new_coords, 
                       m_coord_blocks, 
                       m_max_blocks * sizeof(double*));

                memcpy(new_cmds, 
                       m_cmd_blocks, 
                       m_max_blocks * sizeof(unsigned char*));

                delete [] m_coord_blocks;
            }
            m_coord_blocks = new_coords;
            m_cmd_blocks = new_cmds;
            m_max_blocks += block_pool;
        }
        m_coord_blocks[nb] = 
            new double [block_size * 2 + 
                        block_size / 
                        (sizeof(double) / sizeof(unsigned char))];

        m_cmd_blocks[nb]  = 
            (unsigned char*)(m_coord_blocks[nb] + block_size * 2);

        m_total_blocks++;
    }


    //------------------------------------------------------------------------
    void path_storage::rewind(unsigned path_id)
    {
        m_iterator = path_id; 
    }

    //------------------------------------------------------------------------
    void path_storage::curve3(double x_ctrl, double y_ctrl, 
                              double x_to,   double y_to)
    {
        add_vertex(x_ctrl, y_ctrl, path_cmd_curve3);
        add_vertex(x_to,   y_to,   path_cmd_curve3);
    }


    //------------------------------------------------------------------------
    void path_storage::curve4(double x_ctrl1, double y_ctrl1, 
                                     double x_ctrl2, double y_ctrl2, 
                                     double x_to,    double y_to)
    {
        add_vertex(x_ctrl1, y_ctrl1, path_cmd_curve4);
        add_vertex(x_ctrl2, y_ctrl2, path_cmd_curve4);
        add_vertex(x_to,    y_to,    path_cmd_curve4);
    }



    //------------------------------------------------------------------------
    void path_storage::end_poly(unsigned flags)
    {
        if(m_total_vertices)
        {
            if(is_vertex(command(m_total_vertices - 1)))
            {
                add_vertex(0.0, 0.0, path_cmd_end_poly | flags);
            }
        }
    }


    //------------------------------------------------------------------------
    unsigned path_storage::start_new_path()
    {
        if(m_total_vertices)
        {
            if(!is_stop(command(m_total_vertices - 1)))
            {
                add_vertex(0.0, 0.0, path_cmd_stop);
            }
        }
        return m_total_vertices;
    }


    //------------------------------------------------------------------------
    void path_storage::add_poly(const double* vertices, unsigned num,
                                bool solid_path, unsigned end_flags)
    {
        if(num)
        {
            if(!solid_path)
            {
                move_to(vertices[0], vertices[1]);
                vertices += 2;
                --num;
            }
            while(num--)
            {
                line_to(vertices[0], vertices[1]);
                vertices += 2;
            }
            if(end_flags) end_poly(end_flags);
        }
    }


    //------------------------------------------------------------------------
    unsigned path_storage::perceive_polygon_orientation(unsigned idx,
                                                        double xs, double ys,
                                                        unsigned* orientation)
    {
        unsigned i;
        double sum = 0.0;
        double x, y, xn, yn;

        x = xs;
        y = ys;
        for(i = idx; i < m_total_vertices; ++i)
        {
            if(is_next_poly(vertex(i, &xn, &yn))) break;
            sum += x * yn - y * xn;
            x = xn;
            y = yn;
        }
        if(i > idx) sum += x * ys - y * xs;
        *orientation = path_flags_none;
        if(sum != 0.0)
        {
            *orientation = (sum < 0.0) ? path_flags_cw : path_flags_ccw;
        }
        return i;
    }


    //------------------------------------------------------------------------
    void path_storage::reverse_polygon(unsigned start, unsigned end)
    {
        unsigned i;
        unsigned tmp_cmd = command(start);
        
        // Shift all commands to one position
        for(i = start; i < end; i++)
        {
            modify_command(i, command(i + 1));
        }

        // Assign starting command to the ending command
        modify_command(end, tmp_cmd);

        // Reverse the polygon
        while(end > start)
        {
            unsigned start_nb = start >> block_shift;
            unsigned end_nb   = end   >> block_shift;
            double* start_ptr = m_coord_blocks[start_nb] + ((start & block_mask) << 1);
            double* end_ptr   = m_coord_blocks[end_nb]   + ((end   & block_mask) << 1);
            double tmp_xy;

            tmp_xy       = *start_ptr;
            *start_ptr++ = *end_ptr;
            *end_ptr++   = tmp_xy;

            tmp_xy       = *start_ptr;
            *start_ptr   = *end_ptr;
            *end_ptr     = tmp_xy;

            tmp_cmd = m_cmd_blocks[start_nb][start & block_mask];
            m_cmd_blocks[start_nb][start & block_mask] = m_cmd_blocks[end_nb][end & block_mask];
            m_cmd_blocks[end_nb][end & block_mask] = (unsigned char)tmp_cmd;

            ++start;
            --end;
        }
    }


    //------------------------------------------------------------------------
    unsigned path_storage::arrange_orientations(unsigned path_id, 
                                                path_flags_e new_orientation)
    {
        unsigned end = m_total_vertices;
        if(m_total_vertices && new_orientation != path_flags_none)
        {
            unsigned start = path_id;

            double xs, ys;
            //unsigned cmd = vertex(start, &xs, &ys);
            unsigned inc = 0;
            for(;;)
            {
                unsigned orientation;
                end = perceive_polygon_orientation(start + 1, xs, ys, 
                                                   &orientation);
                if(end > start + 2 &&
                   orientation && 
                   orientation != unsigned(new_orientation))
                {
                    reverse_polygon(start + inc, end - 1);
                }
                if(end >= m_total_vertices) break;
                unsigned cmd = command(end);
                if(is_stop(cmd)) 
                {
                    ++end;
                    break;
                }
                if(is_end_poly(cmd))
                {
                    inc = 1;
                    modify_command(end, set_orientation(cmd, orientation));
                }
                else
                {
                    cmd = vertex(++end, &xs, &ys);
                    inc = 0;
                }
                start = end;
            }
        }
        return end;
    }



    //------------------------------------------------------------------------
    void path_storage::arrange_orientations_all_paths(path_flags_e new_orientation)
    {
        if(new_orientation != path_flags_none)
        {
            unsigned start = 0;
            while(start < m_total_vertices)
            {
                start = arrange_orientations(start, new_orientation);
            }
        }
    }



}

