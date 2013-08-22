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

#include <w3c/svg/SVGLocatable.hpp>
#include <w3c/dom/DOMException.hpp>

namespace svg {

SVGRect*
SVGLocatable::getBBox (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGMatrix*
SVGLocatable::getCTM (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGMatrix*
SVGLocatable::getScreenCTM (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGMatrix*
SVGLocatable::getTransformToElement ( const SVGElement& element )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

}
