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
#include <utils/String.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <w3c/dom/Document.hpp>
#include <w3c/dom/DOMException.hpp>
#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#include <svgl/MultiPassManager.hpp>
#include <svgl/TextureSnapshot.hpp>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <cmath>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>


namespace svg {

#ifdef WITH_XERCESC
void
SVGSVGElement::parse(const DOM_Node& node, SVGSVGElement* owner, SVGElement* viewport)
{
  SVGElement::parse(node, owner, this);
  ownerSVGElement.setValue(owner);
  viewportElement.setValue(viewport);
}
#endif

void
SVGSVGElement::setOwnerAndViewPort(SVGSVGElement* owner, SVGElement* viewport)
{
  SVGElement::setOwnerAndViewPort(owner, this);
  ownerSVGElement.setValue(owner);
  viewportElement.setValue(viewport);
}

dom::Element*
SVGSVGElement::getElementById ( const DOMString& elementId )
{
  return getOwnerDocument()->getElementById(elementId);
}

void
SVGSVGElement::setElementById(const DOMString& id, SVGElement* e)
{
  getOwnerDocument()->putIdentifier(id, e);
}

void
SVGSVGElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo * glinfo)
{
    svg::SVGLength& widthl = MYGETVAL(width);
    svg::SVGLength& heightl = MYGETVAL(height);
    float width = svglContext->computeWidthLength(widthl);
    float height = svglContext->computeHeightLength(heightl);
    glTraverseFromUse(svglContext, glinfo, width, height);
}

  void
  SVGSVGElement::glTraverseFromUse(svgl::Context * svglContext, svgl::GLInfo* glinfo, float width, float height)
  {
    svg::SVGLength& xl = MYGETVAL(x);
    svg::SVGLength& yl = MYGETVAL(y);

    float x = svglContext->computeWidthLength(xl);
    float y = svglContext->computeHeightLength(yl);
		svgl::MultiPassManager multiPassManager;


		if(_compileAsATexture && !_snap) {
			static int count=10;
			if(count>=0)
				--count;
			if(!count) {				
			STDDBG;
			multiPassManager.push(glinfo, 0, 0, width, height);			
#if 0
			glinfo->pushWindowMode();
			glBlendFunc(GL_ONE, GL_ZERO);
			glColor4f(1,0,0,1);
			//glRectf(x,y,x+width,y+height);
			glRectf(0,0,width,height);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glinfo->popWindowMode();
#endif
			// draw it untransformed
			glPushMatrix();
			glLoadIdentity();
			glTranslatef(x,glinfo->winHeight-height,0);
			}
		}
		else {
			glPushMatrix();
			//glLoadIdentity();
			//glTranslatef(x,glinfo->winHeight-height,0);
			glTranslatef(x,y,0);
		}
    
		if(!_compileAsATexture || !_snap) {
			//STDDBG;
			const SVGRect& viewbox = GETVAL(ViewBox);
			SVGPreserveAspectRatio& pAR =  MYGETVAL(preserveAspectRatio);
			pAR.glPreTraverse(svglContext, glinfo, viewbox, width, height);
			float oldwidth = svglContext->viewport_width;
			float oldheight = svglContext->viewport_height;
			svglContext->setViewportWidthHeight(width, height);
			
			glTraverseChildren(svglContext, glinfo);
			
			svglContext->setViewportWidthHeight(oldwidth, oldheight);
			pAR.glPostTraverse(svglContext, glinfo, viewbox, width, height);
		}
		else {
			STDDBG;
			_snap->pop(false);
		}
		
		if(_compileAsATexture && !_snap) {
			static int count=10;
			if(count>=0)
				--count;
			if(!count) {
				_snap = new svgl::TextureSnapshot(0,0,int(width),int(height));
				multiPassManager.pop(glinfo);
				_snap->pop(false);
			}
			glPopMatrix();
		}
		else {
			glPopMatrix();	
		}
		
  }


