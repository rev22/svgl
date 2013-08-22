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
#ifndef __xmlostream__
#define __xmlostream__


//#include <ostream>
#include <iostream> // because of gcc 2.96
#include <string>


class xmlostream {
public:
  xmlostream(std::ostream& out) : _out(out), _tabs(), _print_tabs(false) {}

  template <typename X>
  void print(const X& x) {
    if(_print_tabs) {
      _out << _tabs;
      _print_tabs=false;
    }
    _out << x;
  }

  void endl() { _out << std::endl; _print_tabs=true; }

  void setreltabs(int t) {
    if (t>0) {
      _tabs.append(t, '\t');
    }
    else if (t<0) {
      std::string::size_type s = _tabs.size();
      unsigned int ut = -t;
      if (ut <= s) {
	_tabs.erase(s+t);
      }
    }
  }

  void setabstabs(int t) {
    _tabs.erase();
    _tabs.append(t,'\t');
  }

  //operator std::ostream&() { return _out; }

private:
  std::ostream& _out;
  std::string _tabs;
  bool _print_tabs;
};

template <typename X>
inline
xmlostream&
operator<<(xmlostream& out, const X& x)
{
  out.print(x);
  return out;
}

inline
xmlostream&
operator<<(xmlostream& out, xmlostream& (*manip)(xmlostream&) )
{
  return manip(out);
}

inline
xmlostream&
endl(xmlostream& out)
{
  out.endl();
  return out;
}

// xmlostream

template <typename ARGTYPE>
class xmlomanip {
public:
  typedef xmlostream& (*fptr_type)(xmlostream&, ARGTYPE);
  xmlomanip(fptr_type fptr, ARGTYPE arg) : _fptr(fptr), _arg(arg) {}
  xmlostream& operator()(xmlostream& out) const { return _fptr(out,_arg); }
private:
  fptr_type _fptr;
  ARGTYPE _arg;
};


template <typename ARGTYPE>
inline
xmlostream&
operator<<(xmlostream& out, const xmlomanip<ARGTYPE>& manip)
{
  manip(out);
  return out;
}

inline
xmlostream&
setabstabs(xmlostream& out, int t)
{
  out.setabstabs(t);
  return out;
}

inline
xmlomanip<int>
abstabs(int t)
{
  return xmlomanip<int>(&setabstabs, t);
}

inline
xmlostream&
setreltabs(xmlostream& out, int t)
{
  out.setreltabs(t);
  return out;
}

inline
xmlomanip<int>
reltabs(int t)
{
  return xmlomanip<int>(&setreltabs, t);
}


#if 0
// ostream

std::ostream&
setabstabs(std::ostream& out, int t)
{
  return out;
}

std::omanip<int>
abstabs(int t)
{
  return omanip<int>(&settabs, t);
}

std::ostream&
setreltabs(std::ostream& out, int t)
{
  return out;
}

std::omanip<int>
reltabss(int t)
{
  return omanip<int>(&settabs, t);
}
#endif



#endif // __xmlostream__
