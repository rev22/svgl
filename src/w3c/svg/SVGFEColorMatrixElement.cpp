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
#include <w3c/svg/SVGFEColorMatrixElement.hpp>
#include <svgl/GLInfo.hpp>
#include <w3c/svg/String.hpp>
#include <w3c/svg/SVGNumberList.hpp>
#include <math.h>
#include <svgl/FilterContext.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

//#define C_MATRIX_ORDER
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace svg {

  unicode::String *
  SVGFEColorMatrixElement::getOptionalIn1()
  {
    unicode::String * in1 = GETVAL(In1);
    return in1;
  }

  unicode::String *
  SVGFEColorMatrixElement::getOptionalIn2()
  {
    return 0;
  }

  unicode::String *
  SVGFEColorMatrixElement::getOptionalResult()
  {
    return GETVAL(Result);
  }


  void
  SVGFEColorMatrixElement::glPreTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, SVGElement* elem, svgl::FilterContext* filterContext)
  {
#ifdef GL_ARB_imaging
    const SVG_FECOLORMATRIX_TYPE& type_ = GETVAL(Type);
    const SVGNumberList& values_ = GETVAL(Values);

    const int numValues = values_.numberOfItems();
#if 0
    std::cerr << "size " << values_.numberOfItems() << __FL__;
    for(unsigned int i=0; i<values_.numberOfItems(); ++i) {
      std::cerr << i << ":" << values_.getItem(i).GET_SIMPLE_VAL(Value) << " ";
    }
    std::cerr << std::endl;
#endif

    switch (type_) {
    case SVG_FECOLORMATRIX_TYPE_MATRIX: {
      if(numValues==20 ) {

	GLfloat mat[16];
	#define VAL(i) values_.getItem(i).GET_SIMPLE_VAL(Value);
#ifdef C_MATRIX_ORDER
	mat[0] = VAL(0);
	mat[1] = VAL(1);
	mat[2] = VAL(2);
	mat[3] = VAL(3);

	mat[4] = VAL(5);
	mat[5] = VAL(6);
	mat[6] = VAL(7);
	mat[7] = VAL(8);

	mat[8] = VAL(10);
	mat[9] = VAL(11);
	mat[10] = VAL(12);
	mat[11] = VAL(13);

	mat[12] = VAL(15);
	mat[13] = VAL(16);
	mat[14] = VAL(17);
	mat[15] = VAL(18);
#else
	mat[0] = VAL(0);
	mat[1] = VAL(5);
	mat[2] = VAL(10);
	mat[3] = VAL(15);

	mat[4] = VAL(1);
	mat[5] = VAL(6);
	mat[6] = VAL(11);
	mat[7] = VAL(16);

	mat[8] = VAL(2);
	mat[9] = VAL(7);
	mat[10] = VAL(12);
	mat[11] = VAL(17);

	mat[12] = VAL(3);
	mat[13] = VAL(8);
	mat[14] = VAL(13);
	mat[15] = VAL(18);
#endif
#undef VAL
#if 0
	for(int i=0; i<16; ++i) {
	  std::cerr << "mat[" << i << "]=" << mat[i] << __FL__;
	}
#endif
	//mat[3]=mat[7]=mat[11]=0;mat[15]=1;
	glMatrixMode(GL_COLOR);
	glLoadMatrixf(mat);

	if(mat[12] || mat[13] || mat[14] || mat[15]) {
	  std::cerr << "last column of color matrix is non-zero, result may not be exact" __FL__;
	}

	//glScalef(.1,.1,.1);
	//glRotatef(90,1,1,1);
	glMatrixMode(GL_MODELVIEW);
      }
    }
    break;
      
    case SVG_FECOLORMATRIX_TYPE_SATURATE: {
      float s = values_.getItem(0).GET_SIMPLE_VAL(Value);
      //      std::cerr << "saturate " << s << std::endl;
#ifdef C_MATRIX_ORDER
      GLfloat mat[16] = {0.213,0.715,0.072,0, 0.213,0.715,0.072,0, 0.213,0.715,0.072,0,  0,0,0,1};
      mat[0] += 0.787*s;
      mat[1] += -0.715*s;
      mat[2] += -0.072*s;

      mat[4] += -0.213*s;
      mat[5] += 0.285*s;
      mat[6] += -0.072*s;

      mat[8] += -0.213*s;
      mat[9] += -0.715*s;
      mat[10] += 0.928*s;
#else
      GLfloat mat[16] = {0.213,0.213,0.213,0, 0.715,0.715,0.715,0, 0.072,0.072,0.072,0, 0,0,0,1};
      mat[0] += 0.787*s;
      mat[4] += -0.715*s;
      mat[8] += -0.072*s;

      mat[1] += -0.213*s;
      mat[5] += 0.285*s;
      mat[9] += -0.072*s;

      mat[2] += -0.213*s;
      mat[6] += -0.715*s;
      mat[10] += 0.928*s;
#endif
      glMatrixMode(GL_COLOR);
      glLoadMatrixf(mat);
      glMatrixMode(GL_MODELVIEW);  
    }
      break;

    case SVG_FECOLORMATRIX_TYPE_HUEROTATE: {
      float huerotate = values_.getItem(0).GET_SIMPLE_VAL(Value);
      // std::cerr << DBGVAR(huerotate) << __FL__;
      GLfloat mat[16] = {0.213,0.213,0.213,0, 0.715,0.715,0.715,0, 0.072,0.072,0.072,0, 0,0,0,1};
      float
	c = ::cos(M_PI*huerotate/180.),
	s = ::sin(M_PI*huerotate/180.);


      mat[0] += 0.787*c - 0.213*s;
      mat[4] += -0.715*c - 0.715*s;
      mat[8] += -0.072*c + 0.928*s;

      mat[1] += -0.212*c + 0.143*s;
      mat[5] += 0.285*c + 0.140*s;
      mat[9] += -0.072*c - 0.283*s;

      mat[2] += -0.213*c - 0.787*s;
      mat[6] += -0.715*c + 0.715*s;
      mat[10] += 0.928*c + 0.072*s;

      glMatrixMode(GL_COLOR);
      glLoadMatrixf(mat);
      glMatrixMode(GL_MODELVIEW);  
    }

    break;

    case SVG_FECOLORMATRIX_TYPE_LUMINANCETOALPHA: {
#ifdef C_MATRIX_ORDER
      GLfloat mat[16] = {0,0,0,0, 0,0,0,0, 0.2125,0.7154,0.0721,0, 0,0,0,1};
#else
      GLfloat mat[16] = {0,0,0.2125,0, 0,0.7154,0,0, 0,0,0.0721,0, 0,0,0,1};
#endif

      glMatrixMode(GL_COLOR);
      glLoadMatrixf(mat);
      glMatrixMode(GL_MODELVIEW);  
     }
      break;

    default:
      std::cerr << "unsupported color matrix type" << __FL__;
      break;
    }

#endif // GL_ARB_imaging

  }


  void
  SVGFEColorMatrixElement::glPostTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo, SVGElement* elem, svgl::FilterContext* filterContext)
  {
    unicode::String * in1 = GETVAL(In1);
    unicode::String * result = GETVAL(Result);

    SVGFilterPrimitiveStandardAttributes::glTraverse(svglContext, glinfo, elem, filterContext, in1, result);

#ifdef GL_ARB_imaging
    glMatrixMode(GL_COLOR);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
#endif
  }

}
