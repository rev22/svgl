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

#include <w3c/svg/SVGTransform.hpp>

namespace svg {

void
SVGTransform::setMatrix ( const SVGMatrix& m )
{
    matrix.setValue(m);
}

void
SVGTransform::setTranslate ( float tx, float ty ) 
{
}

void
SVGTransform::setScale ( float sx, float sy ) 
{
}

void
SVGTransform::setRotate ( float angle, float cx, float cy ) 
{
}

void
SVGTransform::setSkewX ( float angle )
{
}

void
SVGTransform::setSkewY ( float angle ) 
{
}

}
