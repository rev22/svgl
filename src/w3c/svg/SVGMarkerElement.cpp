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
#include <w3c/svg/SVGMarkerElement.hpp>
#include <w3c/svg/SVGPreserveAspectRatio.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>
#if defined (__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <utils/StringUtils.hpp>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>

namespace svg {

  void
  SVGMarkerElement::setOwnerAndViewPort(SVGSVGElement* owner, SVGElement* viewport)
  {
    SVGElement::setOwnerAndViewPort(owner, this);
    ownerSVGElement.setValue(owner);
    viewportElement.setValue(viewport);
  }


  extern const String orient_attribute_string;

  int
  SVGMarkerElement::customParseAttributes(const String& attrName_ptr, const String& attrValue)
  {
    unicode::String& attrName = *attrName_ptr;
    //unicode::String& orient_attribute_string_ref = *orient_attribute_string;
    unicode::String& orient_attribute_string_ref = *unicode::String::internString("orient");
    unicode::String& auto_string=*unicode::String::internString("auto");
    if(attrName==orient_attribute_string_ref) {
      if(attrName==auto_string) {
	SETVAL(orientType, SVG_MARKER_ORIENT_AUTO);
	return 1;
      }
      else {
	float v;
	if(attrValue && unicode::get_float(*attrValue, &v)) {
	  SETVAL(orientType, SVG_MARKER_ORIENT_ANGLE);
#if 0
	  SETVAL(orientAngle, v);
#endif
	  return 1;
	}
	else {
	  std::cerr << "invalid 'orient' attribute \"" << attrValue << "\"  in <marker>" __FL__;
	  return 0;
	}
      }
    }
    return 0;
  }

  void
  SVGMarkerElement::glTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
  }

  void
  SVGMarkerElement::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
  }

  void
  SVGMarkerElement::glTraverseFromUse(svgl::Context * svglContext, svgl::GLInfo* glinfo, float shapeStrokeWidth, float shapeOrient)
  {
    svg::SVGLength refxl = GETVAL(RefX);
    svg::SVGLength refyl = GETVAL(RefY);
    svg::SVGLength markerwidthl = GETVAL(MarkerWidth);
    svg::SVGLength markerheightl = GETVAL(MarkerHeight);

    float markerWidth = svglContext->computeWidthLength(markerwidthl);
    float markerHeight = svglContext->computeHeightLength(markerheightl);

    switch(GETVAL(MarkerUnits)) {
    case SVG_MARKERUNITS_STROKEWIDTH:
      markerWidth*=shapeStrokeWidth;
      markerHeight*=shapeStrokeWidth;      
      break;
    case SVG_MARKERUNITS_USERSPACEONUSE:
    default:
      break;
    }


    // spec not clear (dtd != idl)
    #if 1
    float angle=shapeOrient;
    #else
    float orientAngle = GETVAL(OrientAngle);

    float angle=0;
    switch(GETVAL(OrientType)) {
    case SVG_MARKER_ORIENT_AUTO:
      angle=shapeOrient;
      break;
    case SVG_MARKER_ORIENT_ANGLE:
      angle=orientAngle;
      break;
    default:
      break;
    }
    #endif
    
    glPushMatrix();

    SVGRect viewbox = GETVAL(ViewBox);
    SVGPreserveAspectRatio pAR =  GETVAL(PreserveAspectRatio);
    pAR.glPreTraverse(svglContext, glinfo, viewbox, markerWidth, markerHeight);

    float oldwidth = svglContext->viewport_width;
    float oldheight = svglContext->viewport_height;
    svglContext->setViewportWidthHeight(markerWidth, markerHeight);

    float refX = svglContext->computeWidthLength(refxl);
    float refY = svglContext->computeHeightLength(refyl);

    glRotatef(angle,0,0,1);
    glTranslatef(-refX,-refY,0); // why the minus ?????

    glTraverseChildren(svglContext, glinfo);

    svglContext->setViewportWidthHeight(oldwidth, oldheight);
    pAR.glPostTraverse(svglContext, glinfo, viewbox, markerWidth, markerHeight);
    glPopMatrix();

  }

  void
  SVGMarkerElement::pickTraverseFromUse(svgl::Context * svglContext, svgl::GLInfo* glinfo, float shapeStrokeWidth, float shapeOrient)
  {
    svg::SVGLength refxl = GETVAL(RefX);
    svg::SVGLength refyl = GETVAL(RefY);
    svg::SVGLength markerwidthl = GETVAL(MarkerWidth);
    svg::SVGLength markerheightl = GETVAL(MarkerHeight);

    float markerWidth = svglContext->computeWidthLength(markerwidthl);
    float markerHeight = svglContext->computeHeightLength(markerheightl);

    switch(GETVAL(MarkerUnits)) {
    case SVG_MARKERUNITS_STROKEWIDTH:
      markerWidth*=shapeStrokeWidth;
      markerHeight*=shapeStrokeWidth;      
      break;
    case SVG_MARKERUNITS_USERSPACEONUSE:
    default:
      break;
    }


    // spec not clear (dtd != idl)
    #if 1
    float angle=shapeOrient;
    #else
    float orientAngle = GETVAL(OrientAngle);

    float angle=0;
    switch(GETVAL(OrientType)) {
    case SVG_MARKER_ORIENT_AUTO:
      angle=shapeOrient;
      break;
    case SVG_MARKER_ORIENT_ANGLE:
      angle=orientAngle;
      break;
    default:
      break;
    }
    #endif
    
    glPushMatrix();

    SVGRect viewbox = GETVAL(ViewBox);
    SVGPreserveAspectRatio pAR =  GETVAL(PreserveAspectRatio);
    pAR.glPreTraverse(svglContext, glinfo, viewbox, markerWidth, markerHeight);

    float oldwidth = svglContext->viewport_width;
    float oldheight = svglContext->viewport_height;
    svglContext->setViewportWidthHeight(markerWidth, markerHeight);

    float refX = svglContext->computeWidthLength(refxl);
    float refY = svglContext->computeHeightLength(refyl);

    glRotatef(angle,0,0,1);
    glTranslatef(-refX,-refY,0); // why the minus ?????

    glinfo->pushPickInfo(this);
    pickTraverseChildren(svglContext, glinfo);
    glinfo->popPickInfo();

    svglContext->setViewportWidthHeight(oldwidth, oldheight);
    pAR.glPostTraverse(svglContext, glinfo, viewbox, markerWidth, markerHeight);
    glPopMatrix();

  }

void
SVGMarkerElement::setOrientToAuto (  ) 
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

void
SVGMarkerElement::setOrientToAngle ( const SVGAngle& angle ) 
{
  throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
}

}

