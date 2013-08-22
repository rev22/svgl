#ifndef __glft_FontInfo__
#define __glft_FontInfo__

#include <string>

namespace glft {
  struct FontInfo {
    FontInfo() : style(fs_regular), weight(fw_normal) {}

    std::string family;

    enum Style {
      fs_regular,
      fs_normal,
      fs_italic,
      fs_oblique,
      fs_unknown
    };
    Style style;

    static const char* styleDesc[];
    static Style getStyle(const std::string&);

    enum Weight {
      fw_normal,
      fw_bold,
      fw_unknown
    };
    Weight weight;
    
    static const char* weightDesc[];
    static Weight getWeight(const std::string&);

    FontInfo(std::string fam, Style fs=fs_regular, Weight fw=fw_normal) : family(fam), style(fs), weight(fw) {}
  };

}

#endif // __glft_FontInfo__
