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
#include <w3c/svg/SVGFEGaussianBlurElement.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/FilterContext.hpp>
#include <svgl/TextureSnapshot.hpp>
#include <utils/StringUtils.hpp>

#ifdef WIN32
#include <windows.h>
#endif
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#ifndef  GL_GLEXT_PROTOTYPES
#define  GL_GLEXT_PROTOTYPES
#endif
#include <cmath>
#include <memory>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

namespace svg {

  unicode::String *
  SVGFEGaussianBlurElement::getOptionalIn1()
  {
    unicode::String * in1 = GETVAL(In1);
    return in1;
  }

  unicode::String *
  SVGFEGaussianBlurElement::getOptionalIn2()
  {
    return 0;
  }

  unicode::String *
  SVGFEGaussianBlurElement::getOptionalResult()
  {
    return GETVAL(Result);
  }


  extern String stdDeviation_attribute_string;

  int
  SVGFEGaussianBlurElement::customParseAttributes(const String& attrName, const String& attrValue)
  {
    if(attrName==stdDeviation_attribute_string && attrValue) {
      std::vector<float> v;
      if(!unicode::get_float_list(*attrValue, &v) || (v.size()!=1 && v.size()!=2)) {
	std::cerr << "stdDeviation is not a list of one or two floats" __FL__;
	return 0;
      }

      float xdev = v[0];
      float ydev=xdev;
      if (v.size()==2)
	ydev = v[1];

      SVGNumber n;
      n.dom_setValue(xdev);
      SETVAL(stdDeviationX, n);
      n.dom_setValue(ydev);
      SETVAL(stdDeviationY, n);
      return 1;
    }
    return 0;
  }


  void
  SVGFEGaussianBlurElement::glPreTraverse(svgl::Context*, svgl::GLInfo*, SVGElement*, svgl::FilterContext* filterContext)
  {
    GLint widthMax,heightMax;
#ifndef WIN32
#if 1
    glGetConvolutionParameteriv(GL_CONVOLUTION_2D, GL_MAX_CONVOLUTION_WIDTH, &widthMax);
    glGetConvolutionParameteriv(GL_CONVOLUTION_2D, GL_MAX_CONVOLUTION_HEIGHT, &heightMax);
#else
    glGetConvolutionParameteriv(GL_CONVOLUTION_1D, GL_MAX_CONVOLUTION_WIDTH, &widthMax);
    glGetConvolutionParameteriv(GL_CONVOLUTION_1D, GL_MAX_CONVOLUTION_HEIGHT, &heightMax);
#endif
#endif

    //std::cerr << DBGVAR(widthMax) << DBGVAR(heightMax) << __FL__;

#if 0
    float xdev = GETVAL(StdDeviationX);
    float ydev = GETVAL(StdDeviationY);
    /*
    The Gaussian blur kernel is an approximation of the normalized convolution:
    H(x) = exp(-x^2 / (2s^2 )) / sqrt(2* pi*s^2 )

    gnuplot
    s=2
    plot exp(-x**2/(2*s**2)) / sqrt(2*pi*s**2)

    */
    const int halfsize=5;
    const int size = 2*halfsize-1;
    float row[size], col[size];

    int rowsize = size;
    int colsize = size;

    float row_twossquare = 2*xdev*xdev;
    // FIXME
    if(row_twossquare==0) {
      row_twossquare=1;
      rowsize=1;
    }
    float row_invtwossquare = 1./row_twossquare;
    float row_invsqstuff = 1./sqrt(M_PI*row_twossquare);

    float col_twossquare = 2*ydev*ydev;
    // FIXME
    if(col_twossquare==0) {
      col_twossquare=1;
      colsize=1;
    }
    float col_invtwossquare = 1./col_twossquare;
    float col_invsqstuff = 1./sqrt(M_PI*col_twossquare);


    float sum=0;
    for(int i=0;i<halfsize;++i) {
      float rowtmp = std::exp(-i*i*row_invtwossquare)*row_invsqstuff;
      row[halfsize-1+i] = row[halfsize-1-i] = rowtmp;
      float coltmp = std::exp(-i*i*col_invtwossquare)*col_invsqstuff;
      col[halfsize-1+i] = col[halfsize-1-i] = coltmp;
      sum+=rowtmp*coltmp;
    }
    if(sum==0)
      sum=1;
    sum=std::sqrt(2*sum);
    //std::cerr << DBGVAR(sum) << __FL__;
    for(int i=0;i<size;++i) {
      row[i]/=sum;
      col[i]/=sum;
    }
    //std::cerr << __FL__;
#endif

#if 1
    // box filter

    SVGNumber xdevn = GETVAL(StdDeviationX);
    SVGNumber ydevn = GETVAL(StdDeviationY);
    
    float xdev = xdevn.GET_SIMPLE_VAL(Value);
    float ydev = ydevn.GET_SIMPLE_VAL(Value);

    int rowd = static_cast<int>(xdev * 3 * sqrt(2*M_PI)/4. + 0.5);
    int cold = static_cast<int>(ydev * 3 * sqrt(2*M_PI)/4. + 0.5);
    //std::cerr << DBGVAR(rowd) << DBGVAR(cold) << std::endl;
    if(rowd>widthMax)
      rowd=widthMax;
    if(cold>heightMax)
      cold=heightMax;
    //std::cerr << DBGVAR(rowd) << DBGVAR(cold) << std::endl;

    const int rowsize=rowd, colsize=cold;
    std::auto_ptr<GLfloat> row(new GLfloat[rowsize]), col(new GLfloat[colsize]);
    const float v = 1./(rowsize*colsize);
    int i;
    for(i=0; i<rowsize; ++i) {
      row.get()[i]=v;
    }
    for(i=0; i<colsize; ++i) {
      col.get()[i]=1;
    }

#endif

#if 1
#ifdef GL_ARB_imaging
    glConvolutionParameteri(GL_SEPARABLE_2D, GL_CONVOLUTION_BORDER_MODE, GL_CONSTANT_BORDER);
    GLfloat border [] = {1,1,1,1};
    glConvolutionParameterfv(GL_SEPARABLE_2D, GL_CONVOLUTION_BORDER_COLOR, border);
    glSeparableFilter2D(GL_SEPARABLE_2D, GL_RGBA, rowsize, colsize, GL_LUMINANCE, GL_FLOAT, row.get(), col.get());
    glEnable(GL_SEPARABLE_2D);
#endif

#else
    glMatrixMode(GL_COLOR);
    glRotatef(90,1,1,1);
    glMatrixMode(GL_MODELVIEW);
#endif

  }


  void
  SVGFEGaussianBlurElement::glPostTraverse(svgl::Context* svglContext, svgl::GLInfo* glinfo, SVGElement* elem, svgl::FilterContext* filterContext)
  {
    unicode::String * in1 = GETVAL(In1);
    unicode::String * result = GETVAL(Result);
    
    SVGFilterPrimitiveStandardAttributes::glTraverse(svglContext, glinfo, elem, filterContext, in1, result);

#ifdef GL_ARB_imaging
#if 1
    glDisable(GL_SEPARABLE_2D);
#else
    glMatrixMode(GL_COLOR);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
#endif
#endif
  }

  void
  SVGFEGaussianBlurElement::setStdDeviation ( float stdDeviationX, float stdDeviationY )
  {
    throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
  }
}

