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
#include <w3c/svg/SVGStylable.hpp>
#include <w3c/svg/SVGClipPathElement.hpp>
#include <w3c/svg/SVGFilterElement.hpp>
#include <w3c/svg/SVGMarkerElement.hpp>
//#include <w3c/svg/SVGSVGElement.hpp>
//#include <w3c/svg/SVGDocument.hpp>
#include <w3c/dom/Document.hpp>
#include <w3c/css/CSSStyleDeclaration.hpp>
#include <w3c/css/RGBColor.hpp>

#include <svgl/Context.hpp>
#include <svgl/GLInfo.hpp>

#include <svgl/bezierpath.hpp>
#include <svgl/BoundingBoxManager.hpp>
#include <svgl/MultiPassManager.hpp>
#include <svgl/ExternalEntityManager.hpp>

#include <svgl/shape_type.h>
#include <svgl/glPoly.h>
#include <svgl/agg2glPoly.h>

#include <svgl/getattr.hpp>
#include <svgl/debug.hpp>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

namespace svg {

  int
  SVGStylable::customParseAttributes(const String& attrName_ptr, const String& attrValue)
  {
    static unicode::String
      * column = unicode::String::internString(":"),
      * semicolumn = unicode::String::internString(";");

    //std::cerr << DBGVAR(attrName_ptr) << DBGVAR(attrValue) << __FL__;
    if(!attrName_ptr || !css::CSSStyleDeclaration::isStyleAttribute(attrName_ptr)) {
#if 0
      if(attrName_ptr)
	std::cerr << "\"" << attrName_ptr << "\"";
      std::cerr << "not a style attr" << __FL__;
#endif
      return 0;
    }


    unicode::String  * onestyle = attrName_ptr;
    onestyle = concat(onestyle, column);
    onestyle = concat(onestyle, attrValue);
    onestyle = concat(onestyle, semicolumn);
    style.getValue().appendStyleString(onestyle);
    return 1;
  }




  template<class Element>
  inline
  Element
  getRefElement(const dom::string& uri, svgl::ExternalEntityManager * entityManager, dom::Document * currentDoc)
  {
    if (uri && uri->getLength() && (*uri)[0]=='#') {
      //std::cerr << DBGVAR(uri) << std::endl;
      //dom::Element* de = owner->getElementById(uri->substring(1));
      dom::Element* de = entityManager->get(uri, currentDoc);
      //std::cerr << DBGVAR(de) << std::endl;
      return dynamic_cast<Element>(de);
    }
    return 0;
  }
  //  SVGStylable::SVGStylable(){}

  //SVGStylable::~SVGStylable() { }

#define GETWHATEVER(type,which) \
  SVG##type##Element* \
  SVGStylable::getSVG##which##Element(svgl::ExternalEntityManager * entityManager, dom::Document * currentDoc) \
  { \
    css::CSSStyle::UriType uri = getStyle().getValue().get##which(); \
    return getRefElement<svg::SVG##type##Element*>(uri.uri, entityManager, currentDoc); \
  }

    GETWHATEVER(ClipPath,ClipPath)
    GETWHATEVER(Filter,Filter)
    GETWHATEVER(Marker,Marker)
    GETWHATEVER(Marker,MarkerStart)
    GETWHATEVER(Marker,MarkerEnd)
    GETWHATEVER(Marker,MarkerMid)

#undef GETWHATEVER


