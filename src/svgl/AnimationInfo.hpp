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
#ifndef __AnimationInfo__
#define __AnimationInfo__

#include <svgl/config.hpp>


namespace svg {
  class SVGElement;
}

namespace svgl {

  class ExternalEntityManager;
  
  namespace Animation {
    class Manager;
  }

  class AnimationInfo {
  public:
    Animation::Manager* animationManager;
    ExternalEntityManager* externalEntityManager;
    svg::SVGElement* element;
    //bool _redisplay;
    //void setRedisplay(bool v) { _redisplay=v; }
  };

}

#endif // __AnimationInfo__
