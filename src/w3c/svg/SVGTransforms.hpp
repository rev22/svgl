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
#ifndef __svg_SVGTrasnforms__
#define __svg_SVGTrasnforms__

#include <svgl/config.hpp>
namespace svgl {
  class Context;
  class GLInfo;
}

#include <w3c/svg/SVGTransform.hpp>
#include <vector>
#include <iosfwd>

namespace svg {

  class GLInfo;

  class SVGMatrixTransform : public SVGTransform {
  public:
    float &a,&b,&c,&d,&e,&f;
    typedef std::vector<float> ArgsContainer;
    SVGMatrixTransform(const ArgsContainer&) throw (WrongNumberOfArguments);
    SVGMatrixTransform();
    void setNeutral();
    virtual ~SVGMatrixTransform() {}
    void glTraverse(svgl::Context*, svgl::GLInfo*) const;
    virtual void print(std::ostream&) const;
    void apply(double x, double y, double* x1, double* y1) const;
    void apply(SVGMatrix*) const;
    void updateMatrix();
    //void parseArgs(const ArgsContainer&) throw (WrongNumberOfArguments);
  };

  class SVGTranslateTransform : public SVGTransform {
  public:
    float &tx,&ty;
    typedef std::vector<float> ArgsContainer;
    SVGTranslateTransform(const ArgsContainer&) throw (WrongNumberOfArguments);
    SVGTranslateTransform();
    void setNeutral();
    virtual ~SVGTranslateTransform() {}
    void glTraverse(svgl::Context*, svgl::GLInfo*) const;
    virtual void print(std::ostream&) const;
    void apply(double x, double y, double* x1, double* y1) const;
    void apply(SVGMatrix*) const;
    void updateMatrix();
    //void parseArgs(const ArgsContainer&) throw (WrongNumberOfArguments);
  };
  
  class SVGScaleTransform : public SVGTransform {
  public:
    float &sx,&sy; 
    typedef std::vector<float> ArgsContainer;
    SVGScaleTransform(const ArgsContainer&) throw (WrongNumberOfArguments);
    SVGScaleTransform();
    void setNeutral();
    virtual ~SVGScaleTransform() {}
    void glTraverse(svgl::Context*, svgl::GLInfo*) const;
    virtual void print(std::ostream&) const;
    void apply(double x, double y, double* x1, double* y1) const;
    void apply(SVGMatrix*) const;
    void updateMatrix();
    //void parseArgs(const ArgsContainer&) throw (WrongNumberOfArguments);
  };

  class SVGRotateTransform : public SVGTransform {
  public:
    float angle, cx, cy;
    typedef std::vector<float> ArgsContainer;
    SVGRotateTransform(const ArgsContainer&) throw (WrongNumberOfArguments);
    SVGRotateTransform();
    void setNeutral();
    virtual ~SVGRotateTransform() {}
    void glTraverse(svgl::Context*, svgl::GLInfo*) const;
    virtual void print(std::ostream&) const;
    void apply(double x, double y, double* x1, double* y1) const;
    void apply(SVGMatrix*) const;
    void updateMatrix();
    //void parseArgs(const ArgsContainer&) throw (WrongNumberOfArguments);
  };

  class SVGSkewTransform : public SVGTransform {
  public:
    float angle;
    typedef std::vector<float> ArgsContainer;
    SVGSkewTransform(const ArgsContainer&) throw (WrongNumberOfArguments);
    SVGSkewTransform();
    void setNeutral();
    virtual ~SVGSkewTransform() {}
    //void parseArgs(const ArgsContainer&) throw (WrongNumberOfArguments);
  };

  class SVGSkewXTransform : public SVGSkewTransform {
  public:
    typedef std::vector<float> ArgsContainer;
    SVGSkewXTransform(const ArgsContainer&) throw (WrongNumberOfArguments);
    SVGSkewXTransform() {}
    virtual ~SVGSkewXTransform() {}
    void glTraverse(svgl::Context*, svgl::GLInfo*) const;
    virtual void print(std::ostream&) const;
    void apply(double x, double y, double* x1, double* y1) const;
    void apply(SVGMatrix*) const;
    void updateMatrix();
  };

  class SVGSkewYTransform : public SVGSkewTransform {
  public:
    typedef std::vector<float> ArgsContainer;
    SVGSkewYTransform(const ArgsContainer&) throw (WrongNumberOfArguments);
    SVGSkewYTransform() {}
    virtual ~SVGSkewYTransform() {}
    void glTraverse(svgl::Context*, svgl::GLInfo*) const;
    virtual void print(std::ostream&) const;
    void apply(double x, double y, double* x1, double* y1) const;
    void apply(SVGMatrix*) const;
    void updateMatrix();
  };

}

#endif // __svg_SVGTrasnforms__

