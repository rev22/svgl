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
#include <w3c/svg/SVGTransforms.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/debug.hpp>
#ifdef WIN32
#include <windows.h>
#endif
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <cmath>
#include <iostream>

#define CHECK_NUM_ARGS(classname, inf, sup)										\
    ArgsContainer::size_type s = args.size();										\
    if (s<inf || s>sup) {												\
      std::cerr << "wrong # of argument for "#classname"( "<< s << ")" << __FL__;					\
      throw WrongNumberOfArguments();											\
    }

#if 1
//static
std::ostream&
operator<< (std::ostream& out, const svg::SVGMatrix& m)
{
    out << "a:" << m.getA() << " b:" << m.getB() << " c:" << m.getC() << std::endl;
    out << "d:" << m.getD() << " e:" << m.getE() << " f:" << m.getF() << std::endl;
    return out;
}
#endif

namespace svg {

  void
  SVGTransform::print(std::ostream& out) const
  {
    out << "SVGTransform::print";
  }


  SVGMatrixTransform::SVGMatrixTransform(const ArgsContainer& args) throw (WrongNumberOfArguments)
    :
    a(matrix.getValue().getA()),
    b(matrix.getValue().getB()),
    c(matrix.getValue().getC()),
    d(matrix.getValue().getD()),
    e(matrix.getValue().getE()),
    f(matrix.getValue().getF())
    
  {
    CHECK_NUM_ARGS(SVGMatrixTransform, 6,6);
    a = args[0];
    b = args[1];
    c = args[2];
    d = args[3];
    e = args[4];
    f = args[5];    
  }

    SVGMatrixTransform::SVGMatrixTransform()
    :
    a(matrix.getValue().getA()),
    b(matrix.getValue().getB()),
    c(matrix.getValue().getC()),
    d(matrix.getValue().getD()),
    e(matrix.getValue().getE()),
    f(matrix.getValue().getF())
    {
        setNeutral();
    }
    
  void
  SVGMatrixTransform::setNeutral()
  {
    a=d=1;
    b=c=e=f=0;
  }

  void
  SVGMatrixTransform::glTraverse(svgl::Context * svglContext, svgl::GLInfo*) const
  {
    //std::cerr << "SVGMatrixTransform::glTraverse() not checked" << __FL__;
      
    float m[16];
    m[0]=a;
    m[4]=c;
    m[12]=e;
    m[1]=b;
    m[5]=d;
    m[13]=f;

    m[2]=m[3]=m[6]=m[7]=m[8]=m[9]=m[11]=m[14]=0;
    m[10]=m[15]=1;
  
    glMultMatrixf(m);
    
  }

  void
  SVGMatrixTransform::print(std::ostream& out) const
  {
    out << "matrix(" << a << "," << b << "," << c << "," << d << "," << e << "," << f;
  }

  void
  SVGMatrixTransform::apply(double x, double y, double* x1, double* y1) const
  {
    *x1 = a*x + b*y + c;
    *y1 = d*x + e*y + f;
  }
    
  void
    SVGMatrixTransform::apply(SVGMatrix *res) const
{
        SVGMatrix m;
        m.getA() = res->getA() * a + res->getB() * d;
        m.getB() = res->getA() * b + res->getB() * e;
        m.getC() = res->getA() * c + res->getB() * f;
        m.getD() = res->getD() * a + res->getE() * d;
        m.getE() = res->getD() * b + res->getE() * e;
        m.getF() = res->getD() * c + res->getE() * f;
        *res  = m;
}

void
SVGMatrixTransform::updateMatrix()
{
}

  SVGTranslateTransform::SVGTranslateTransform(const ArgsContainer& args) throw (WrongNumberOfArguments)
    :
tx(matrix.getValue().getC()),
ty(matrix.getValue().getF())
  {
    CHECK_NUM_ARGS(SVGTranslateTransform, 1,2);
    tx = args[0];
    if (s==2)
      ty = args[1];
  }

SVGTranslateTransform::SVGTranslateTransform()
:
tx(matrix.getValue().getC()),
ty(matrix.getValue().getF())
{
    setNeutral();
}

  void
  SVGTranslateTransform::setNeutral()
  {
      matrix.getValue().getA()=matrix.getValue().getE()=1;
      matrix.getValue().getB()=matrix.getValue().getC()=matrix.getValue().getD()=matrix.getValue().getF()=0;
      tx=ty=0;
  }