  void
  SVGStylable::glTraverseMarker(svgl::Context* svglContext, svgl::GLInfo* glinfo, float shapeStrokeWidth)
  {
    SVGElement * meAsElement = dynamic_cast<SVGElement*>(this);
    dom::Document * currentDoc = meAsElement->getOwnerDocument();
    SVGMarkerElement* marker = getSVGMarkerElement(svglContext->externalEntityManager, currentDoc);
    SVGMarkerElement *marker_start=marker, *marker_end=marker, *marker_mid=marker;
    marker_start = getSVGMarkerStartElement(svglContext->externalEntityManager, currentDoc);
    marker_end = getSVGMarkerEndElement(svglContext->externalEntityManager, currentDoc);
    marker_mid = getSVGMarkerMidElement(svglContext->externalEntityManager, currentDoc);

    if( !(marker || marker_start || marker_end || marker_mid) )
      return;

    BezierPath bezierpath;
    asBezier(svglContext, glinfo, &bezierpath);

#if 0
    SVGStylable* stylable = dynamic_cast<SVGStylable*>(shape);
    if(!stylable) {
      std::cerr << "unstylable referring shape for marker " __FL__;
      return;
    }
#endif
    
    SVGStylable* stylable = this;

    if(!marker_start) {
      marker_start=marker;
    } else {
      css::CSSStyleDeclaration& style = marker_start->getStyle().getValue();
      style.setParent(stylable->getStyle().getValue().getParent());
    }

    if(!marker_end) {
      marker_end=marker;
    } else {
      css::CSSStyleDeclaration& style = marker_end->getStyle().getValue();
      style.setParent(stylable->getStyle().getValue().getParent());
    }

    if(!marker_mid) {
      marker_mid=marker;
    } else {
      css::CSSStyleDeclaration& style = marker_mid->getStyle().getValue();
      style.setParent(stylable->getStyle().getValue().getParent());
    }

    if(marker) {
      css::CSSStyleDeclaration& style = marker->getStyle().getValue();
      style.setParent(stylable->getStyle().getValue().getParent());
    }
    
    marker=0;


    glPushMatrix();
    for (BezierPath::const_iterator i = bezierpath.begin();
	 i!=bezierpath.end();
	 ++i) {

      if(marker)
	marker->glTraverseFromUse(svglContext, glinfo, shapeStrokeWidth, 0);

      switch ( (*i).type() ) {

      case BezierPath::move: {
	const BezierPath::Point& p0 = (*i)[0];
	glTranslatef(p0.x(), p0.y(), 0);
	marker = marker_start;
      }
      break;

      case BezierPath::line: {
	const BezierPath::Point& p0 = (*i)[0];
	const BezierPath::Point& p1 = (*i)[1];
	BezierPath::Coord dx = p1.x()-p0.x();
	BezierPath::Coord dy = p1.y()-p0.y();
	glTranslatef(dx, dy, 0);
	marker = marker_mid;
      }
      break;
      
      case BezierPath::close: {
	const BezierPath::Point& p0 = (*i)[0];
	const BezierPath::Point& p1 = (*i)[1];
	BezierPath::Coord dx = p1.x()-p0.x();
	BezierPath::Coord dy = p1.y()-p0.y();
	glTranslatef(dx, dy, 0);
	marker = marker_end;
      }
      break;
      default:
	STDDBG;
	break;
      }
    }

    marker=marker_end;
    if(marker)
      marker->glTraverseFromUse(svglContext, glinfo, shapeStrokeWidth, 0);

    glPopMatrix();
  }


