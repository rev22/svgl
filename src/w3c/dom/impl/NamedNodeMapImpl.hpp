#ifndef _dom_NamedNodeMapImpl_hpp_
#define _dom_NamedNodeMapImpl_hpp_

#include <map>
#if SVGL_USE_GC
#include <utils/gc_alloc.hpp>
#endif
#include <w3c/dom/DOMException.hpp>
#include <utils/String.hpp>
#include <w3c/dom/NamedNodeMap.hpp>
#include <w3c/dom/Node.hpp>
#include <w3c/dom/impl/NodeListImpl.hpp>

namespace dom {

class Document;

class NamedNodeMapImpl : virtual public NamedNodeMap
#if SVGL_USE_GC
			 ,virtual public gc
#endif
{
public:
#if SVGL_USE_GC
  typedef std::multimap<String*, Node*, string_less, ::gc_alloc<std::pair<const String*,Node*> > >
  			Map;
#else
  typedef std::multimap<String*, Node*, string_less>
  			Map;
#endif
  typedef Map::iterator	iterator;
  typedef Map::const_iterator const_iterator;
protected:
  int			flags;
  enum {
    READONLY		= 1,
    CHANGED 		= 2,
    HASDEFAULTS		= 4
  };
  Node *		ownerNode;
  Map			map;

  bool			isReadOnly() const { return (flags & READONLY) != 0; }
  void			isReadOnly(bool value) {
    flags = (value ? flags | READONLY : flags & ~READONLY);
  }
  bool			changed() const { return (flags & CHANGED) != 0; }
  void			changed(bool value) {
    flags = (value ? flags | CHANGED : flags & ~CHANGED);
  }
  bool			hasDefaults() const { return (flags & HASDEFAULTS) != 0; }
  void			hasDefaults(bool value) {
    flags = (short) (value ? flags | HASDEFAULTS : flags & ~HASDEFAULTS);
  }
  iterator		find(String * name);
  const_iterator	find(String * name) const;
  iterator		find(String * localName, String * namespaceURI);
  const_iterator	find(String * localName, String * namespaceURI) const;
  
public:
  			NamedNodeMapImpl(Node * ownerNode);
  			NamedNodeMapImpl(const NamedNodeMapImpl& other);
  virtual		~NamedNodeMapImpl();
  virtual Node *	getNamedItem(String * name) const;
  virtual Node *	setNamedItem(Node * arg)
    throw(DOMException);
  virtual Node *	removeNamedItem(String * name)
    throw(DOMException);
  virtual unsigned int	getLength() const;
  virtual Node *	item(unsigned int index) const;
  virtual Node *	getNamedItemNS(String * namespaceURI, 
				       String * localName) const;
  virtual Node *	setNamedItemNS(Node * arg)
    throw(DOMException);
  virtual Node *	removeNamedItemNS(String * namespaceURI, 
					  String * localName)
    throw(DOMException);
  virtual NamedNodeMapImpl * cloneMap(Node * ownerNode) const;
  virtual void		cloneContent(const NamedNodeMapImpl * srcmap);
  virtual void		setReadOnly(bool readOnly, bool deep);
  virtual bool		getReadOnly() const;
  virtual void		setOwnerDocument(Document * doc);
  iterator		begin() { return map.begin();}
  const_iterator	begin() const { return map.begin(); }
  iterator		end() { return map.end();}
  const_iterator	end() const { return map.end(); }
  static Node *&	it(const iterator& i) { return i->second; }
  static Node *		it(const const_iterator& i) { return i->second; }
  Node *		set(Node * arg) {
    if (arg == 0)
      throw DOMException(DOMException::INVALID_MODIFICATION_ERR);
    String * name = arg->getNodeName();
    iterator i = find(name);
    if (i == end()) {
      map.insert(Map::value_type(name, arg));
      return 0;
    }
    Node * ret = it(i);
    i->second = arg;
    return ret;
  }
};

}

#endif
