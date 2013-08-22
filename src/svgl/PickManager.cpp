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
#include <svgl/PickManager.hpp>
#include <svgl/GLInfo.hpp>

#include <w3c/svg/SVGDocument.hpp>
#include <w3c/svg/SVGSVGElement.hpp>
#include <svgl/ExternalEntityManager.hpp>
#include <svgl/Context.hpp>
#include <w3c/svg/SVGClipPathElement.hpp>
#include <svgl/xmlostream.hpp>

#if defined (__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <algorithm>

#include <svgl/debug.hpp>
#include <svgl/getattr.hpp>
#include <svgl/xmlostream.hpp>

// see docs to know how it works...



namespace svgl {
  
  void
  find_clip(svgl::ExternalEntityManager * entityManager, svg::SVGElement *_element,  std::vector<svg::SVGClipPathElement*>* res)
  {
    for(dom::Node* n = _element; n; n=n->getParentNode()) {
      _element = dynamic_cast<svg::SVGElement*>(n);
      svg::SVGStylable* stylable = dynamic_cast<svg::SVGStylable*>(n);
      if(stylable) {
	svg::SVGClipPathElement *clip = stylable->getSVGClipPathElement( entityManager, _element->getOwnerDocument());
	if(clip) {
	  //STDDBG;
	  res->push_back(clip);
	}
      }
    }
  }

  bool
  isANonClippedStack(ExternalEntityManager * entityManager, GLuint* beg, /*GLuint* end,*/ std::vector<svg::SVGClipPathElement*> * validClipPathes)
  {
    svg::SVGElement ** _element = reinterpret_cast<svg::SVGElement **>(beg);
    //svg::SVGElement ** _end = reinterpret_cast<svg::SVGElement **>(end);
    bool visible=false;
    //    for(; _element!=_end && (!visible); ++_element) {
    //std::cerr << DBGVAR(*_element) << __FL__;
      visible=true;
      std::vector<svg::SVGClipPathElement*> clips;
      find_clip(entityManager, *_element, &clips);
      for(std::vector<svg::SVGClipPathElement*>::iterator i = clips.begin();
	  i!=clips.end();
	  ++i) {
	visible = (std::find(validClipPathes->begin(), validClipPathes->end(), *i)!=validClipPathes->end());
	//std::cerr << DBGVAR(*i) << __FL__;
	if(!visible)
	  break;
      }
      //}
      //std::cerr << DBGVAR(visible) << __FL__;
    return visible;
  }

  PickManager::HitElementIterator::HitElementIterator(pointer e)
    : _element(e)
  {
  }

  PickManager::HitElementIterator&
  PickManager::HitElementIterator::operator++()
  {
    ++_element;
    return *this;
  }

  PickManager::HitElementIterator&
  PickManager::HitElementIterator::operator--()
  {
    --_element;
    return *this;
  }


  /*
    it seems that there is a bug when picking
    even if any name is loaded or pushed, we have a hit record
    containing no elements (size==0)
  */

  bool
  isAValidPickStack(ExternalEntityManager * entityManager, GLuint *buf,  std::vector<svg::SVGClipPathElement*> * validClipPathes)
  {
    bool res=false;
    if(!*buf || ( (*buf)==1 && !*(buf+3)) ) {
      //std::cerr << "==================" << DBGVAR(*buf) << DBGVAR( *(buf+3) )  << __FL__;
      res = false;
    }
    else {
      GLuint *b=buf+4; // begins with a zero
      unsigned int i=0, len = *buf-1;
      //std::cerr << DBGVAR(buf[3]) << __FL__;
      for(; i<len;++i) {
	//std::cerr << DBGVAR(i) << DBGVAR(&b[i]) << DBGVAR(b[i]) << __FL__;
	if(!b[i]) {
	  //std::cerr << DBGVAR(i) << DBGVAR(b[i]) << DBGVAR(b[i+1]) << DBGVAR(len) << std::endl;
	  //std::cerr << "it's a clip !!" __FL__;
	  // b[i]->0
	  // b[i+1] -> clip
	  // b[i+2] -> maybe a hit from clippath
	  svg::SVGClipPathElement* clip = reinterpret_cast<svg::SVGClipPathElement*>(b[i+1]);
	  if((i+2)<len) {
	    //std::cerr << "clip path has triggered a hit" << std::endl;
	    //xmlostream out(std::cerr);
	    //reinterpret_cast<svg::SVGElement*>(b[i+2])->print(out);

	    validClipPathes->push_back(clip);
	  } else {
	    //std::cerr << "clip path has NOT triggered a hit" << std::endl;
	    std::vector<svg::SVGClipPathElement*>::iterator pos = std::find(validClipPathes->begin(), validClipPathes->end(), clip);
	    if(pos!=validClipPathes->end())
	      validClipPathes->erase(pos);
	  }
	  break;
	}
      }
      if(i==len) {
	//std::cerr << "maybe a hit" << __FL__;
	res = isANonClippedStack(entityManager, b+len-1, validClipPathes);
      }
    }
    //std::cerr << DBGVAR(res) << __FL__;
    return res;
  }


  PickManager::HitIterator::HitIterator(PickStackPositionIterator p)
    : _position(p)
  {
  }

    
  PickManager::HitIterator&
  PickManager::HitIterator::operator++()
  {
    ++_position;
    return *this;
  }

  PickManager::HitIterator&
  PickManager::HitIterator::operator--()
  {
    --_position;
    return *this;
  }

