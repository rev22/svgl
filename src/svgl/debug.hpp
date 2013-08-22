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
the file license.txt and at the following URL:
http://www.emn.fr/info/image/Themes/Indigo/licence.html

 */
#ifndef __svgl_debug__
#define __svgl_debug__

#include <svgl/config.hpp>
#include <iostream>

#define STDDBG std::cerr << __FILE__ ":" << __LINE__ << std::endl;
#define DBGVAR(x) #x ": " << x << " "
#define __FL__ " " __FILE__ ":" << __LINE__ << std::endl;

#define GLDBG \
{ \
  int err = glGetError(); \
  if(err) { \
    std::cerr << "GL error: " << gluErrorString(err) << __FL__; \
  }  \
}


#endif // __svgl_debug__
