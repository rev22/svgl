#ifndef _dom_NodeListImpl_hpp_
#define _dom_NodeListImpl_hpp_

#include <utils/List.hpp>
#include <w3c/dom/NodeList.hpp>

#if SVGL_USE_GC
#include <utils/gc_alloc.hpp>
#endif

namespace dom {

class NodeListImpl : virtual public NodeList,
		     public List<Node*>
{
public:
#if SVGL_USE_GC
  typedef std::vector<Node*, gc_alloc<Node*> > super;
#else
  typedef std::vector<Node*> super;
#endif
  NodeListImpl();
  NodeListImpl(const NodeList&);

  void			append(const NodeList * nl);
  void			append(Node * node);
  void			insert(unsigned int index, Node * node);
  void			remove(unsigned int index);
};

class EmptyNodeList : public NodeList
{
protected:
  static EmptyNodeList * singleton;
  EmptyNodeList();
public:
  virtual unsigned int	getLength() const;
  virtual Node *	item(unsigned int index) const;
  static EmptyNodeList * instance();
};

}


#endif