  void
  SVGTranslateTransform::glTraverse(svgl::Context * svglContext, svgl::GLInfo*) const
  {
    glTranslatef(tx,ty,0);
  }

  void
  SVGTranslateTransform::print(std::ostream& out) const
  {
    out << "translate(" << tx << "," << ty << ")";
  }

  void
  SVGTranslateTransform::apply(double x, double y, double* x1, double* y1) const
  {
    *x1 = x + tx;
    *y1 = y + ty;
  }

void
SVGTranslateTransform::apply(SVGMatrix *res) const
{
    res->getC() += tx;
    res->getF() += ty;
}

void
SVGTranslateTransform::updateMatrix()
{
}

 // ----

  SVGScaleTransform::SVGScaleTransform(const ArgsContainer& args) throw (WrongNumberOfArguments)
    :
sx(matrix.getValue().getA()),
sy(matrix.getValue().getE())
  {
    CHECK_NUM_ARGS(SVGScaleTransform, 1,2);
    sx = args[0];
    if (s==2)
      sy = args[1];
    else
      sy = sx;
  }

SVGScaleTransform::SVGScaleTransform()
:
sx(matrix.getValue().getA()),
sy(matrix.getValue().getE())
{
    setNeutral();
}

  void
  SVGScaleTransform::setNeutral()
  {
      matrix.getValue().getA()=matrix.getValue().getE()=1;
      matrix.getValue().getB()=matrix.getValue().getC()=matrix.getValue().getD()=matrix.getValue().getF()=0;
      
      sx=sy=1;
      updateMatrix();
  }

  void
  SVGScaleTransform::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo) const
  {
    glScalef(sx,sy,1);
  }

  void
  SVGScaleTransform::print(std::ostream& out) const
  {
    if(sx != sy) {
      out << "scale(" << sx << "," << sy << ")";
    } else {
      out << "scale(" << sx << ")";
    }
  }

  void
  SVGScaleTransform::apply(double x, double y, double* x1, double* y1) const
  {
    *x1 = sx * x;
    *y1 = sy * y;
  }

void
SVGScaleTransform::apply(SVGMatrix *res) const
{
    res->getA() *= sx;
    res->getE() *= sy;
}

void
SVGScaleTransform::updateMatrix()
{
}

  //---


  SVGRotateTransform::SVGRotateTransform(const ArgsContainer& args) throw (WrongNumberOfArguments)
    : angle(0), cx(0), cy(0)
  {
    CHECK_NUM_ARGS(SVGRotate, 1,3);
    angle = args[0];
    if (s==3) {
      cx = args[1];
      cy = args[2];
    }
    else if (s==2) {
      std::cerr << "wrong # of argument for SVGRotateTransform ( "<< s << ")" << __FL__;
    }
    SVGRotateTransform::updateMatrix();
  }

SVGRotateTransform::SVGRotateTransform()
: angle(0), cx(0), cy(0)
{
    setNeutral();
}

  void
  SVGRotateTransform::setNeutral()
  {
    angle=cx=cy=0;
      updateMatrix();
  }


  void
  SVGRotateTransform::glTraverse(svgl::Context * svglContext, svgl::GLInfo*) const
  {    
    glTranslatef(cx,cy,0);
    glRotatef(angle, 0,0,1);
    glTranslatef(-cx,-cy,0);
  }

  void
  SVGRotateTransform::print(std::ostream& out) const
  {
    if(cx!=0 || cy!=0) {
      out << "rotate(" << angle << "," << cx << "," << cy << ")";
    } else {
      out << "rotate(" << angle << ")";
    }
  }

  void
  SVGRotateTransform::apply(double x, double y, double* x1, double* y1) const
  {
    double anglerad= angle*M_PI/180.;
    double tmpx,tmpy;
    tmpx = x + cx;
    tmpy = y + cy;
    *x1 = cos(anglerad) * tmpx - sin(anglerad)*tmpy;
    *y1 = sin(anglerad) * tmpx + cos(anglerad)*tmpy;
    *x1 -= cx;
    *y1 -= cy;
  }

  void
  SVGRotateTransform::apply(SVGMatrix *res) const
{
    double a,b;
    apply(res->getA(), res->getD(), &a, &b);
    res->getA() = a;
    res->getD() = b;
    apply(res->getB(), res->getE(), &a, &b);
    res->getB() = a;
    res->getE() = b;
    apply(res->getC(), res->getF(), &a, &b);
    res->getC() = a;
    res->getF() = b;
}

