#ifndef _utils_List_hpp_
#define _utils_List_hpp_

#include <vector>
#if SVGL_USE_GC
#include <utils/gc_alloc.hpp>
#endif

template <class T>
#if SVGL_USE_GC
class List : public std::vector<T,gc_alloc<T> >, virtual public gc
#else
class List : public std::vector<T>
#endif
{
public:
  virtual ~List() {}
    unsigned int getLength() const { return size(); }
    T item(unsigned int index) const { return (*this)[index]; }
};

#endif
