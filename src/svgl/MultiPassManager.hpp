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
#ifndef __svgl_MultiPassManager__
#define __svgl_MultiPassManager__

#include <svgl/config.hpp>

namespace svgl {

  class GLInfo;
  class TextureSnapshot;

  class MultiPassManager {
  public:
    void push(svgl::GLInfo*, float xminf, float yminf, float xmaxf, float ymaxf);
    void pop(svgl::GLInfo*);
    svgl::TextureSnapshot * getSaveUnder() { return snap_saveunder; }
  private:
    int xmin, ymin, xmax, ymax;
    svgl::TextureSnapshot * snap_saveunder;
    //svgl::TextureSnapshot * snap_savegroup;
  };

}

#endif // __svgl_MultiPassManager__

