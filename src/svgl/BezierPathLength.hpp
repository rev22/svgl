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
#ifndef __svgl_BezierPathLength__
#define __svgl_BezierPathLength__

#include <svgl/config.hpp>
#include <svgl/bezierpath.hpp>

namespace svgl {

  float
  length(const BezierPath& bezier);

}


#endif // __svgl_BezierPathLength__
