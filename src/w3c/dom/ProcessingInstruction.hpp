#ifndef _dom_ProcessingInstruction_hpp_
#define _dom_ProcessingInstruction_hpp_
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

#include <w3c/dom/CharacterData.hpp>

namespace dom {

/**
 * The <code>ProcessingInstruction</code> interface represents a "processing 
 * instruction", used in XML as a way to keep processor-specific information 
 * in the text of the document.
 * <p>See also the <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>Document Object Model (DOM) Level 2 Core Specification</a>.
 */
class ProcessingInstruction : public CharacterData {
public:
  			ProcessingInstruction(Document * ownerDoc,
					      String * target, String * data);
  			ProcessingInstruction(const ProcessingInstruction& other);
    /**
     * The target of this processing instruction. XML defines this as being 
     * the first token following the markup that begins the processing 
     * instruction.
     */
    virtual String * getTarget() const;

    /**
     * The content of this processing instruction. This is from the first non 
     * white space character after the target to the character immediately 
     * preceding the <code>?&gt;</code>.
     * @exception DOMException
     *   NO_MODIFICATION_ALLOWED_ERR: Raised when the node is readonly.
     */
    virtual String * getData() const throw (dom::DOMException);
    virtual void setData(String * data)
                           throw(DOMException);

    virtual NodeType	getNodeType() const;
    virtual Node *	cloneNode(bool deep) const;
    virtual String *	getNodeName() const;
protected:
    String *		target;
};
}
#endif