  void
  SVGStylable::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, BezierPath* bezier)
  {
  }

	
	// mmm should be pure-virtual
	// basically, turn any shape into an agg path, with the highest-level command available (bezier, arc(?)) etc...
	void
	SVGStylable::asBezier(svgl::Context* svglContext, svgl::GLInfo* glinfo, agg::path_storage* ppath)
  {
  }		

	
  void
  SVGStylable::getBoundingBox(svgl::Context* svglContext, svgl::GLInfo* glinfo, float* bbx, float* bby, float* bbwidth, float* bbheight)
  {
    *bbx=*bby=*bbwidth=*bbheight=0;
  }

	/*
	Drawing scheme
	
	the default Stylable drawing implementation gets the geometry as an agg:path,
	translate it to 2 glPolys (fill and stroke), and draw them:
	
	SVGStylable::glTraverse
	 asBezier(path)
	 computeGeometryForDrawing(path) -> fillGlPoly and strokeGlPoly
	 draw fillGlPoly and strokeGlPoly
	
	however, some shapes can be drawn that way:
	 Line: (no fill possible)
	 Image: render the image
	 Text: use textures of fonts
	
	The basic aglorithm asks for geometry. Since it's useless in the case of Image and TextContent, we separate two kinds of geometry query:
	 one targeted at drawing (null in TextContent and Image), the other one for future purposes (turn everything into a path ie TextOnPath)
	*/
	
	void
	SVGStylable::computeGeometryForDrawing(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
		computeGeometry(svglContext, glinfo);
	}
	
	void
	SVGStylable::computeGeometry(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
		SVGElement * meAsElement = dynamic_cast<SVGElement*>(this);
		if(!meAsElement)
			return;
		bool damaged = meAsElement->getDamaged(); 
		if(!(damaged))
			return;
		
		shape_type * gshape = new shape_type;
		agg::path_storage * ppath = new agg::path_storage;
		asBezier(svglContext, glinfo, ppath);
		shape_type::contour_type *  pcontour = new shape_type::contour_type(*ppath);
		gshape->_path = ppath;
		gshape->_contour = pcontour;

		fillGlPoly = new glPoly;
		fillGlPoly->setTessCallbacks(glinfo->gluTriangulator);
		agg2glPoly(*pcontour, *fillGlPoly, glinfo->gluTriangulator);

		const css::CSSStyleDeclaration& style = GET_SIMPLE_VAL(Style);
		svg::SVGLength strokeWidth = style.getStrokeWidth();
    double sw = svglContext->computeWidthLength(strokeWidth);
		
		if (sw > 0) {			
			const css::CSSStyle::StringType& linecap = style.getStrokeLinecap();
			const css::CSSStyle::StringType& linejoin = style.getStrokeLinejoin();
			const css::CSSStyle::FloatType& strokeMiterLimit = style.getStrokeMiterlimit();
			const css::CSSStyle::StrokeDasharrayType& strokeDasharray = style.getStrokeDasharray();
			css::CSSStyle::LengthType strokeDashoffset = style.getStrokeDashoffset();
			float dashOffset = svglContext->computeWidthLength(strokeDashoffset);
			
			//std::cerr << DBGVAR(dashOffset) << __FL__;
			
			if(!strokeDasharray.empty()) {
				shape_type::dash_type * pdash = new shape_type::dash_type(*pcontour);
				shape_type::dash_type& dash = *pdash;
				
				for(css::CSSStyle::StrokeDasharrayType::const_iterator it = strokeDasharray.begin();
						it != strokeDasharray.end();
						++it) {
					double a,b;
					a = *it++;
					b = *it;
					dash.add_dash(a,b);
				}
				dash.dash_start(dashOffset);
				
				gshape->_dash= new shape_type::dashstroke_type(*pdash);
			}
			else
				gshape->_plain = new shape_type::plainstroke_type(*pcontour);
						
			shape_type& shape = *gshape;
			
			static unicode::String
				// *butt_cap_string = unicode::String::internString("butt"),
				*square_cap_string = unicode::String::internString("square"),
				*round_cap_string = unicode::String::internString("round"),
				// *miter_join_string = unicode::String::internString("miter"),
				*round_join_string = unicode::String::internString("round"),
				*bevel_join_string = unicode::String::internString("bevel");
			
			//std::cerr << DBGVAR(linecap) << __FL__;
			//std::cerr << DBGVAR(linejoin) << __FL__;
			
			agg::gen_stroke::line_cap_e cap = agg::gen_stroke::butt_cap;
			if(*linecap == *square_cap_string) {
				cap = agg::gen_stroke::square_cap;
			} else
				if(*linecap == *round_cap_string) {
					cap = agg::gen_stroke::round_cap;
				}
			
			agg::gen_stroke::line_join_e join = agg::gen_stroke::miter_join;
			if(*linejoin == *round_join_string) {
				join = agg::gen_stroke::round_join;
			} else
				if(*linejoin == *bevel_join_string) {
					join = agg::gen_stroke::bevel_join;
				}
			
			shape.line_join(join);
			shape.line_cap(cap);
			shape.miter_limit(strokeMiterLimit);
			shape.width(sw);
			
			strokeGlPoly = new glPoly;
			strokeGlPoly->setTessCallbacks(glinfo->gluTriangulator);
			gluTessProperty(glinfo->gluTriangulator, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);			
			agg2glPoly(*gshape, *strokeGlPoly, glinfo->gluTriangulator);
			gluTessProperty(glinfo->gluTriangulator, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
		}
  
	}
	
  void
  SVGStylable::glTraverseFill(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
		draw(*fillGlPoly);
  }

  void
  SVGStylable::glTraverseStroke(svgl::Context * svglContext, svgl::GLInfo* glinfo, float sw)
  {
		if(sw==0) return;
#if 1
		draw(*strokeGlPoly);
#else		
		double scale = glinfo->getCurrentScale();
		//if(scale*sw>2) {
		if(0) {
			draw(*strokeGlPoly);
		} else {
			// TODO : set up dash for contour

			// draw the contour, but it must be dashed as well !!!
			shape_type * gshape = new shape_type;
			agg::path_storage * ppath = new agg::path_storage;
			asBezier(svglContext, glinfo, ppath);
			shape_type::contour_type *  pcontour = new shape_type::contour_type(*ppath);
			gshape->_path = ppath;
			gshape->_contour = pcontour;

			unsigned int cmd;
			bool begun=false;
			double x,y,lastx, lasty;
			vector<GLfloat> coords;
#if 0
			SVGElement * meAsElement = dynamic_cast<SVGElement*>(this);
			if(meAsElement)
				std::cerr << meAsElement->getId() << __FL__;
#endif
			pcontour->rewind(0);
			do {
				cmd = pcontour->vertex(&x,&y);
				//std::cerr << std::hex << DBGVAR(cmd) << std::dec << DBGVAR(x) << DBGVAR(y) << std::endl;
				
				if(agg::is_move_to(cmd)) {
					if(begun) {
						glInterleavedArrays(GL_V3F, 0, &coords[0]);
						glDrawArrays(GL_LINE_STRIP, 0, coords.size()/3);
						begun=false;
					}
					lastx=x;
					lasty=y;
				} else
					if (agg::is_line_to(cmd)) {
						if(!begun) {
							coords.clear();
							coords.push_back(lastx);
							coords.push_back(lasty);
							coords.push_back(0);
							begun=true;
						}
						coords.push_back(x);
						coords.push_back(y);
						coords.push_back(0);
					} else if (agg::is_end_poly(cmd)) {
						if(agg::is_close(cmd)) {
							coords.push_back(lastx);
							coords.push_back(lasty);
							coords.push_back(0);
						}
						glInterleavedArrays(GL_V3F, 0, &coords[0]);
						glDrawArrays(GL_LINE_STRIP, 0, coords.size()/3);
						begun=false;
					} else { //stop_cmd
						//std::cerr << "unexpected agg_path command " <<  std::hex << cmd << std::dec << std::endl;
					}
			} while(cmd != agg::path_cmd_stop);
			if(begun)  {
				glInterleavedArrays(GL_V3F, 0, &coords[0]);
				glDrawArrays(GL_LINE_STRIP, 0, coords.size()/3);
				begun=false;
			}
			

#if 0
			const css::CSSStyle::FloatType& strokeMiterLimit = style.getStrokeMiterlimit();
			const css::CSSStyle::StrokeDasharrayType& strokeDasharray = style.getStrokeDasharray();
			css::CSSStyle::LengthType strokeDashoffset = style.getStrokeDashoffset();
			float dashOffset = svglContext->computeWidthLength(strokeDashoffset);
			
			//std::cerr << DBGVAR(dashOffset) << __FL__;
			
			if(!strokeDasharray.empty()) {
				shape_type::dash_type * pdash = new shape_type::dash_type(*pcontour);
				shape_type::dash_type& dash = *pdash;
				
				for(css::CSSStyle::StrokeDasharrayType::const_iterator it = strokeDasharray.begin();
						it != strokeDasharray.end();
						++it) {
					double a,b;
					a = *it++;
					b = *it;
					dash.add_dash(a,b);
				}
				dash.dash_start(dashOffset);
			}			
#endif			
		}
#endif		
			
	}

  void
  SVGStylable::glTraverse(svgl::Context * svglContext,
			  svgl::GLInfo* glinfo,
			  bool stencilStrokeHint,
			  bool traverseEvenIfNonVisible)
  {

    const css::CSSStyleDeclaration& style = GET_SIMPLE_VAL(Style);

    const css::Color * fill = style.getFill();
    const css::Color * stroke = style.getStroke();
    float fillOpacity = style.getFillOpacity();
    float strokeOpacity = style.getStrokeOpacity();
    float opacity = style.getOpacity();
    svg::SVGLength strokeWidth = style.getStrokeWidth();
    float sw = svglContext->computeWidthLength(strokeWidth);
    css::CSSStyle::DisplayType display = style.getDisplay();
		
		//FIXME
    if(fill==css::Color::getNoneColor())
      fill = 0; // as if it wasn't there...
    if(stroke==css::Color::getNoneColor())
      stroke = 0; // as if it wasn't there...

    if(!display) {
      return;
    }

		computeGeometryForDrawing(svglContext,glinfo);
		
    SVGElement * meAsElement = dynamic_cast<SVGElement*>(this);
    dom::Document * currentDoc = meAsElement->getOwnerDocument();

    svg::SVGClipPathElement *clippath_element=0;
    clippath_element = getSVGClipPathElement(svglContext->externalEntityManager, currentDoc);
    if(clippath_element) {
      clippath_element->glPreTraverse(svglContext, glinfo);
    }

    SVGFilterElement* filter_element = 0;
#if 1
    filter_element = getSVGFilterElement(svglContext->externalEntityManager, currentDoc);
    if(filter_element)
      filter_element->glPreTraverse(svglContext, glinfo, dynamic_cast<SVGElement*>(this));
#endif

    if(fill && stroke && opacity<1) {
      glTraverseOpacity(svglContext,
			glinfo,
			fill,
			stroke,
			fillOpacity,
			strokeOpacity,
			opacity,
			sw,
			stencilStrokeHint
			//,
			//bbx,bby,bbwidth,bbheight
			);
    }
    else {
      float alpha_save = glinfo->alpha;
      glinfo->alpha *= opacity;

#define AA 1

#if AA
//#define AA_PRIMITIVE GL_POLYGON_SMOOTH
//#define AA_PRIMITIVE GL_MULTISAMPLE_ARB
#define AA_PRIMITIVE glinfo->aa_primitive
#define AA_PRIMITIVE_HINT GL_POLYGON_SMOOTH_HINT
#endif

      if(fill) {
	float alpha_save = glinfo->alpha;
	glinfo->alpha *= fillOpacity;

	fill->glPreTraverse(svglContext, glinfo, this);
	if(!stroke && glinfo->doAntiAliasing()) {
	  bool opaque = false;
	  if(glinfo->alpha==1) opaque = true;
	  glinfo->enableAntiAliasing(opaque);
	}

	glTraverseFill(svglContext, glinfo);
	fill->glPostTraverse(svglContext, glinfo, this);

	if(!stroke && glinfo->doAntiAliasing()) {
	  glinfo->disableAntiAliasing();
	}
	glinfo->alpha = alpha_save;
      }

      if(stroke && sw > 0) {
	float alpha_save = glinfo->alpha;
	float swon2 = sw/2.;
	float bbx,bby,bbwidth,bbheight;

	stencilStrokeHint=1;
	if(stencilStrokeHint && strokeOpacity<1) {
	  // since stroke pathes may recover themselves (think "8") , we must use stencil
#if 1
	  glinfo->stencilEnableTopAndPush();
#else
	  glinfo->stencilEnableTest();
	  int& stencil = glinfo->stencil;
	  glStencilFunc(GL_EQUAL, stencil, static_cast<GLuint>(-1));
	  glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	  ++stencil;
#endif
	}

	glinfo->alpha *= strokeOpacity;
	stroke->glPreTraverse(svglContext, glinfo, this, sw);

	if(glinfo->doAntiAliasing()) {
	  bool opaque = false;
	  if(glinfo->alpha==1) opaque = true;
	  glinfo->enableAntiAliasing(opaque);
	}

	glTraverseStroke(svglContext, glinfo, sw);

	if(glinfo->doAntiAliasing()) {
	  glinfo->disableAntiAliasing();
	}

	// remove stencil
	if(stencilStrokeHint && strokeOpacity<1) {
	  // FIXME, not necessary if stroke is a gradient...
	  // not sure though...
	  getBoundingBox(svglContext, glinfo, &bbx, &bby, &bbwidth, &bbheight);
	  glinfo->stencilPop();
	  glBegin(GL_QUAD_STRIP);
	  glVertex2f(bbx-swon2,bby-swon2);
	  glVertex2f(bbx-swon2, bby+bbheight+sw);
	  glVertex2f(bbx+bbwidth+sw, bby-swon2);
	  glVertex2f(bbx+bbwidth+sw, bby+bbheight+sw);
	  glEnd();
	  glinfo->stencilEnableTop();
	}

	stroke->glPostTraverse(svglContext, glinfo, this, sw);
	glinfo->alpha = alpha_save;
	
	

#if AA
#undef AA_PRIMITIVE
#undef AA_PRIMITIVE_HINT
#undef AA
#endif
      }

      glinfo->alpha = alpha_save;
    }

    SVGStylable::glTraverseMarker(svglContext, glinfo, sw);

    if(filter_element) {
      filter_element->glPostTraverse(svglContext, glinfo, dynamic_cast<SVGElement*>(this));
    }

    if(clippath_element) {
      clippath_element->glPostTraverse(svglContext, glinfo);
    }

#if 0
		// draw bbox
		//if(glinfo->_has_GL_ARB_imaging) {
		if(1) {
			float swon2 = sw/2.;
			float bbx,bby,bbwidth,bbheight;
			getBoundingBox(svglContext, glinfo, &bbx, &bby, &bbwidth, &bbheight);

			glinfo->stencilDisableTest();
			glColor4d(0,0,0,1);
			glBegin(GL_LINE_LOOP);
			glVertex2f(bbx-swon2,bby-swon2);
			glVertex2f(bbx-swon2, bby+bbheight+swon2);
			glVertex2f(bbx+bbwidth+swon2, bby+bbheight+swon2);
			glVertex2f(bbx+bbwidth+swon2, bby-swon2);
			glEnd();
			glinfo->stencilEnableTest();
		}
		// --
#endif
		

  }

  void
  SVGStylable::glTraverseOpacity(svgl::Context * svglContext,
				 svgl::GLInfo* glinfo,
				 const css::Color * fill,
				 const css::Color * stroke,
				 float fillOpacity,
				 float strokeOpacity,
				 float opacity,
				 float strokeWidth,
				 bool stencilStrokeHint
				 )
  {
    float sw = strokeWidth;
    float swon2 = sw/2.;
    float bbx,bby,bbwidth,bbheight;
    getBoundingBox(svglContext, glinfo, &bbx, &bby, &bbwidth, &bbheight);


    if(strokeOpacity==1) { // stroke is opaque
      // we can draw the stroke with stencil on,
      // then draw the fill where the stencil is off

      // stroke
      glinfo->stencilEnableTopAndPush();
      float alpha_save = glinfo->alpha;
      glinfo->alpha *= strokeOpacity * opacity;
      stroke->glPreTraverse(svglContext, glinfo, this, sw);
      glTraverseStroke(svglContext, glinfo, sw);
      stroke->glPostTraverse(svglContext, glinfo, this, sw);

      // fill only parts that do not belong to the stroke
      glinfo->alpha = alpha_save * fillOpacity * opacity;
      fill->glPreTraverse(svglContext, glinfo, this);
      glTraverseFill(svglContext, glinfo);
      fill->glPostTraverse(svglContext, glinfo, this);
      glinfo->alpha = alpha_save;

      // remove stencil
      glinfo->stencilPop();
      glBegin(GL_QUAD_STRIP);
      glVertex2f(bbx-swon2,bby-swon2);
      glVertex2f(bbx-swon2, bby+bbheight+sw);
      glVertex2f(bbx+bbwidth+sw, bby-swon2);
      glVertex2f(bbx+bbwidth+sw, bby+bbheight+sw);
      glEnd();
      glinfo->stencilEnableTop();
    }

    else { // stroke is not opaque
      svgl::BoundingBoxManager bboxManager;
      glPushMatrix();

      bboxManager.glPreTraverse(glinfo);

      glBegin(GL_LINE_LOOP);
      glVertex2f(bbx-swon2,bby-swon2);
      glVertex2f(bbx+sw+bbwidth,bby-swon2);
      glVertex2f(bbx+sw+bbwidth,bby+sw+bbheight);
      glVertex2f(bbx-swon2,bby+sw+bbheight);

      glVertex2f(bbx-swon2,bby-swon2);
      glVertex2f(bbx+sw+bbwidth,bby+sw+bbheight);
      glVertex2f(bbx+sw+bbwidth,bby-swon2);
      glVertex2f(bbx-swon2,bby+sw+bbheight);
      glEnd();  

      float xminf,yminf,xmaxf,ymaxf;
      bool snap_done = bboxManager.glPostTraverse(glinfo, &xminf, &yminf, &xmaxf, &ymaxf);
      glPopMatrix();

      if(snap_done) {
	svgl::MultiPassManager multiPassManager;
	multiPassManager.push(glinfo, xminf, yminf, xmaxf, ymaxf);
	float alpha_save  = glinfo->alpha;

	// fill
	glinfo->alpha = alpha_save * fillOpacity;
	fill->glPreTraverse(svglContext, glinfo, this);
	glTraverseFill(svglContext, glinfo);
	fill->glPostTraverse(svglContext, glinfo, this);
	  
	// stroke	
	//stencilStrokeHint=0;
	if(stencilStrokeHint) {
	  // since stroke pathes recover themself, we must use stencil
#if 1
	  glinfo->stencilEnableTopAndPush();
#else
	  glinfo->stencilEnableTest();
	  int& stencil = glinfo->stencil;
	  glStencilFunc(GL_EQUAL, stencil, static_cast<GLuint>(-1));
	  glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	  ++stencil;
#endif
	}

	glinfo->alpha = alpha_save * strokeOpacity;
	stroke->glPreTraverse(svglContext, glinfo, this, sw);
	glTraverseStroke(svglContext, glinfo, sw);
#if 0
	if(stencilStrokeHint) {
	  glinfo->stencilEnableTop();
	}
#endif
	stroke->glPostTraverse(svglContext, glinfo, this, sw);

	// remove stencil
	if(stencilStrokeHint) {
	  // FIXME, not necessary if stroke is a gradient...
	  // not sure though...
	  glinfo->stencilPop();
	  glBegin(GL_QUAD_STRIP);
	  glVertex2f(bbx-swon2,bby-swon2);
	  glVertex2f(bbx-swon2, bby+bbheight+sw);
	  glVertex2f(bbx+bbwidth+sw, bby-swon2);
	  glVertex2f(bbx+bbwidth+sw, bby+bbheight+sw);
	  glEnd();
	  glinfo->stencilEnableTop();
	}
	glinfo->alpha = opacity;
	multiPassManager.pop(glinfo);
	glinfo->alpha = alpha_save;

      }
    }

  }

  void
  SVGStylable::pickTraverse(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {

    const css::CSSStyleDeclaration& style = getStyle().getValue();

    const css::Color * fill = style.getFill();
    const css::Color * stroke = style.getStroke();
    svg::SVGLength strokeWidth = style.getStrokeWidth();
    float sw = svglContext->computeWidthLength(strokeWidth);
    SVGElement * e = dynamic_cast<SVGElement*>(this);
    if(!e) {
      std::cerr << "SVGStylable must be an SVGElement in pickTraverse" __FL__;
      return;
    }

    //FIXME
    if(fill==css::Color::getNoneColor())
      fill = 0; // as if it wasn't there...
    if(stroke==css::Color::getNoneColor())
      stroke = 0; // as if it wasn't there...

		computeGeometryForDrawing(svglContext,glinfo);
		
    dom::Document * currentDoc = e->getOwnerDocument();

    svg::SVGClipPathElement *clippath_element=0;
    clippath_element = getSVGClipPathElement(svglContext->externalEntityManager, currentDoc);
    if(clippath_element)
      clippath_element->pickTraverse(svglContext, glinfo);

    glinfo->pushPickInfo(e);
    if(fill) {
      glTraverseFill(svglContext, glinfo);
    }
    if(stroke) {
      glTraverseStroke(svglContext, glinfo, sw);
    }
    glinfo->popPickInfo();
  }


  void
  SVGStylable::glTraverseBoundingBox(svgl::Context * svglContext, svgl::GLInfo* glinfo)
  {
    float x,y,width,height;
    getBoundingBox(svglContext, glinfo, &x, &y, &width, &height);
    const css::CSSStyleDeclaration& style = getStyle().getValue();
    svg::SVGLength strokeWidth = style.getStrokeWidth();
    float sw = svglContext->computeWidthLength(strokeWidth);
    float swon2 = sw/2.0;
    glBegin(GL_LINE_LOOP);

    glVertex2f(x-swon2,y-swon2);
    glVertex2f(x+sw+width,y-swon2);
    glVertex2f(x+sw+width,y+sw+height);
    glVertex2f(x-swon2,y+sw+height);

    glVertex2f(x-swon2,y-swon2);
    glVertex2f(x+sw+width,y+sw+height);
    glVertex2f(x+sw+width,y-swon2);
    glVertex2f(x-swon2,y+sw+height);

    glEnd();  
  }

  css::CSSValue*
  SVGStylable::getPresentationAttribute ( const DOMString& name ) 
  {
    throw dom::DOMException(dom::DOMException::NOT_SUPPORTED_ERR);
  }


  // -----------------------------------------------------------------
  // style svgl ad-hoc API


#define ATTRDEF(type, name, dtdstr)											\
															\
  css::CSSStyle::type													\
  SVGStylable::get##name() const											\
  {															\
    const css::CSSStyleDeclaration& style = GET_SIMPLE_VAL(Style);							\
    css::CSSStyle::type name = style.get##name();									\
    return name;													\
  }															\
															\
  bool															\
  SVGStylable::isMy##name() const											\
  {															\
    const SVGElement * meAsElement = dynamic_cast<const SVGElement*>(this);						\
    if(!meAsElement) {													\
      return false;													\
    }															\
															\
    return (meAsElement==getElementDefining##name());									\
															\
  }															\
															\
  void															\
  SVGStylable::unsetMy##name()												\
  {															\
    css::CSSStyleDeclaration& style = const_cast<css::CSSStyleDeclaration&>(GET_SIMPLE_VAL(Style));			\
    const css::CSSOnePatchStyle* genericPatch = style.getStyle()->findOnePatch##name();					\
    css:: name##Style* patch = dynamic_cast<css:: name##Style*>(const_cast<css::CSSOnePatchStyle*>(genericPatch));	\
															\
    if(! patch) {													\
      return;														\
    }															\
    else {														\
      css::CSSOnePatchStyle * s = dynamic_cast<css::CSSOnePatchStyle*>(const_cast<css::CSSStyle*>(style.getStyle()));	\
      if(s == patch) {													\
	style.setStyle(const_cast<css::CSSStyle*>(patch->getNext()));							\
	updateChildrenStyle();												\
      }															\
      else {														\
	while (s && (s->getNext() != patch))										\
	  s=dynamic_cast<css::CSSOnePatchStyle*>(const_cast<css::CSSStyle*>(s->getNext()));				\
	if(s) {														\
	  s->setNext(const_cast<css::CSSStyle*>(patch->getNext()));							\
	}														\
      }															\
    }															\
															\
  }															\
															\
  svg::SVGElement*													\
  SVGStylable::getElementDefining##name() const										\
  {															\
    /* return 0 if default...*/												\
    const SVGElement * asElement = 0;											\
    const SVGStylable * stylable = this;										\
															\
    while(stylable) {													\
      asElement = dynamic_cast<const SVGElement*>(stylable);								\
      SVGStylable * ncstylable = const_cast<SVGStylable*>(stylable) ;							\
															\
      css::CSSStyleDeclaration& style = const_cast<css::CSSStyleDeclaration&>(ncstylable->GET_SIMPLE_VAL(Style));	\
      const css::CSSOnePatchStyle* patch = style.getStyle()->findOnePatch##name();					\
      if(patch)														\
	break;														\
															\
      stylable = dynamic_cast<svg::SVGStylable*>(asElement->getParentNode());						\
    }															\
															\
    return const_cast<svg::SVGElement*>(asElement);									\
															\
  }															\
															\
															\
  void															\
  SVGStylable::set##name(css::CSSStyle::type c)										\
  {															\
    css::CSSStyleDeclaration& styleDecl = const_cast<css::CSSStyleDeclaration&>(GET_SIMPLE_VAL(Style));			\
    css::CSSStyle * style = const_cast<css::CSSStyle*>(styleDecl.getStyle());						\
    css:: name##Style * patch = 0;											\
    if(style) {														\
      const css::CSSOnePatchStyle* genericPatch = style->findOnePatch##name();						\
      patch = dynamic_cast<css:: name##Style*>(const_cast<css::CSSOnePatchStyle*>(genericPatch));			\
    }															\
    if(! patch) {													\
      patch = new css:: name##Style(style, c);										\
      styleDecl.setStyle(patch);											\
      updateChildrenStyle();												\
    }															\
    else {														\
      patch->set(c);													\
    }															\
    SVGElement* meAsSVGElement = dynamic_cast<SVGElement*>(this);					       		\
    if(meAsSVGElement) meAsSVGElement->setStyleDamaged(true);								\
  }


	extern
	bool
	getRGBColor(const unicode::String* s_ptr, css::Color** pcolor, unicode::String::size_type beg=0, unicode::String::size_type end=unicode::String::npos);
	

#include <w3c/css/inherited_style_defs.hpp>
#include <w3c/css/not_inherited_style_defs.hpp>
#undef ATTRDEF

#define ATTRDEF(type, name, dtdstr)			\
  void							\
  SVGStylable::set##name(float r, float g, float b)	\
  {							\
    css::RGBColor * c = new css::RGBColor;		\
    c->r = r; c->g = g; c->b = b;			\
    set##name(c);					\
  } \
\
  void							\
  SVGStylable::set##name(const char* colorname)	\
{							\
	css::Color * c;		\
    const unicode::String* cname = unicode::String::internString(colorname);\
			getRGBColor(cname, &c); \
			set##name(c);					\
}


  ATTRDEF(const_PaintType,Color,"color")
  ATTRDEF(const_PaintType,Fill,"fill")
  ATTRDEF(const_PaintType,Stroke,"stroke")
  ATTRDEF(const_PaintType,StopColor,"stop-color")
#undef ATTRDEF

  void
  SVGStylable::updateChildrenStyle()
  {
    css::CSSStyleDeclaration& styleDecl = const_cast<css::CSSStyleDeclaration&>(GET_SIMPLE_VAL(Style));
    SVGElement * asElement = dynamic_cast<SVGElement*>(this);
    if(asElement) {
      dom::Node *domchild = asElement->getFirstChild();
      SVGStylable *child = dynamic_cast<SVGStylable*>(domchild);
      while (domchild) {
	if(child) {
	  //std::cerr << (void*) const_cast<css::CSSStyleDeclaration&>(child->GET_SIMPLE_VAL(Style)).getParent() << __FL__;
	  const_cast<css::CSSStyleDeclaration&>(child->GET_SIMPLE_VAL(Style)).setParent(&styleDecl);
	  //std::cerr << (void*) const_cast<css::CSSStyleDeclaration&>(child->GET_SIMPLE_VAL(Style)).getParent() << __FL__;
	}
	domchild = domchild->getNextSibling();
	child = dynamic_cast<SVGStylable*>(domchild);
      }
    }

  }



#define ATTRDEF(type, name, dtdstr)							\
  void											\
  SVGStylable::set##name(svg::SVGGradientElement*)					\
  {											\
    /* don't forget to create an id for the gradient if it doesn't have one		\
       to be able to dump an svg file...						\
    */											\
  }											\
											\
  void											\
  SVGStylable::set##name(svg::SVGPatternElement*)					\
  {											\
    /* don't forget to create an id for the gradient if it doesn't have one		\
       to be able to dump an svg file...						\
    */											\
  }

ATTRDEF(const_PaintType,Color,"color")
ATTRDEF(const_PaintType,Fill,"fill")
ATTRDEF(const_PaintType,Stroke,"stroke")
#undef ATTRDEF



}

