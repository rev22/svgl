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
#ifndef __svg_Parser__
#define __svg_Parser__

#include <svgl/config.hpp>
#include <w3c/svg/String.hpp>
//#include <w3c/svg/strings.hpp>

#include <utils/InputSource.hpp>


// #include <w3c/svg/length.hpp>
// #include <w3c/svg/graphic.hpp>
// #include <w3c/svg/group.hpp>
// #include <w3c/svg/rect.hpp>

#include <istream>
#include <iostream>

class DOM_Node;
class DOMString;

namespace svg {
  class SVGDocument;
}

namespace svgl {

  typedef svg::String string;
  extern string empty_string;



  class Parser {
  public:

    struct exception {
      explicit exception(const svg::String& msg_, const svg::String& file_=empty_string, int line_=-1) : msg(msg_), file(file_), line(line_) {}
      virtual ~exception() {}
      void print(std::ostream& out) const { out << msg << " (moncul" << file << ":" << line << ")"; }
      const svg::String& what() const { return msg; }

      const svg::String msg, file;
      const int line;
    };

    Parser();
	 
	 svg::SVGDocument * parseInputSource(sdom::InputSource *input);
	 
    svg::SVGDocument * parse(const char* url);
    svg::SVGDocument * parseFromString(unicode::String*);
	 svg::SVGDocument * parseFromStream(std::istream *);

  private:

  };
}



inline
std::ostream&
operator<<(std::ostream& out, const svgl::Parser::exception& e)
{
  e.print(out);
  return out;
}


#endif // __svg_Parser__
