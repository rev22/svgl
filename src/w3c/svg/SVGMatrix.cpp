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

#include <w3c/svg/SVGMatrix.hpp>

namespace svg {

SVGMatrix*
SVGMatrix::multiply ( const SVGMatrix& secondMatrix )
{
    SVGMatrix * res = new SVGMatrix;
    res->getA() = getA()*secondMatrix.getA()+getB()*secondMatrix.getD();
    res->getB() = getA()*secondMatrix.getB()+getB()*secondMatrix.getE();
    res->getC() = getA()*secondMatrix.getC()+getB()*secondMatrix.getF() + getC();
    res->getD() = getD()*secondMatrix.getA()+getE()*secondMatrix.getD();
    res->getE() = getD()*secondMatrix.getB()+getE()*secondMatrix.getE();
    res->getF() = getD()*secondMatrix.getC()+getE()*secondMatrix.getF() + getF();
    return res;
}

SVGMatrix*
SVGMatrix::inverse (  )
{
    double ae_bd = getA()*getE()-getB()*getD();
    if( (ae_bd)==0 || (getA()==0))
        return 0;
    //ae_bd = 1/(ae_bd);
		
#if 0		
    double fa_dcDIVae_bd = (getF()*getA()-getD()*getC())*ae_bd;

    SVGMatrix* res = new SVGMatrix;
    res->getA() = getE()*ae_bd;
    res->getB() = -getB()*ae_bd;
    res->getC() = getA()*(getB()*getF() -getC()*getE())*ae_bd; 
    res->getD() = -getD()*ae_bd;
    res->getE() = getA()*ae_bd;
    res->getF() = -fa_dcDIVae_bd;
#else
		double fa_dc = getF()*getA()-getD()*getC();
		SVGMatrix* res = new SVGMatrix;
    res->getA() = getE()/ae_bd;
    res->getB() = -getB()/ae_bd;
    res->getC() = -(getC()*getE() + getB()*getF())/ae_bd; 
    res->getD() = -getD()/ae_bd;
    res->getE() = getA()/ae_bd;
    res->getF() = -fa_dc/ae_bd;
		
		
#endif
    return res;
}

SVGMatrix*
SVGMatrix::translate ( float x, float y )
{
    SVGMatrix * res = new SVGMatrix;
    *res = *this;
    res->getC() += x;
    res->getF() += y;
    return res;
}

SVGMatrix*
SVGMatrix::scale ( float scaleFactor )
{
    SVGMatrix * res = new SVGMatrix;
    *res = *this;
    res->getA() *= scaleFactor;
    res->getF() *= scaleFactor;
    return res;
}

SVGMatrix*
SVGMatrix::scaleNonUniform ( float scaleFactorX, float scaleFactorY )
{
    SVGMatrix * res = new SVGMatrix;
    *res = *this;
    res->getA() *= scaleFactorX;
    res->getF() *= scaleFactorY;
    return res;
}

SVGMatrix*
SVGMatrix::rotate ( float angle )
{
  return 0;
}

SVGMatrix*
SVGMatrix::rotateFromVector ( float x, float y )
{
  return 0;
}

SVGMatrix*
SVGMatrix::flipX (  )
{
  return 0;
}

SVGMatrix*
SVGMatrix::flipY (  )
{
  return 0;
}

SVGMatrix*
SVGMatrix::skewX ( float angle )
{
  return 0;
}

SVGMatrix*
SVGMatrix::skewY ( float angle )
{
  return 0;
}

}
