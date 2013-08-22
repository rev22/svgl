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
#include <svgl/DisplayManager.hpp>
#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/debug.hpp>

#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/svg/SVGDocument.hpp>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace svgl {

  void
  DisplayManager::init(svgl::Context * svglContext, svgl::GLInfo* info)
  {
    context = svglContext;
    glinfo = info;
#if 1
		// should it really be done here ? what if we have something to display along svgl stuff ?
    glClearColor(1,1,1,0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
  }

  void
  DisplayManager::display(svgl::Context * svglContext, svgl::GLInfo* glinfo, svg::SVGSVGElement * svgelem)
  {
    svglContext->svgOwner = svgelem;
    glinfo->setZoomAndPan();
    if(svgelem)
      svgelem->glTraverse(svglContext, glinfo);
  }

  void
  DisplayManager::display(svg::SVGSVGElement * svgelem)
  {
    display(context, glinfo, svgelem);
  }


  void
  DisplayManager::display(svg::SVGDocument * doc)
  {
    if (!doc || !doc->getRootElement())
      return;

    display(const_cast<svg::SVGSVGElement*>(doc->getRootElement()));
  }


  void
  buildBranch(DisplayManager::Branch& b, svg::SVGElement * elem)
  {
    svg::SVGElement * parent = dynamic_cast<svg::SVGElement*>(elem->getParentNode());
    if(parent)
      buildBranch(b, parent);
    //std::cerr << "<" << elem->getDtdElement() << ">";
    b.push_back(elem);
  }

  void
  DisplayManager::displayBranch(svgl::Context * svglContext, svgl::GLInfo* glinfo, svg::SVGElement * elem)
  {
    Branch b;
    buildBranch(b,elem);
    //std::cerr << __FL__;
    displayBranch(svglContext, glinfo, b);
  }

  void
  DisplayManager::displayBranch(svg::SVGElement * elem)
  {
    displayBranch(context, glinfo, elem);
  }

  void
  DisplayManager::displayBranch(svgl::Context * svglContext, svgl::GLInfo* glinfo, Branch& b)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glinfo->setZoomAndPan();
    if(b.begin()!=b.end()) {
      svg::SVGElement * elem = *(b.begin());
      Branch::iterator beg = b.begin(), end = b.end();
      elem->glTraverseBranch(svglContext, glinfo, beg, end);
    }
  }

  void
  DisplayManager::reshape(svgl::Context * svglContext, svgl::GLInfo* glinfo, svg::SVGSVGElement * svgelem, unsigned int width, unsigned int height)
  {
    svglContext->setViewportWidthHeight(width,height);
    glinfo->winWidth=width;
    glinfo->winHeight=height;
    glViewport(0,0,width,height);
  }

  void
  DisplayManager::reshape(unsigned int width, unsigned int height)
  {
    reshape(context, glinfo, 0, width, height);
  }


}
