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
// POD array
//
//----------------------------------------------------------------------------
#ifndef AGG_ARRAY_INCLUDED
#define AGG_ARRAY_INCLUDED

#include <string.h>
#include "agg_basics.h"

namespace agg
{

    //------------------------------------------------------------------------
    // A simple class template to store Plain Old Data, similar to std::vector
    // The main difference is it doesn't reallocate memory but instead 
    // uses blocks of data of size (1 << S), that is, power of two. The 
    // data is NOT continuous in memory, so the only valid access method 
    // is operator [].
    //------------------------------------------------------------------------
    template<class T, unsigned S=6> class array
    {
        enum 
        {   
            block_shift = S,
            block_size  = 1 << block_shift,
            block_mask  = block_size - 1
        };

    public:
        typedef T value_type;

        ~array();
        array();

        void remove_all() { m_size = 0; }
        void add(const T& val);
        void modify_last(const T& val);
        void remove_last();

        unsigned size() const { return m_size; }

        const T& operator [] (unsigned idx) const
        {
            return m_blocks[idx >> block_shift][idx & block_mask];
        }

        T& operator [] (unsigned idx)
        {
            return m_blocks[idx >> block_shift][idx & block_mask];
        }

        const T& curr(unsigned idx) const
        {
            return (*this)[idx];
        }

        T& curr(unsigned idx)
        {
            return (*this)[idx];
        }

        const T& prev(unsigned idx) const
        {
            return (*this)[(idx + m_size - 1) % m_size];
        }

        T& prev(unsigned idx)
        {
            return (*this)[(idx + m_size - 1) % m_size];
        }

        const T& next(unsigned idx) const
        {
            return (*this)[(idx + 1) % m_size];
        }

        T& next(unsigned idx)
        {
            return (*this)[(idx + 1) % m_size];
        }

    private:
        void allocate_block(unsigned nb);
        T*   data_ptr();

        // Prohibit copying
        array(const array<T, S>&);
        const array<T, S>& operator = (const array<T, S>&);

        unsigned        m_size;
        unsigned        m_total_blocks;
        unsigned        m_max_blocks;
        T**             m_blocks;
    };




    //------------------------------------------------------------------------
    template<class T, unsigned S> array<T, S>::~array()
    {
        if(m_total_blocks)
        {
            T** blk = m_blocks + m_total_blocks - 1;
            while(m_total_blocks--)
            {
                delete [] *blk;
                --blk;
            }
            delete [] m_blocks;
        }
    }



    //------------------------------------------------------------------------
    template<class T, unsigned S> array<T, S>::array() :
        m_size(0),
        m_total_blocks(0),
        m_max_blocks(0),
        m_blocks(0)
    {
    }



    //------------------------------------------------------------------------
    template<class T, unsigned S>
    void array<T, S>::allocate_block(unsigned nb)
    {
        if(nb >= m_max_blocks) 
        {
            T** new_blocks = new T* [m_max_blocks + block_size];

            if(m_blocks)
            {
                memcpy(new_blocks, 
                       m_blocks, 
                       m_max_blocks * sizeof(T*));

                delete [] m_blocks;
            }
            m_blocks = new_blocks;
            m_max_blocks += block_size;
        }
        m_blocks[nb] = new T [block_size];
        m_total_blocks++;
    }



    //------------------------------------------------------------------------
    template<class T, unsigned S>
    inline T* array<T, S>::data_ptr()
    {
        unsigned nb = m_size >> block_shift;
        if(nb >= m_total_blocks)
        {
            allocate_block(nb);
        }
        return m_blocks[nb] + (m_size & block_mask);
    }



    //------------------------------------------------------------------------
    template<class T, unsigned S> 
    inline void array<T, S>::add(const T& val)
    {
        *data_ptr() = val;
        ++m_size;
    }


    //------------------------------------------------------------------------
    template<class T, unsigned S> 
    inline void array<T, S>::remove_last()
    {
        if(m_size) --m_size;
    }


    //------------------------------------------------------------------------
    template<class T, unsigned S> 
    void array<T, S>::modify_last(const T& val)
    {
        remove_last();
        add(val);
    }

}

#endif
