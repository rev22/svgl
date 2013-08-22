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
#ifndef __parseTransform__
#define __parseTransform__

#include <svgl/config.hpp>
#include <w3c/svg/SVGTransformList.hpp>

#include <utils/String.hpp>

namespace svg {


  class SVGTransform;

  void
  parseTransform(const unicode::String* s, SVGTransformList *thelist);

  SVGTransform*
  parseTransform(const unicode::String* s, unicode::String::size_type beg=0, unicode::String::size_type end=unicode::String::npos);

}


#endif // __parseTransform__

