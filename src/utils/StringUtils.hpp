#ifndef _utils_StringUtils_hpp
#define _utils_StringUtils_hpp

#include <utils/String.hpp>
#include <vector>

namespace unicode {

String::size_type
find_among(const String& s, const String& charlist, String::size_type beg=0,String::size_type end=String::npos);

String::size_type
eat_among(const String& s, const String& charlist, String::size_type beg=0,String::size_type end=String::npos);

String::size_type
find_among_reverse(const String& s, const String& charlist, String::size_type beg=String::npos, String::size_type end=0);

String::size_type
eat_among_reverse(const String& s, const String& charlist, String::size_type beg=String::npos, String::size_type end=0);


String::size_type
find_whitespaces(const String& s, String::size_type beg=0, String::size_type end=String::npos);

String::size_type
eat_whitespaces(const String& s, String::size_type beg=0, String::size_type end=String::npos);

String::size_type
find_whitespaces_reverse(const String& s, String::size_type beg=0, String::size_type end=String::npos);

String::size_type
eat_whitespaces_reverse(const String& s, String::size_type beg=0, String::size_type end=String::npos);




bool
get_hex_int(const String& s, unsigned int* val, String::size_type beg=0, String::size_type end=String::npos);

String::size_type
find_not_dec_int(const String& s, String::size_type beg=0, String::size_type end=String::npos);

bool
get_dec_int(const String& s, unsigned int* val,
	    String::size_type beg=0, String::size_type end=String::npos, String::size_type* after=0);

bool
get_dec_signed_int(const String& s, int* val,
	    String::size_type beg=0, String::size_type end=String::npos, String::size_type* after=0);

bool
get_bool(const String& s, bool* val,
	    String::size_type beg=0, String::size_type end=String::npos, String::size_type* after=0);

bool
get_float(const String& s, float* val,
	  String::size_type beg=0, String::size_type end=String::npos, String::size_type* after=0);

#if 1

bool
get_float_list(const String& s, std::vector<float>* v,
	       String::size_type beg=0, String::size_type end=String::npos);

// for <fecolormatrix> (list with %)
bool
get_float_and_percentage_list(const String& s, std::vector<float>* v,
			      String::size_type beg=0, String::size_type end=String::npos);
#endif

}


#endif // _utils_StringUtils_hpp
