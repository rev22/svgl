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
#include <w3c/svg/SVGGElement.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/Context.hpp>
#include <svgl/BoundingBoxManager.hpp>
#include <svgl/MultiPassManager.hpp>


#include <w3c/svg/SVGClipPathElement.hpp>
#include <w3c/svg/SVGFilterElement.hpp>
// uri of ClipPath
#include <w3c/svg/SVGSVGElement.hpp>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <w3c/css/CSSStyleDeclaration.hpp>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

namespace svg {

  void
  SVGGElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    bool reallyTraverse = preManageGlCompilation();
    if(!reallyTraverse)
      return;

    glPushMatrix();
    SVGTransformable::glTraverse(svglContext, glinfo);

    float opacity=1;
    svg::SVGClipPathElement *clippath_element=0;
    SVGFilterElement* filter_element = 0;
    bool snap_done=false;
    svgl::MultiPassManager multiPassManager;

    const css::CSSStyle* style = getStyle().getValue().getStyle();
    if(style) {
      opacity = style->getOpacity();
      if (opacity!=1) {
	svgl::BoundingBoxManager bboxManager;
	bboxManager.glPreTraverse(glinfo);
	glPushMatrix();
	glTraverseBoundingBoxChildren(svglContext, glinfo);
	glPopMatrix();
	float xminf,yminf,xmaxf,ymaxf;
	snap_done = bboxManager.glPostTraverse(glinfo, &xminf, &yminf, &xmaxf, &ymaxf);
	if(snap_done) {
	  multiPassManager.push(glinfo, xminf, yminf, xmaxf, ymaxf);
	}
      }

      clippath_element = getSVGClipPathElement(svglContext->externalEntityManager, getOwnerDocument());
      if(clippath_element)
	clippath_element->glPreTraverse(svglContext, glinfo);

#if 1
      filter_element = getSVGFilterElement(svglContext->externalEntityManager, getOwnerDocument());
      if(filter_element)
	filter_element->glPreTraverse(svglContext, glinfo, dynamic_cast<SVGElement*>(this));
#endif

    }

    //glinfo->pushPickInfo(this);
    glTraverseChildren(svglContext, glinfo);
    //glinfo->popPickInfo();

    if( snap_done ) {
      float alpha_save  = glinfo->alpha;
      glinfo->alpha = opacity;
      multiPassManager.pop(glinfo);
      glinfo->alpha = alpha_save;
    }

    if(filter_element) {
      filter_element->glPostTraverse(svglContext, glinfo, dynamic_cast<SVGElement*>(this));
    }

    if(clippath_element) {
      clippath_element->glPostTraverse(svglContext, glinfo);
    }
  
    glPopMatrix();

    postManageGlCompilation();
  }

  void
  SVGGElement::glTraverseBranch(svgl::Context * svglContext, svgl::GLInfo* glinfo, Branch::iterator& cur, Branch::iterator& end)
  {
    glPushMatrix();
    SVGTransformable::glTraverse(svglContext, glinfo);

    float opacity=1;
    svg::SVGClipPathElement *clippath_element=0;
    SVGFilterElement* filter_element = 0;
    bool snap_done=false;
    svgl::MultiPassManager multiPassManager;

    const css::CSSStyle* style = getStyle().getValue().getStyle();
    if(style) {
      opacity = style->getOpacity();
      if (opacity!=1) {
	svgl::BoundingBoxManager bboxManager;
	bboxManager.glPreTraverse(glinfo);
	glPushMatrix();
	glTraverseBoundingBoxChildren(svglContext, glinfo);
	glPopMatrix();
	float xminf,yminf,xmaxf,ymaxf;
	snap_done = bboxManager.glPostTraverse(glinfo, &xminf, &yminf, &xmaxf, &ymaxf);
	if(snap_done) {
	  multiPassManager.push(glinfo, xminf, yminf, xmaxf, ymaxf);
	}
      }

      clippath_element = getSVGClipPathElement(svglContext->externalEntityManager, getOwnerDocument());
      if(clippath_element)
	clippath_element->glPreTraverse(svglContext, glinfo);

#if 1
      filter_element = getSVGFilterElement(svglContext->externalEntityManager, getOwnerDocument());
      if(filter_element)
	filter_element->glPreTraverse(svglContext, glinfo, dynamic_cast<SVGElement*>(this));
#endif

    }

    ++cur;
    if(cur!=end) {
      SVGElement * elem = *cur;
      elem->glTraverseBranch(svglContext, glinfo, cur, end);
    }

    if( snap_done ) {
      float alpha_save  = glinfo->alpha;
      glinfo->alpha = opacity;
      multiPassManager.pop(glinfo);
      glinfo->alpha = alpha_save;
    }

    if(filter_element) {
      filter_element->glPostTraverse(svglContext, glinfo, dynamic_cast<SVGElement*>(this));
    }

    if(clippath_element) {
      clippath_element->glPostTraverse(svglContext, glinfo);
    }
  
    glPopMatrix();

  }



  void
  SVGGElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    glPushMatrix();
    SVGTransformable::glTraverse(svglContext, glinfo);

    svg::SVGClipPathElement *clippath_element=0;

    clippath_element = getSVGClipPathElement(svglContext->externalEntityManager, getOwnerDocument());
    if(clippath_element)
      clippath_element->pickTraverse(svglContext, glinfo);

    glinfo->pushPickInfo(this);
    pickTraverseChildren(svglContext, glinfo);
    glinfo->popPickInfo();

    glPopMatrix();

  }


}




