#ifndef _dom_Node_hpp_
#define _dom_Node_hpp_
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

#include <w3c/dom/DOMException.hpp>

#if SVGL_USE_GC
#include <gc_cpp.h>
#endif

namespace dom {

class Attr;
class CDATASection;
class CharacterData;
class Comment;
class DOMImplementation;
class Document;
class DocumentFragment;
class DocumentType;
class Element;
class Entity;
class EntityReference;
class NamedNodeMap;
class NodeList;
class Notation;
class ProcessingInstruction;
class Text;

using unicode::String;

/**
 * The <code>Node</code> interface is the primary datatype for the entire 
 * Document Object Model. It represents a single node in the document tree. 
 * While all objects implementing the <code>Node</code> interface expose 
 * methods for dealing with children, not all objects implementing the 
 * <code>Node</code> interface may have children. For example, 
 * <code>Text</code> nodes may not have children, and adding children to 
 * such nodes results in a <code>DOMException</code> being raised.
 * <p>The attributes <code>nodeName</code>, <code>nodeValue</code> and 
 * <code>attributes</code> are included as a mechanism to get at node 
 * information without casting down to the specific derived interface. In 
 * cases where there is no obvious mapping of these attributes for a 
 * specific <code>nodeType</code> (e.g., <code>nodeValue</code> for an 
 * <code>Element</code> or <code>attributes</code> for a <code>Comment</code>
 * ), this returns <code>null</code>. Note that the specialized interfaces 
 * may contain additional and more convenient mechanisms to get and set the 
 * relevant information.
 * <p>See also the <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>Document Object Model (DOM) Level 2 Core Specification</a>.
 */
class Node
#if SVGL_USE_GC
: virtual public gc
#endif
{
public:
			Node(Document * ownerDocument);
			Node(const Node&);
    virtual		~Node();
    enum NodeType {
    /**
     * The node is an <code>Element</code>.
     */
      ELEMENT_NODE              = 1,
    /**
     * The node is an <code>Attr</code>.
     */
      ATTRIBUTE_NODE            = 2,
    /**
     * The node is a <code>Text</code> node.
     */
      TEXT_NODE                 = 3,
    /**
     * The node is a <code>CDATASection</code>.
     */
      CDATA_SECTION_NODE        = 4,
    /**
     * The node is an <code>EntityReference</code>.
     */
      ENTITY_REFERENCE_NODE     = 5,
    /**
     * The node is an <code>Entity</code>.
     */
      ENTITY_NODE               = 6,
    /**
     * The node is a <code>ProcessingInstruction</code>.
     */
      PROCESSING_INSTRUCTION_NODE = 7,
    /**
     * The node is a <code>Comment</code>.
     */
      COMMENT_NODE              = 8,
    /**
     * The node is a <code>Document</code>.
     */
      DOCUMENT_NODE             = 9,
    /**
     * The node is a <code>DocumentType</code>.
     */
      DOCUMENT_TYPE_NODE        = 10,
    /**
     * The node is a <code>DocumentFragment</code>.
     */
      DOCUMENT_FRAGMENT_NODE    = 11,
    /**
     * The node is a <code>Notation</code>.
     */
      NOTATION_NODE             = 12,
    };

    /**
     * The name of this node, depending on its type; see the table above. 
     */
    virtual String * getNodeName() const = 0;

    /**
     * The value of this node, depending on its type; see the table above. 
     * When it is defined to be <code>null</code>, setting it has no effect.
     * @exception DOMException
     *   NO_MODIFICATION_ALLOWED_ERR: Raised when the node is readonly.
     * @exception DOMException
     *   DOMSTRING_SIZE_ERR: Raised when it would return more characters than 
     *   fit in a <code>String</code> variable on the implementation 
     *   platform.
     */
    virtual String * getNodeValue() const
                                  throw(DOMException);
    virtual void setNodeValue(String * nodeValue)
                                  throw(DOMException);

    /**
     * A code representing the type of the underlying object, as defined above.
     */
    virtual NodeType getNodeType() const = 0;

    /**
     * The parent of this node. All nodes, except <code>Attr</code>, 
     * <code>Document</code>, <code>DocumentFragment</code>, 
     * <code>Entity</code>, and <code>Notation</code> may have a parent. 
     * However, if a node has just been created and not yet added to the 
     * tree, or if it has been removed from the tree, this is 
     * <code>null</code>.
     */
    virtual Node * getParentNode() const;

    /**
     * A <code>NodeList</code> that contains all children of this node. If 
     * there are no children, this is a <code>NodeList</code> containing no 
     * nodes.
     */
    virtual const NodeList * getChildNodes() const;

    /**
     * The first child of this node. If there is no such node, this returns 
     * <code>null</code>.
     */
    virtual Node * getFirstChild() const;

    /**
     * The last child of this node. If there is no such node, this returns 
     * <code>null</code>.
     */
    virtual Node * getLastChild() const;

    /**
     * The node immediately preceding this node. If there is no such node, 
     * this returns <code>null</code>.
     */
    virtual Node * getPreviousSibling() const;

    /**
     * The node immediately following this node. If there is no such node, 
     * this returns <code>null</code>.
     */
    virtual Node * getNextSibling() const;

    /**
     * A <code>NamedNodeMap</code> containing the attributes of this node (if 
     * it is an <code>Element</code>) or <code>null</code> otherwise. 
     */
    virtual NamedNodeMap * getAttributes() const;

    /**
     * The <code>Document</code> object associated with this node. This is 
     * also the <code>Document</code> object used to create new nodes. When 
     * this node is a <code>Document</code> or a <code>DocumentType</code> 
     * which is not used with any <code>Document</code> yet, this is 
     * <code>null</code>.
     * @version DOM Level 2
     */
    virtual Document * getOwnerDocument() const;

    /**
     * Inserts the node <code>newChild</code> before the existing child node 
     * <code>refChild</code>. If <code>refChild</code> is <code>null</code>, 
     * insert <code>newChild</code> at the end of the list of children.
     * <br>If <code>newChild</code> is a <code>DocumentFragment</code> object, 
     * all of its children are inserted, in the same order, before 
     * <code>refChild</code>. If the <code>newChild</code> is already in the 
     * tree, it is first removed.
     * @param newChildThe node to insert.
     * @param refChildThe reference node, i.e., the node before which the new 
     *   node must be inserted.
     * @return The node being inserted.
     * @exception DOMException
     *   HIERARCHY_REQUEST_ERR: Raised if this node is of a type that does not 
     *   allow children of the type of the <code>newChild</code> node, or if 
     *   the node to insert is one of this node's ancestors.
     *   <br>WRONG_DOCUMENT_ERR: Raised if <code>newChild</code> was created 
     *   from a different document than the one that created this node.
     *   <br>NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly or 
     *   if the parent of the node being inserted is readonly.
     *   <br>NOT_FOUND_ERR: Raised if <code>refChild</code> is not a child of 
     *   this node.
     */
    virtual Node * insertBefore(Node * newChild, 
				Node * refChild)
                              throw(DOMException);

    /**
     * Replaces the child node <code>oldChild</code> with <code>newChild</code>
     *  in the list of children, and returns the <code>oldChild</code> node.
     * <br>If <code>newChild</code> is a <code>DocumentFragment</code> object, 
     * <code>oldChild</code> is replaced by all of the 
     * <code>DocumentFragment</code> children, which are inserted in the 
     * same order. If the <code>newChild</code> is already in the tree, it 
     * is first removed.
     * @param newChildThe new node to put in the child list.
     * @param oldChildThe node being replaced in the list.
     * @return The node replaced.
     * @exception DOMException
     *   HIERARCHY_REQUEST_ERR: Raised if this node is of a type that does not 
     *   allow children of the type of the <code>newChild</code> node, or if 
     *   the node to put in is one of this node's ancestors.
     *   <br>WRONG_DOCUMENT_ERR: Raised if <code>newChild</code> was created 
     *   from a different document than the one that created this node.
     *   <br>NO_MODIFICATION_ALLOWED_ERR: Raised if this node or the parent of 
     *   the new node is readonly.
     *   <br>NOT_FOUND_ERR: Raised if <code>oldChild</code> is not a child of 
     *   this node.
     */
    virtual Node * replaceChild(Node * newChild, 
                             Node * oldChild)
                              throw(DOMException);

    /**
     * Removes the child node indicated by <code>oldChild</code> from the list 
     * of children, and returns it.
     * @param oldChildThe node being removed.
     * @return The node removed.
     * @exception DOMException
     *   NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
     *   <br>NOT_FOUND_ERR: Raised if <code>oldChild</code> is not a child of 
     *   this node.
     */
    virtual Node * removeChild(Node * oldChild)
                             throw(DOMException);

    /**
     * Adds the node <code>newChild</code> to the end of the list of children 
     * of this node. If the <code>newChild</code> is already in the tree, it 
     * is first removed.
     * @param newChildThe node to add.If it is a <code>DocumentFragment</code>
     *    object, the entire contents of the document fragment are moved 
     *   into the child list of this node
     * @return The node added.
     * @exception DOMException
     *   HIERARCHY_REQUEST_ERR: Raised if this node is of a type that does not 
     *   allow children of the type of the <code>newChild</code> node, or if 
     *   the node to append is one of this node's ancestors.
     *   <br>WRONG_DOCUMENT_ERR: Raised if <code>newChild</code> was created 
     *   from a different document than the one that created this node.
     *   <br>NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
     */
    virtual Node * appendChild(Node * newChild)
                             throw(DOMException);

    /**
     * Returns whether this node has any children.
     * @return  <code>true</code> if this node has any children, 
     *   <code>false</code> otherwise.
     */
    virtual bool hasChildNodes() const;

    /**
     * Returns a duplicate of this node, i.e., serves as a generic copy 
     * constructor for nodes. The duplicate node has no parent; (
     * <code>parentNode</code> is <code>null</code>.).
     * <br>Cloning an <code>Element</code> copies all attributes and their 
     * values, including those generated by the XML processor to represent 
     * defaulted attributes, but this method does not copy any text it 
     * contains unless it is a deep clone, since the text is contained in a 
     * child <code>Text</code> node. Cloning an <code>Attribute</code> 
     * directly, as opposed to be cloned as part of an <code>Element</code> 
     * cloning operation, returns a specified attribute (
     * <code>specified</code> is <code>true</code>). Cloning any other type 
     * of node simply returns a copy of this node.
     * <br>Note that cloning an immutable subtree results in a mutable copy, 
     * but the children of an <code>EntityReference</code> clone are readonly
     * . In addition, clones of unspecified <code>Attr</code> nodes are 
     * specified. And, cloning <code>Document</code>, 
     * <code>DocumentType</code>, <code>Entity</code>, and 
     * <code>Notation</code> nodes is implementation dependent.
     * @param deepIf <code>true</code>, recursively clone the subtree under 
     *   the specified node; if <code>false</code>, clone only the node 
     *   itself (and its attributes, if it is an <code>Element</code>). 
     * @return The duplicate node.
     */
    virtual Node * cloneNode(bool deep) const = 0;

    /**
     * Puts all <code>Text</code> nodes in the full depth of the sub-tree 
     * underneath this <code>Node</code>, including attribute nodes, into a 
     * "normal" form where only structure (e.g., elements, comments, 
     * processing instructions, CDATA sections, and entity references) 
     * separates <code>Text</code> nodes, i.e., there are neither adjacent 
     * <code>Text</code> nodes nor empty <code>Text</code> nodes. This can 
     * be used to ensure that the DOM view of a document is the same as if 
     * it were saved and re-loaded, and is useful when operations (such as 
     * XPointer  lookups) that depend on a particular document tree 
     * structure are to be used.In cases where the document contains 
     * <code>CDATASections</code>, the normalize operation alone may not be 
     * sufficient, since XPointers do not differentiate between 
     * <code>Text</code> nodes and <code>CDATASection</code> nodes.
     * @version DOM Level 2
     */
    virtual void normalize();

    /**
     * Tests whether the DOM implementation implements a specific feature and 
     * that feature is supported by this node.
     * @param featureThe name of the feature to test. This is the same name 
     *   which can be passed to the method <code>hasFeature</code> on 
     *   <code>DOMImplementation</code>.
     * @param versionThis is the version number of the feature to test. In 
     *   Level 2, version 1, this is the string "2.0". If the version is not 
     *   specified, supporting any version of the feature will cause the 
     *   method to return <code>true</code>.
     * @return Returns <code>true</code> if the specified feature is 
     *   supported on this node, <code>false</code> otherwise.
     * @since DOM Level 2
     */
    virtual bool isSupported(String * feature, 
                               String * version);

    /**
     * The namespace URI of this node, or <code>null</code> if it is 
     * unspecified.
     * <br>This is not a computed value that is the result of a namespace 
     * lookup based on an examination of the namespace declarations in 
     * scope. It is merely the namespace URI given at creation time.
     * <br>For nodes of any type other than <code>ELEMENT_NODE</code> and 
     * <code>ATTRIBUTE_NODE</code> and nodes created with a DOM Level 1 
     * method, such as <code>createElement</code> from the 
     * <code>Document</code> interface, this is always <code>null</code>.Per 
     * the Namespaces in XML Specification  an attribute does not inherit 
     * its namespace from the element it is attached to. If an attribute is 
     * not explicitly given a namespace, it simply has no namespace.
     * @since DOM Level 2
     */
    virtual String * getNamespaceURI() const;

    /**
     * The namespace prefix of this node, or <code>null</code> if it is 
     * unspecified.
     * <br>Note that setting this attribute, when permitted, changes the 
     * <code>nodeName</code> attribute, which holds the qualified name, as 
     * well as the <code>tagName</code> and <code>name</code> attributes of 
     * the <code>Element</code> and <code>Attr</code> interfaces, when 
     * applicable.
     * <br>Note also that changing the prefix of an attribute that is known to 
     * have a default value, does not make a new attribute with the default 
     * value and the original prefix appear, since the 
     * <code>namespaceURI</code> and <code>localName</code> do not change.
     * <br>For nodes of any type other than <code>ELEMENT_NODE</code> and 
     * <code>ATTRIBUTE_NODE</code> and nodes created with a DOM Level 1 
     * method, such as <code>createElement</code> from the 
     * <code>Document</code> interface, this is always <code>null</code>.
     * @exception DOMException
     *   INVALID_CHARACTER_ERR: Raised if the specified prefix contains an 
     *   illegal character.
     *   <br>NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
     *   <br>NAMESPACE_ERR: Raised if the specified <code>prefix</code> is 
     *   malformed, if the <code>namespaceURI</code> of this node is 
     *   <code>null</code>, if the specified prefix is "xml" and the 
     *   <code>namespaceURI</code> of this node is different from "
     *   http://www.w3.org/XML/1998/namespace", if this node is an attribute 
     *   and the specified prefix is "xmlns" and the 
     *   <code>namespaceURI</code> of this node is different from "
     *   http://www.w3.org/2000/xmlns/", or if this node is an attribute and 
     *   the <code>qualifiedName</code> of this node is "xmlns" .
     * @since DOM Level 2
     */
    virtual String * getPrefix() const;
    virtual void setPrefix(String * prefix)
                                throw(DOMException);

    /**
     * Returns the local part of the qualified name of this node.
     * <br>For nodes of any type other than <code>ELEMENT_NODE</code> and 
     * <code>ATTRIBUTE_NODE</code> and nodes created with a DOM Level 1 
     * method, such as <code>createElement</code> from the 
     * <code>Document</code> interface, this is always <code>null</code>.
     * @since DOM Level 2
     */
    virtual String * getLocalName() const;

    /**
     * Returns whether this node (if it is an element) has any attributes.
     * @return <code>true</code> if this node has any attributes, 
     *   <code>false</code> otherwise.
     * @since DOM Level 2
     */
    virtual bool hasAttributes() const;

    virtual void	changed();
    virtual int		changes() const;
protected:
    Node *		ownerNode;
    short		flags;
    enum {
	READONLY	= 1,
	OWNED		= 2,
	FIRSTCHILD	= 4,
	SPECIFIED	= 8,
	SETVALUE	= 16,
	IGNORABLEWS	= 32,
	ID		= 64,
    };
public:
    virtual void	setOwnerDocument(Document * doc);
    bool		isReadOnly() const { return (flags & READONLY) != 0; }
    void		isReadOnly(bool value) {
	flags = (value ? flags | READONLY : flags & ~READONLY);
    }
    bool		isOwned() const { return (flags & OWNED) != 0; }
    void		isOwned(bool value) {
	flags = (value ? flags | OWNED : flags & ~OWNED);
    }
    bool		isFirstChild() const { return (flags & FIRSTCHILD) != 0; }
    void		isFirstChild(bool value) {
	flags = (value ? flags | FIRSTCHILD : flags & ~FIRSTCHILD);
    }
    bool		isSpecified() const { return (flags & SPECIFIED) != 0; }
    void		isSpecified(bool value) {
	flags = (value ? flags | SPECIFIED : flags & ~SPECIFIED);
    }
    bool		setValueCalled() const { return (flags & SETVALUE) != 0; }
    void		setValueCalled(bool value) {
	flags = (value ? flags | SETVALUE : flags & ~SETVALUE);
    }
    bool		internalIsIgnorableWhitespace() const {
	return (flags & IGNORABLEWS) != 0;
    }
    void		isIgnorableWhitespace(bool value) {
	flags =  (value ? flags | IGNORABLEWS : flags & ~IGNORABLEWS);
    }
    bool		isId() const { return (flags & ID) != 0; }
    void		isId(bool value) {
	flags =  (value ? flags | ID : flags & ~ID);
    }

    virtual void	setReadOnly(bool readOnly, bool deep);
    virtual bool	getReadOnly();
};
}
#endif