void
SVGRotateTransform::updateMatrix()
{
    double anglerad= angle*M_PI/180.;
    matrix.getValue().getA() = cos(anglerad);
    matrix.getValue().getB() = -sin(anglerad);
    matrix.getValue().getC() = cos(anglerad) * cx - sin(anglerad) * cy - cx;
    matrix.getValue().getD() = sin(anglerad);
    matrix.getValue().getE() = cos(anglerad);
    matrix.getValue().getF() = sin(anglerad) * cx + cos(anglerad) * cy - cy;
    
    //std::cerr << matrix.getValue() << __FL__;
}


//---
  SVGSkewTransform::SVGSkewTransform(const ArgsContainer& args) throw (WrongNumberOfArguments)
    : angle(0)
  {
    CHECK_NUM_ARGS(SVGSkewTransform, 1,1);
    angle = args[0];
  }

SVGSkewTransform::SVGSkewTransform()
: angle(0)
{
    setNeutral();
}
  void
  SVGSkewTransform::setNeutral()
  {
    angle=0;
      updateMatrix();
  }


  SVGSkewXTransform::SVGSkewXTransform(const ArgsContainer& args) throw (WrongNumberOfArguments)
    : SVGSkewTransform(args)
  {
  }

  void
  SVGSkewXTransform::print(std::ostream& out) const
  {
    out << "skewX(" << angle << ")";
  }


  void
  SVGSkewXTransform::glTraverse(svgl::Context * svglContext, svgl::GLInfo*) const
  {
    float m[16]={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    float anglerad= angle*M_PI/180.;
    m[4] = tan(anglerad);
    glMultMatrixf(m);
  }

  void
  SVGSkewXTransform::apply(double x, double y, double* x1, double* y1) const
  {
    double anglerad= angle*M_PI/180.;
    *x1 = x + tan(anglerad)*y;
    *y1 = y;
  }

void
SVGSkewXTransform::apply(SVGMatrix *res) const
{
    double anglerad= angle*M_PI/180.;
    res->getA() = res->getA() + tan(anglerad)*res->getD();
    res->getB() = res->getB() + tan(anglerad)*res->getE();
    res->getC() = res->getC() + tan(anglerad)*res->getF();

}    

void
SVGSkewXTransform::updateMatrix()
{
    matrix.getValue().getA()=matrix.getValue().getE()=1;
    matrix.getValue().getB()=matrix.getValue().getC()=matrix.getValue().getD()=matrix.getValue().getF()=0;
    
    double anglerad= angle*M_PI/180.;
    matrix.getValue().getB() = tan(anglerad);
}

  SVGSkewYTransform::SVGSkewYTransform(const ArgsContainer& args) throw (WrongNumberOfArguments)
    : SVGSkewTransform(args)
  {
  }

  void
  SVGSkewYTransform::glTraverse(svgl::Context * svglContext, svgl::GLInfo*) const
  {
    float m[16]={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    float anglerad= angle*M_PI/180.;
    m[1] = tan(anglerad);
    glMultMatrixf(m);
  }

  void
  SVGSkewYTransform::print(std::ostream& out) const
  {
    out << "skewY(" << angle << ")";
  }

  void
  SVGSkewYTransform::apply(double x, double y, double* x1, double* y1) const
  {
    double anglerad= angle*M_PI/180.;
    *x1 = x;
    *y1 = tan(anglerad)*x + y;
  }

void
SVGSkewYTransform::apply(SVGMatrix *res) const
{
    double anglerad= angle*M_PI/180.;
    res->getD() += tan(anglerad)*res->getA();
    res->getE() += tan(anglerad)*res->getB();
    res->getF() += tan(anglerad)*res->getC();
    
}    

void
SVGSkewYTransform::updateMatrix()
{
    matrix.getValue().getA()=matrix.getValue().getE()=1;
    matrix.getValue().getB()=matrix.getValue().getC()=matrix.getValue().getD()=matrix.getValue().getF()=0;
    
    double anglerad= angle*M_PI/180.;
    matrix.getValue().getD() = tan(anglerad);
}


}

