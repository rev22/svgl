/*
 * $Id$
 *
Copyright (c) 2001 Stephane Conversy, Jean-Daniel Fekete and Ecole des
Mines de Nantes.
All rights reserved.
 
This software is proprietary information of Stephane Conversy,
Jean-Daniel Fekete and Ecole des Mines de Nantes.  You shall use it
only in accordance with the terms of the license agreement you
accepted when downloading this software.  The license is available in
the file licence.txt and at the following URL:
http://www.emn.fr/info/image/Themes/Indigo/licence.html

 */
#ifndef __svg_List__
#define __svg_List__

#include <svgl/config.hpp>
#include <vector>
#include <w3c/dom/string.hpp>
#if SVGL_USE_GC
#include <utils/gc_alloc.hpp>
#endif

namespace svg {

  template<class X=dom::string>
  class SVGList {
  public:
#if SVGL_USE_GC
    typedef std::vector<X, gc_alloc<X> > container_type;
#else
    typedef std::vector<X> container_type;
#endif
    typedef unsigned long size_type;

    size_type numberOfItems() const {
      return _impl.size();
    }

    void clear() {
      _impl.clear();
    }

    X& initialize(const X& x) {
      _impl.clear();
      _impl.push_back(x);
      return _impl[0];
    }

    X createItem() const {
      return X();
    }

    X& getItem( size_type index ) {
      return _impl[index];
    }

    const X& getItem( size_type index ) const {
      return _impl[index];
    }

    X& insertItemBefore( const X& newItem, size_type index ) {
      if(index >= numberOfItems()) {
	return appendItem(newItem);
      }
      typename container_type::iterator i = _impl.begin();
      while(index) { ++i; --index; }
      _impl.insert(i, newItem); return _impl[index];
    }

    X& replaceItem( const X& newItem, size_type index) {
      _impl[index] = newItem;
      return _impl[index];
    }

    X& removeItem(size_type index) {
      X& x = _impl[index];
      //_impl.erase(&_impl[index]);
			_impl.erase(container_type::iterator(&_impl[index]));
      return x; 
    }

    X& appendItem(const X& x) {
      _impl.push_back(x);
      return _impl.back();
    }

  private:
    container_type _impl;
    
  };

}

#endif // __svg_List__
