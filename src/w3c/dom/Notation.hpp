#ifndef _dom_Notation_hpp_
#define _dom_Notation_hpp_
/*
 * Copyright (c) 2000 World Wide Web Consortium,
 * (Massachusetts Institute of Technology, Institut National de
 * Recherche en Informatique et en Automatique, Keio University). All
 * Rights Reserved. This program is distributed under the W3C's Software
 * Intellectual Property License. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 * See W3C License http://www.w3.org/Consortium/Legal/ for more details.
 */

#include <w3c/dom/Node.hpp>
namespace dom {

/**
 * This interface represents a notation declared in the DTD. A notation either 
 * declares, by name, the format of an unparsed entity (see section 4.7 of 
 * the XML 1.0 specification ), or is used for formal declaration of 
 * processing instruction targets (see section 2.6 of the XML 1.0 
 * specification ). The <code>nodeName</code> attribute inherited from 
 * <code>Node</code> is set to the declared name of the notation.
 * <p>The DOM Level 1 does not support editing <code>Notation</code> nodes; 
 * they are therefore readonly.
 * <p>A <code>Notation</code> node does not have any parent.
 * <p>See also the <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>Document Object Model (DOM) Level 2 Core Specification</a>.
 */
class Notation : public Node {
public:
    			Notation(Document * ownerNode);
    			Notation(const Notation& other);
    /**
     * The public identifier of this notation. If the public identifier was 
     * not specified, this is <code>null</code>.
     */
    virtual String * getPublicId() const = 0;

    /**
     * The system identifier of this notation. If the system identifier was 
     * not specified, this is <code>null</code>.
     */
    virtual String * getSystemId() const = 0;
    virtual NodeType	getNodeType() const;
    virtual Node *	cloneNode(bool deep) const;
protected:
    String *		publicId;
    String *		systemId;
};
}
#endif
