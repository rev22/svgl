#ifndef _gc_alloc_hpp
#define _gc_alloc_hpp

#include <vector>
#include <gc_cpp.h>

template <class T>
class gc_alloc {
public:
  typedef size_t	size_type;
  typedef ptrdiff_t	difference_type;
  typedef T		value_type;
  typedef T *		pointer;
  typedef const T*	const_pointer;
  typedef T&		reference;
  typedef const T&	const_reference;

  template <class U> struct rebind { typedef gc_alloc<U> other; };
  
  gc_alloc() throw() {}
  gc_alloc(const gc_alloc&) throw() {}
#ifndef _MSC_VER
  template <class U> gc_alloc(const gc_alloc<U>&) throw() {}
#endif
  ~gc_alloc() throw() {}

  pointer address(reference x) const { return &x; }
  const_pointer address(const_reference x) const { return &x; }

  pointer allocate(size_type n, const void * = 0) {
    return n ==0 ? 0 : (T*)GC_MALLOC(n * sizeof(T));
  }
  char *_Charalloc(size_type n) {
    return n == 0 ? 0 : (char*)GC_MALLOC(n);
  }
  void deallocate(pointer p, size_type) {
    deallocate(p);
  }
  void deallocate(pointer p) const {
    if (p != 0)
      GC_FREE(p);
  }
  void deallocate(void * p, size_type) {
    if (p != 0)
      GC_FREE(p);
  }

  size_type max_size() const throw()
    { return size_t(-1) / sizeof(value_type); }

  void construct(pointer p, const T& val) const {
    new (p) T(val);
  }
  void destroy(pointer p) const {
    p->~T();
  }
};

template<class T, class U> inline
bool operator==(const gc_alloc<T>&, const gc_alloc<U>&)
{return (true); }

template<class T, class U> inline
bool operator!=(const gc_alloc<T>&, const gc_alloc<U>&)
{return (false); }

#endif
