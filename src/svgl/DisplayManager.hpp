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
#ifndef __svgl__DisplayManager__
#define __svgl__DisplayManager__

//#include <svgl/config.hpp>
#include <vector>

namespace svg {
  class SVGSVGElement;
  class SVGElement;
  class SVGDocument;
}

namespace svgl {

  class Context;
  class GLInfo;

  class DisplayManager {
  public:
    // new API
    void init(Context * svglContext, GLInfo* glinfo);
    void display(svg::SVGDocument*);
    void reshape(unsigned int width, unsigned int height);
    void displayBranch(svg::SVGElement *);

    // old API
    void display(Context*, GLInfo*, svg::SVGSVGElement*);
    void display(svg::SVGSVGElement*);

    void displayBranch(svgl::Context*, svgl::GLInfo*, svg::SVGElement *);
    typedef std::vector<svg::SVGElement*> Branch;
    void displayBranch(svgl::Context*, svgl::GLInfo*, Branch&);

    void reshape(svgl::Context*, svgl::GLInfo*, svg::SVGSVGElement*, unsigned int width, unsigned int height);

    Context * context;
    GLInfo * glinfo;

  };

}

#endif // __svgl__DisplayManager__