void
SVGSVGElement::glTraverseBranch(svgl::Context * svglContext, svgl::GLInfo * glinfo, Branch::iterator& cur, Branch::iterator& end)
{
  svg::SVGLength& xl = MYGETVAL(x);
  svg::SVGLength& yl = MYGETVAL(y);
  svg::SVGLength& widthl = MYGETVAL(width);
  svg::SVGLength& heightl = MYGETVAL(height);

  float x = svglContext->computeWidthLength(xl);
  float y = svglContext->computeHeightLength(yl);
  float width = svglContext->computeWidthLength(widthl);
  float height = svglContext->computeHeightLength(heightl);

  glPushMatrix();
  glTranslatef(x,y, 0);

  const SVGRect& viewbox = GETVAL(ViewBox);
  SVGPreserveAspectRatio& pAR =  MYGETVAL(preserveAspectRatio);
  pAR.glPreTraverse(svglContext, glinfo, viewbox, width, height);
  float oldwidth = svglContext->viewport_width;
  float oldheight = svglContext->viewport_height;
  svglContext->setViewportWidthHeight(width, height);

  ++cur;
  if(cur!=end) {
    SVGElement * elem = *cur;
    elem->glTraverseBranch(svglContext, glinfo, cur, end);
  }

  svglContext->setViewportWidthHeight(oldwidth, oldheight);
  pAR.glPostTraverse(svglContext, glinfo, viewbox, width, height);

  glPopMatrix();
}


void
SVGSVGElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo * glinfo)
{
  svg::SVGLength& widthl = MYGETVAL(width);
  svg::SVGLength& heightl = MYGETVAL(height);
  float width = svglContext->computeWidthLength(widthl);
  float height = svglContext->computeHeightLength(heightl);
  pickTraverseFromUse(svglContext, glinfo, width, height);
}

void
SVGSVGElement::pickTraverseFromUse(svgl::Context * svglContext, svgl::GLInfo * glinfo, float width, float height)
{
  svg::SVGLength& xl = MYGETVAL(x);
  svg::SVGLength& yl = MYGETVAL(y);

  float x = svglContext->computeWidthLength(xl);
  float y = svglContext->computeHeightLength(yl);

  glPushMatrix();
  glTranslatef(x,y, 0);

  const SVGRect& viewbox = GETVAL(ViewBox);
  SVGPreserveAspectRatio& pAR =  MYGETVAL(preserveAspectRatio);

  pAR.glPreTraverse(svglContext, glinfo, viewbox, width, height);
  float oldwidth = svglContext->viewport_width;
  float oldheight = svglContext->viewport_height;
  svglContext->setViewportWidthHeight(width, height);
  glinfo->pushPickInfo(this);
  pickTraverseChildren(svglContext, glinfo);
  glinfo->popPickInfo();
  svglContext->setViewportWidthHeight(oldwidth, oldheight);
  pAR.glPostTraverse(svglContext, glinfo, viewbox, width, height);

  glPopMatrix();
}

unsigned long
SVGSVGElement::suspendRedraw ( unsigned long max_wait_milliseconds )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGSVGElement::unsuspendRedraw ( unsigned long suspend_handle_id )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGSVGElement::unsuspendRedrawAll (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGSVGElement::forceRedraw (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGSVGElement::pauseAnimations (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGSVGElement::unpauseAnimations (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

bool
SVGSVGElement::animationsPaused (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

float
SVGSVGElement::getCurrentTime (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGSVGElement::setCurrentTime ( float seconds )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

dom::NodeList*
SVGSVGElement::getIntersectionList ( const SVGRect& rect, const SVGElement& referenceElement )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

dom::NodeList*
SVGSVGElement::getEnclosureList ( const SVGRect& rect, const SVGElement& referenceElement )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

bool
SVGSVGElement::checkIntersection ( const SVGElement& element, const SVGRect& rect )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

bool
SVGSVGElement::checkEnclosure ( const SVGElement& element, const SVGRect& rect )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGSVGElement::deSelectAll (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGNumber*
SVGSVGElement::createSVGNumber (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGLength*
SVGSVGElement::createSVGLength (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGAngle*
SVGSVGElement::createSVGAngle (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGPoint*
SVGSVGElement::createSVGPoint (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGMatrix*
SVGSVGElement::createSVGMatrix (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGRect*
SVGSVGElement::createSVGRect (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGTransform*
SVGSVGElement::createSVGTransform (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

SVGTransform*
SVGSVGElement::createSVGTransformFromMatrix ( const SVGMatrix& matrix )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

DOMString*
SVGSVGElement::createSVGString (  )
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}


}
