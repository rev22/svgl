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
// class path_storage
//
//----------------------------------------------------------------------------
#ifndef AGG_PATH_STORAGE_INCLUDED
#define AGG_PATH_STORAGE_INCLUDED

#include "agg_basics.h"

namespace agg
{

    //------------------------------------------------------------------------
    // A concept of vertex sources and conversion pipelines.
    //
    // Before rendering we must collect and store the necessary number 
    // of vertices. There different approaches can be used, for example,
    // simple buffer in memory. But we also may want to perform different
    // kinds of conversions, for example, affine transformations, clipping,
    // making an outline, calculation of Minkowski sum, and so on. Many graphic
    // libraries do that, but the sequence of these conversions is often 
    // hardcoded, i.e., there's a number of settings such as set_rotate(), 
    // set_clip_box(), etc, which can be used or ommited, but anyway a particular
    // vertex suffers all these conversions in the strictly defined order.
    // Another approach is based on a number of functions or classes each 
    // of which performs only one kind of conversion (in fact, this is how the 
    // first approach looks inside). In this case we usually have to to store 
    // intermediate results as vertex arrays or something on each step of
    // the conversion. 
    //
    // AGG allows you to construct any custom pipelines, but at the same time 
    // does not require the use of any intermediate storages. Not possible?
    // Yes, it is possible with some exceptions which are not painful.
    // 
    // A concept of rendering assumes creating a rasterizer object, then
    // adding vertices into it, and finally render the filled polygon. 
    // The rasterizer has template method add_path which takes an abstract 
    // vertex source and "asks" it to give vertices one by one. This storage must 
    // support the interface of two methods: rewind() and vertex(). The latest 
    // method gives the rasterizer one vertex from the vertex source and 
    // returns special flag.
    //
    // When all verices are given, method vertex() returns path_cmd_stop. 
    // Such a concept allows you to avoid storing vertices anywhere,
    // in case, for example, if you know the analitical representation of your
    // figure and calculate vertices on demand. For example, we can
    // easily write a class that produces a sinewave, but does not use any arrays. 
    // This class can be used as a source of vertices either directly or via some
    // intermediate converters. The examples of such kind of classes you can find
    // in file agg_curves.h.
    //
    // The main conclusion of it is: Any class that supports interface of two 
    // mentioned above methods can be used as a vertex source.
    //
    // There're two kinds of these classes - initial sources and intermediate 
    // converters. The latest require a pointer to a preceeding source when
    // being constructed. All these classes have prefix "conv_". Classes 
    // path_storage, curve3, curve4 can be only initial sources.
    //
    // All converter classes are template ones, all initial sources are usualy 
    // regular ones. Template mechanism us used because it gives us more freedom.
    // All the vertex source class need to have is two-method interface. These
    // classes don't have to be derived from any particular base class, and, 
    // besides, it allows us to avoid using virtual calls which works much
    // slower than regular calls (They make the processor pipeline to resynchronize
    // because they are doubly indirect. BTW, pure C indirect calls, via a pointer
    // to a function work almost as fast as direct ones, because they have only
    // one level of indirection).
    //
    // Still, sometimes we have to use converters that have one common base
    // class and their interface methods are virtual. There is such base class
    // called pipe_conv. See agg_pipe_conv.h for details. In fact there's only 
    // wrappers that turn interface function into virtual. Every vertex source 
    // class has this wrapper.
    //------------------------------------------------------------------------


    //------------------------------------------------------------------------
    class path_storage
    {
        // Allocation parameters
        enum
        {
            block_shift = 8,
            block_size = 1 << block_shift,
            block_mask = block_size - 1,
            block_pool = 256
        };

    public:

        //--------------------------------------------------------------------
        class const_iterator
        {
            void vertex()
            {
                if(m_vertex_idx < m_path->total_vertices()) 
                {
                    m_vertex.cmd = m_path->vertex(m_vertex_idx, &m_vertex.x, &m_vertex.y);
                }
                else
                {
                    m_vertex.cmd = path_cmd_stop;
                    m_vertex.x = m_vertex.y = 0.0;
                }
            }

        public:
            const_iterator() {}
            const_iterator(unsigned cmd) { m_vertex.cmd = cmd; }
            const_iterator(const const_iterator& i) : 
                m_path(i.m_path),
                m_vertex_idx(i.m_vertex_idx),
                m_vertex(i.m_vertex) 
            {
            }

            const_iterator(const path_storage& p, unsigned id) : 
                m_path(&p),
                m_vertex_idx(id)
            {
                vertex();
            }

            const_iterator& operator++() 
            {
                ++m_vertex_idx;
                vertex();
                return *this;
            }

            const vertex_type& operator*() const { return m_vertex; }
            const vertex_type* operator->() const { return &m_vertex; }

            bool operator != (const const_iterator& i) 
            { 
                return m_vertex.cmd != i.m_vertex.cmd; 
            }

        private:
            const path_storage* m_path;
            unsigned            m_vertex_idx;
            vertex_type         m_vertex;
        };

        ~path_storage();
        path_storage();
        path_storage(const path_storage& ps);

        void remove_all();

        void rel_to_abs(double* x, double* y);

        void move_to(double x, double y);

        void line_to(double x, double y);

        void curve3(double x_ctrl,  double y_ctrl, 
                    double x_to,    double y_to);

        void curve4(double x_ctrl1, double y_ctrl1, 
                    double x_ctrl2, double y_ctrl2, 
                    double x_to,    double y_to);

