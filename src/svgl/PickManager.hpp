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
#ifndef __svgl_PickManager__
#define __svgl_PickManager__

#include <svgl/config.hpp>
#include <utility>
#include <vector>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace svg {
  class SVGSVGElement;
  class SVGElement;
  class SVGDocument;
}

namespace svgl {

  class Context;
  class GLInfo;

  class PickManager {

  private:
    typedef GLuint PickStackPosition;
    typedef std::vector<PickStackPosition*> PickStackPositions;
    typedef PickStackPositions::iterator PickStackPositionIterator;
    typedef PickStackPositions::reverse_iterator PickStackPositionReverseIterator;
    PickStackPositions _validPickStacks;


  public:

    PickManager();
    PickManager(svgl::Context *, GLInfo *);
    ~PickManager();

    struct HitIterator;
    struct ReverseHitIterator;

    struct HitElementIterator {
      typedef svg::SVGElement * value_type;
      typedef value_type& reference;
      typedef value_type* pointer;

      HitElementIterator() {}

      reference operator*() { return *_element; }
      HitElementIterator& operator++();
      HitElementIterator& operator--();

      bool operator!=(const HitElementIterator& h) const { return _element!=h._element; }
      bool operator==(const HitElementIterator& h) const { return _element==h._element; }

    private:
      HitElementIterator(pointer e);
      pointer _element;
      friend struct HitIterator;
      friend struct ReverseHitIterator;
    };

    // bottom-top
    struct HitIterator {
      typedef std::pair<HitElementIterator, HitElementIterator> value_type;
      typedef value_type& reference;
      typedef value_type* pointer;

      value_type operator*();
      HitIterator& operator++();
      HitIterator& operator--();
      HitIterator() {}

      bool operator!=(const HitIterator& h) const { return _position!=h._position; }
      bool operator==(const HitIterator& h) const { return _position==h._position; }

      //private:
      HitIterator(PickStackPositionIterator);
      PickStackPositionIterator _position;
      friend class PickManager;
    };

    // top-bottom
    struct ReverseHitIterator {
      typedef std::pair<HitElementIterator, HitElementIterator> value_type;
      typedef value_type& reference;
      typedef value_type* pointer;

      value_type operator*();
      ReverseHitIterator& operator++();
      ReverseHitIterator& operator--();
      ReverseHitIterator() {}

      bool operator!=(const ReverseHitIterator& h) const { return _position!=h._position; }
      bool operator==(const ReverseHitIterator& h) const { return _position==h._position; }

      //private:
      ReverseHitIterator(PickStackPositionReverseIterator);
      PickStackPositionReverseIterator _position;
      friend class PickManager;
    };

    friend struct HitIterator;
    friend struct ReverseHitIterator;
    typedef std::pair<HitIterator, HitIterator> PickResult;
    typedef std::pair<ReverseHitIterator, ReverseHitIterator> ReversePickResult;

    PickResult pick(svg::SVGSVGElement*, Context*, GLInfo*, double x, double y, double width, double height);
    PickResult pick(svg::SVGDocument*, double x, double y, double width, double height);
    ReversePickResult reversePick();

  private:
    const unsigned int _bufsize;
    GLuint * _selectBuf;
    Context * context;
    GLInfo * glinfo;
  };

}

#endif // __svgl_PickManager__
