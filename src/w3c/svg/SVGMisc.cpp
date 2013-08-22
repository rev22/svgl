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

#include <w3c/svg/GetSVGDocument.hpp>
#include <w3c/svg/SVGColor.hpp>
#include <w3c/svg/SVGPaint.hpp>
#include <w3c/svg/SVGAngle.hpp>
#include <w3c/svg/SVGPoint.hpp>
#include <w3c/dom/DOMException.hpp>

namespace svg {

SVGDocument*
GetSVGDocument::getSVGDocument (  ) 
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
  return 0;
}

void
SVGColor::setRGBColor ( const DOMString& rgbColor )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGColor::setRGBColorICCColor ( const DOMString& rgbColor, const DOMString& iccColor )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGPaint::setUri ( const DOMString& uri )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGPaint::setPaint ( unsigned short paintType, const DOMString& uri, const DOMString& rgbColor, const DOMString& iccColor )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGAngle::newValueSpecifiedUnits ( unsigned short unitType, float valueInSpecifiedUnits ) 
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGAngle::convertToSpecifiedUnits ( unsigned short unitType ) 
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPoint*
SVGPoint::matrixTransform ( const SVGMatrix& matrix )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

}
