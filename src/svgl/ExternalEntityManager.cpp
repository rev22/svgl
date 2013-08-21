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


#include <svgl/ExternalEntityManager.hpp>
#include <svgl/Parser.hpp>

#include <w3c/dom/Element.hpp>
#include <w3c/dom/Document.hpp>
#include <w3c/svg/SVGDocument.hpp>

#include <svgl/debug.hpp>

namespace svgl {

  ExternalEntityManager* ExternalEntityManager::_default = 0;

  void ExternalEntityManager::setDefault(ExternalEntityManager* m) { _default = m; }
  ExternalEntityManager* ExternalEntityManager::getDefault() { return _default; }


  dom::string
  ExternalEntityManager::getDocumentName(dom::Document* doc) const
  {
    for(Documents::const_iterator p = _documents.begin(); p != _documents.end(); ++p) {
      if(p->second==doc)
	return p->first;
    }
    return 0;
  }

#if 0
  void
  ExternalEntityManager::setCurrentDocumentName(const dom::string& docName)
  {
    _documentName = docName;
    static const unicode::String::char_type dirsep = '/';
    int pos = docName->getLength()-1;
    _directoryName = unicode::String::internString("");
    if(pos<0)
      return;
    if((*_documentName)[pos]==dirsep) {
      std::cerr << "suspect document name " << docName << __FL__;
      _directoryName = _documentName;
      return;
    }
    --pos;
    while(pos!=0) {
      if((*_documentName)[pos]==dirsep) {
	_directoryName = _documentName->substring(0, pos+1);
	break;
      }
      --pos;
    }
  }

  dom::string
  ExternalEntityManager::getCurrentDocumentName()
  {
    return _documentName;
  }
#endif

  dom::string
  ExternalEntityManager::getFileName(const dom::string& ref, dom::Document* doc)
  {
    static const unicode::String::char_type dirsep = '/';
    if (ref->getLength()) {
      if ((*ref)[0]==dirsep) {
	return ref;
      } else {
	//std::cerr << _directoryName << __FL__;
	DocumentInfo infoDoc = _documentInfos[doc];
	return concat(infoDoc.directoryName,ref);
      }
    }
    return ref;
  }

  void
  ExternalEntityManager::register_(dom::Document* doc, const dom::string& name)
  {
    _documents[name] = doc;

    static const unicode::String::char_type dirsep = '/';
    int pos = name->getLength()-1;
    dom::string _directoryName = unicode::String::internString("");
    if(pos<0)
      return;
    if((*name)[pos]==dirsep) {
      std::cerr << "suspect document name " << name << __FL__;
      _directoryName = name;
      return;
    }
    --pos;
    while(pos!=0) {
      if((*name)[pos]==dirsep) {
	_directoryName = name->substring(0, pos+1);
	break;
      }
      --pos;
    }


    DocumentInfo docInfo;
    docInfo.name = name;
    docInfo.directoryName = _directoryName;

    _documentInfos[doc] = docInfo;

    //std::cerr << "registered " << name << __FL__;
  }

  /*
  dom::string
  ExternalEntityManager::register(dom::Document* doc)
  {
  }
  */

  dom::Element*
  ExternalEntityManager::get(const dom::string& url, dom::Document * current)
  {
    static const unicode::String::char_type sharp='#';

    dom::Element * res = 0;
    unicode::String::size_type sharpPos = url->indexOf(sharp);

    dom::Document * docToSearch = current;
    if( url->getLength() && (sharpPos!=0) ) {
      dom::string docName;
      if((sharpPos!=unicode::String::npos))
	docName = url->substring(0, sharpPos);
      else
	docName = url;
      docToSearch = get(docName);
      if(!docToSearch) {
	//std::cerr << "ExternalEntityManager::get couldn't get document " << docName << __FL__;
	return 0;
      }
    }
    
    if( (sharpPos!=unicode::String::npos) && (sharpPos!=(url->getSize()-1))) {
      dom::string refName = url->substring(sharpPos+1);
      if(refName) {
	//std::cerr << refName << __FL__;
	res = docToSearch->getElementById(refName);
      }
      else {
	std::cerr << "ExternalEntityManager::get couldn't get element #" << refName << __FL__;
	return 0;
      }
    }
    else {
      res = docToSearch->getDocumentElement();
      //std::cerr << res->getTagName() << __FL__;
    }

    return res;
  }

  dom::Document*
  ExternalEntityManager::get(const dom::string& docName)
  {
    dom::Document* docToSearch = _documents[docName];
    if(!docToSearch) {
      svgl::Parser parser;
      svg::SVGDocument * doc=0;
      try {
	doc = parser.parse(docName->transcode());
      }
      catch (const svgl::Parser::exception& e) {
	std::cerr << e.what() << __FL__;
      }
      catch (std::exception& e) {
	std::cerr << e.what() << __FL__;
      }
      catch (...) {
	std::cerr << "unknown exception" << __FL__;
      }
      
      if(!doc) {
	std::cerr << "couldn't load document " << docName << __FL__;
	return 0;
      }
      register_(doc, docName);
      docToSearch = doc;
    }
    return docToSearch;
  }

  void
  ExternalEntityManager::addUsedBy(dom::Element* used, dom::Element* user)
  {
    _usedBy.insert(std::pair<dom::Element*,dom::Element*>(used,user));
  }

  void
  ExternalEntityManager::removeUsedBy(dom::Element* used, dom::Element* user)
  {
    //_usedBy.erase(std::pair<dom::Element*,dom::Element*>(used,user));
    std::pair<UsedBy::iterator, UsedBy::iterator> p = _usedBy.equal_range(used);
    for(; p.first != p.second; ++(p.first)) {
      if(p.first->second==used) {
	_usedBy.erase(p.first);
      }
    }
  }

  std::pair<ExternalEntityManager::UsedBy::iterator, ExternalEntityManager::UsedBy::iterator>
  ExternalEntityManager::getUsers(dom::Element* used)
  {
    std::pair<UsedBy::iterator, UsedBy::iterator> p = _usedBy.equal_range(used);
    return p;
  }


}
