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
#ifndef __svgl_ExternalEntityManager__
#define __svgl_ExternalEntityManager__

#include <svgl/config.hpp>
#include <w3c/dom/string.hpp>

#include <map>

namespace dom {
  class Element;
  class Document;
}

namespace svgl {
  class ExternalEntityManager {
  public:
    dom::string getFileName(const dom::string& ref, dom::Document*);

#if 0
    void setCurrentDocumentName(const dom::string&);
    dom::string getCurrentDocumentName();
#endif

    void
    register_(dom::Document*, const dom::string& name);

    /*
    dom::string
    register(dom::Document*); // assign an unique name
    */

    dom::Element*
    get(const dom::string& url, dom::Document * current);

    dom::Document*
    get(const dom::string& name);

    dom::string
    getDocumentName(dom::Document*) const;

    void
    addUsedBy(dom::Element* used, dom::Element* user);

    void
    removeUsedBy(dom::Element* used, dom::Element* user);

    typedef std::multimap<dom::Element*, dom::Element*> UsedBy;

    std::pair<UsedBy::iterator, UsedBy::iterator>
    getUsers(dom::Element* used);
    
    static ExternalEntityManager* getDefault();
    static void setDefault(ExternalEntityManager*);

  private:
    //dom::string _documentName;
    //dom::string _directoryName;

    typedef std::map<dom::string, dom::Document*, string_less> Documents;
    Documents _documents;

    struct DocumentInfo {
      dom::string name;
      dom::string directoryName;
    };

    typedef std::map<dom::Document*, DocumentInfo> DocumentInfos;
    DocumentInfos _documentInfos;


    UsedBy _usedBy;

    static ExternalEntityManager* _default;

  };
}

#endif // __svgl_ExternalEntityManager__

