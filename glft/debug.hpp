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
#ifndef __svgl_debug__
#define __svgl_debug__

#include <iostream>

#define ____STR_1(line)   #line
#define ____STR_2(line)   ____STR_1(line)
#define __SLINE__ ____STR_2(__LINE__) 
#define __FL__ " "__FILE__":"__SLINE__
#define __FLn__ __FL__"\n"
#define __FLendl__ __FL__ << std::endl;

#define STDDBG std::cerr << __FLendl__;

#define DBGVAL(x) #x ": " << x << " "


#endif // __svgl_debug__