        void end_poly(unsigned flags = path_flags_close);

        void close_polygon(unsigned flags = path_flags_none)
        {
            end_poly(path_flags_close | flags);
        }

        void add_poly(const double* vertices, unsigned num, 
                      bool solid_path = false,
                      unsigned end_flags = path_flags_none);

        void add_vertices(const double* vertices, unsigned num)
        {
            add_poly(vertices, num, path_flags_none);
        }

        template<class VertexSource> 
        void add_path(VertexSource& vs, 
                      unsigned path_id = 0, 
                      bool solid_path = false)
        {
            double x, y;
            unsigned cmd;
            vs.rewind(path_id);
            while(!is_stop(cmd = vs.vertex(&x, &y)))
            {
                if(is_move_to(cmd) && solid_path) cmd = path_cmd_line_to;
                add_vertex(x, y, cmd);
            }
        }

        unsigned start_new_path();

        void copy_from(const path_storage& ps);
        const path_storage& operator = (const path_storage& ps)
        {
            copy_from(ps);
            return *this;
        }


        unsigned total_vertices() const { return m_total_vertices; }
        unsigned vertex(unsigned idx, double* x, double* y) const
        {
            unsigned nb = idx >> block_shift;
            const double* pv = m_coord_blocks[nb] + ((idx & block_mask) << 1);
            *x = *pv++;
            *y = *pv;
            return m_cmd_blocks[nb][idx & block_mask];
        }
        unsigned command(unsigned idx) const
        {
            return m_cmd_blocks[idx >> block_shift][idx & block_mask];
        }

        void     rewind(unsigned path_id);
        unsigned vertex(double* x, double* y);

        const_iterator begin(unsigned id) const { return const_iterator(*this, id); }
        const_iterator begin()            const { return const_iterator(*this, 0); }
        const_iterator end()              const { return const_iterator(path_cmd_stop); }

        // Arrange the orientation of all the polygons. After calling this
        // method all the polygons will have the same orientation
        // determined by the new_orientation flag, i.e., 
        // path_flags_cw or path_flags_ccw
        unsigned arrange_orientations(unsigned path_id, path_flags_e new_orientation);
        void arrange_orientations_all_paths(path_flags_e new_orientation);
        
        // This function adds vertex with its flags directly. Since there's no 
        // checking for errors, keeping proper path integrity is the responsibility
        // of the caller. It can be said the function is "not very public". It's
        // used in methods move_to, line_to and so on and also in polygon and 
        // polyline converters.
        void add_vertex(double x, double y, unsigned cmd);

        // Allows you to modify vertex coordinates. The caller must know 
        // the index of the vertex. 
        void modify_vertex(unsigned idx, double x, double y)
        {
            double* pv = m_coord_blocks[idx >> block_shift] + ((idx & block_mask) << 1);
            *pv++ = x;
            *pv   = y;
        }

        // Allows you to modify vertex command. The caller must know 
        // the index of the vertex. 
        void modify_command(unsigned idx, unsigned cmd)
        {
            m_cmd_blocks[idx >> block_shift][idx & block_mask] = (unsigned char)cmd;
        }


    private:
        void allocate_block(unsigned nb);
        unsigned char* storage_ptrs(double** xy_ptr);
        unsigned perceive_polygon_orientation(unsigned idx, 
                                              double xs, double ys,
                                              unsigned* orientation);
        void reverse_polygon(unsigned start, unsigned end);

    private:
        unsigned        m_total_vertices;
        unsigned        m_total_blocks;
        unsigned        m_max_blocks;
        double**        m_coord_blocks;
        unsigned char** m_cmd_blocks;
        unsigned        m_iterator;
    };



    //------------------------------------------------------------------------
    inline unsigned path_storage::vertex(double* x, double* y)
    {
        if(m_iterator >= m_total_vertices) return path_cmd_stop;
        return vertex(m_iterator++, x, y);
    }


    //------------------------------------------------------------------------
    inline void path_storage::rel_to_abs(double* x, double* y)
    {
        if(m_total_vertices)
        {
            double x2;
            double y2;
            vertex(m_total_vertices - 1, &x2, &y2);
            *x += x2;
            *y += y2;
        }
    }


    //------------------------------------------------------------------------
    inline unsigned char* path_storage::storage_ptrs(double** xy_ptr)
    {
        unsigned nb = m_total_vertices >> block_shift;
        if(nb >= m_total_blocks)
        {
            allocate_block(nb);
        }
        *xy_ptr = m_coord_blocks[nb] + ((m_total_vertices & block_mask) << 1);
        return m_cmd_blocks[nb] + (m_total_vertices & block_mask);
    }


    //------------------------------------------------------------------------
    inline void path_storage::add_vertex(double x, double y, unsigned cmd)
    {
        double* coord_ptr = 0;
        unsigned char* cmd_ptr = storage_ptrs(&coord_ptr);
        *cmd_ptr = (unsigned char)cmd;
        *coord_ptr++ = x;
        *coord_ptr   = y;
        m_total_vertices++;
    }

    //------------------------------------------------------------------------
    inline void path_storage::move_to(double x, double y)
    {
        add_vertex(x, y, path_cmd_move_to);
    }


    //------------------------------------------------------------------------
    inline void path_storage::line_to(double x, double y)
    {
        add_vertex(x, y, path_cmd_line_to);
    }

}



#endif