  PickManager::HitIterator::value_type
  PickManager::HitIterator::operator*()
  {
    value_type res;
    GLuint * _hitStart = *_position;
    GLuint * start=_hitStart+4; // it should always start with a zero...
    //std::cerr << DBGVAR(_hitStart) << DBGVAR(start) << __FL__;
    GLuint * finish = start+*_hitStart-1;
    HitElementIterator::pointer beg = reinterpret_cast<HitElementIterator::pointer>(start);
    HitElementIterator::pointer end = reinterpret_cast<HitElementIterator::pointer>(finish);
    res.first = PickManager::HitElementIterator(beg);
    res.second = PickManager::HitElementIterator(end);
    return res;
  }

  PickManager::ReverseHitIterator::ReverseHitIterator(PickStackPositionReverseIterator p)
    : _position(p)
  {
  }

  PickManager::ReverseHitIterator&
  PickManager::ReverseHitIterator::operator++()
  {
    ++_position;
    return *this;
  }

  PickManager::ReverseHitIterator&
  PickManager::ReverseHitIterator::operator--()
  {
    --_position;
    return *this;
  }

  PickManager::ReverseHitIterator::value_type
  PickManager::ReverseHitIterator::operator*()
  {
    value_type res;
    GLuint * _hitStart = *_position;
    GLuint * start=_hitStart+4; // it should always start with a zero...
    //std::cerr << DBGVAR(_hitStart) << DBGVAR(start) << __FL__;
    GLuint * finish = start+*_hitStart-1;
    HitElementIterator::pointer beg = reinterpret_cast<HitElementIterator::pointer>(start);
    HitElementIterator::pointer end = reinterpret_cast<HitElementIterator::pointer>(finish);
    res.first = PickManager::HitElementIterator(beg);
    res.second = PickManager::HitElementIterator(end);
    return res;
  }

  PickManager::PickManager()
    : _bufsize(1024), _selectBuf(new GLuint[_bufsize]), context(0), glinfo(0)
  {
  }

  PickManager::PickManager(Context* svglcontext, GLInfo * info)
    : _bufsize(1024), _selectBuf(new GLuint[_bufsize]), context(svglcontext), glinfo(info)
  {
  }

  PickManager::~PickManager()
  {
    delete [] _selectBuf;
  }

  PickManager::PickResult
  PickManager::pick(svg::SVGDocument* doc, double x, double y, double width, double height)
  {
    if (!doc || !doc->getRootElement())
      throw;

    return pick(const_cast<svg::SVGSVGElement*>(doc->getRootElement()), context, glinfo, x, y, width, height);    
  }

  PickManager::PickResult
  PickManager::pick(svg::SVGSVGElement* svgElement, Context* svglContext, GLInfo* glinfo, double x, double y, double width, double height)
  {
    glSelectBuffer(_bufsize, _selectBuf);
    glRenderMode(GL_SELECT);
  
    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int vpw=viewport[2];
    int vph=viewport[3];
    // std::cerr << DBGVAR(viewport[0]) << DBGVAR(viewport[1]) << DBGVAR(viewport[2]) << DBGVAR(viewport[3]) << __FL__;

    gluPickMatrix(x,vph-y,width,height, viewport);
    glOrtho(0,vpw,vph,0,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(glinfo->zoom, glinfo->zoom, 1);
    glTranslatef(-glinfo->xpan+.3, -glinfo->ypan+.3, 0);

#if 0
    int widthSave = glinfo->winWidth;
    int heightSave = glinfo->winHeight;
    glinfo->winWidth = vpw;
    glinfo->winHeight = vph;
    glinfo->setZoomAndPan();
    glinfo->winWidth = widthSave;
    glinfo->winHeight = heightSave;
#endif

    glinfo->doPick=svgl::GLInfo::pick_this;
    svgElement->pickTraverse(svglContext, glinfo);
    glinfo->doPick=svgl::GLInfo::pick_no;

    GLint hits = glRenderMode(GL_RENDER);
    //std::cerr << DBGVAR(hits) << __FL__;
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);



    int err = glGetError();
    if(err)
      std::cerr << "GL error: " << gluErrorString(err) << __FL__;

#if 0
    {
      //      xmlostream out(std::cerr);
      GLuint names, *ptr;
      ptr = _selectBuf;
      for(int i=0;i<hits;++i) {
	names = *ptr;
	std::cerr << "number of elements for hit " << i << ": " << names << std::endl;
	++ptr;
	++ptr;
	++ptr;
	std::cerr << "elements: " << std::endl;
	for(int j=0; j<(int)names; ++j) {
	  std::cerr << *ptr << " ";
	  if(*ptr) {
	    svg::SVGElement *elem = reinterpret_cast<svg::SVGElement*>(*ptr);
	    //elem->print(out);
	    std::cerr << "<" << elem->getDtdElement() << ">";
	  }
	  ++ptr;
	}
	std::cerr << std::endl;
      }

    }
#endif

    _validPickStacks.clear();
    std::vector<svg::SVGClipPathElement*> validClipPathes;

    {
      GLuint names, *ptr;
      ptr = _selectBuf;
      for(int i=0;i<hits;++i) {
	names = *ptr;
	//std::cerr << DBGVAR(names) << __FL__;
	if(isAValidPickStack(svglContext->externalEntityManager, ptr, &validClipPathes)) {
	  _validPickStacks.push_back(ptr);
	}
	ptr+=3+names;
      }
    }

    std::pair<HitIterator, HitIterator> res;
    HitIterator &begin=res.first, &end=res.second;
    begin = HitIterator(_validPickStacks.begin());
    end = HitIterator(_validPickStacks.end());

    return res;
  }

  PickManager::ReversePickResult
  PickManager::reversePick()
  {
    ReversePickResult res;
    res.first = ReverseHitIterator(_validPickStacks.rbegin());
    res.second = ReverseHitIterator(_validPickStacks.rend());
    return res;
  }

}
