#ifndef __css_TextAnchor__
#define __css_TextAnchor__

//#include <utils/String.hpp>
namespace unicode {
  class String;
}

namespace css {

  class TextAnchor {
  public:
    enum anchor_t {
      CSS_TEXTANCHOR_UNKNOWN,
      CSS_TEXTANCHOR_START,
      CSS_TEXTANCHOR_MIDDLE,
      CSS_TEXTANCHOR_END,
      CSS_TEXTANCHOR_INHERIT
    };

    anchor_t value;
    TextAnchor(anchor_t v) : value(v) {}
  };

 TextAnchor
 makeTextAnchor(unicode::String*);

}

#endif // __css_TextAnchor__
