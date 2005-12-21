#include <utils/StringUtils.hpp>
#include <svgl/debug.hpp>

#include <cmath>

namespace unicode {

  String::size_type
  find_among(const String& s, const String& charlist, String::size_type beg/*=0*/,
	     String::size_type end/*=String::npos*/)
  {
    String::size_type charlistlen = charlist.getSize();
    if(!charlistlen)
      return String::npos;
    
    if (end==String::npos)
      end = s.getSize();
    
    for(String::size_type i=beg; i<end; ++i) {
      String::char_type c=s[i];
      for(String::size_type j=0; j<charlistlen; ++j) {
	if (c==charlist[j])
	  return i;
      }
    }
    return String::npos;
  }

  String::size_type
  eat_among(const String& s, const String& charlist, String::size_type beg/*=0*/,
	     String::size_type end/*=String::npos*/)
  {
    String::size_type charlistlen = charlist.getSize();
    if(!charlistlen)
      return String::npos;
    
    if (end==String::npos)
      end = s.getSize();
    
    for(String::size_type i=beg; i<end; ++i) {
      String::char_type c=s[i];
      bool res=false;
      for(String::size_type j=0; j<charlistlen; ++j) {
	if (c==charlist[j]) {
	  res=true;
	  break;
	}
      }
      if(!res)
	return i;
    }
    return String::npos;
  }


  String::size_type
  eat_whitespaces(const String& s, String::size_type beg/*=0*/, String::size_type end/*=String::npos*/)
  {
#if 1
    static const String* spacesptr = String::internString(" \t\n\r");
    const String& spaces = *spacesptr;
    return eat_among(s, spaces, beg, end);
#else
    static const String::char_type space=' ', tab='\t', newline='\n', linefeed='\r';
    const String::size_type len = s.getSize();
    if (end>len)
      end=len;
    
    while (beg<len) {
      String::char_type c = s[beg];
      if (c==space || c==tab || c==newline || c==linefeed)
	++beg;
      else
	break;
    }
    return beg;
#endif
  }

  String::size_type
  find_whitespace(const String& s, String::size_type beg/*=0*/, String::size_type end/*=String::npos*/)
  {
    static const String* spacesptr = String::internString(" \t\n\r");
    const String& spaces = *spacesptr;
    return find_among(s, spaces, beg, end);
  }


  String::size_type
  eat_whitespaces_reverse(const String& s, String::size_type beg/*=String::npos*/, String::size_type end/*=0*/)
  {
    static const String::char_type space=' ', tab='\t', newline='\n', linefeed='\r';

    const String::size_type len = s.getSize();
    if(beg>len)
      beg=len-1;

    while (beg>=end) {
      String::char_type c = s[beg];
      if (c==space || c==tab || c==newline || c==linefeed)
	--beg;
      else
	break;
    }
    return beg;
  }


  bool
  get_hex_int(const String& s, unsigned int* val, String::size_type beg/*=0*/, String::size_type end/*=String::npos*/)
  {
    static const String* numsptr = String::internString("0123456789"); // {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}
    static const String* alphalowptr = String::internString("abcdef"); // {'a', 'b', 'c', 'd', 'e', 'f'};
    static const String* alphaupptr = String::internString("ABCDEF"); //{'A', 'B', 'C', 'D', 'E', 'F'};

    const String& nums = *numsptr;
    const String& alphalow = *alphalowptr;
    const String& alphaup = *alphaupptr;

    const String::size_type len = s.getSize();
    if(!len)
      return false;
    if (end>len)
      end=len;

    unsigned int v=0;
    bool res=false;
    bool ok=true;

    for (String::size_type j = beg; j<end && ok; ++j) {
      const String::char_type& c = s[j];
      ok=false;
      unsigned int i=0;
      for(;i<10; ++i) {
	if (nums[i]==c){
	  v<<=4;
	  v+=i;
	  res=true;
	  ok=true;
	  break;
	}
      }

      if(!ok) {
	for(i=0;i<6; ++i) {
	  if (alphalow[i]==c || alphaup[i]==c) {
	    v<<=4;
	    v+=i+10;
	    res=true;
	    ok=true;
	    break;
	  }
	}
      }
    
      if(!ok)
	break;
    }

    *val=v;
    return res;
  }


  String::size_type
  find_not_dec_int(const String& s, String::size_type beg/*=0*/, String::size_type end/*=String::npos*/)
  {
    static const String* numsptr = String::internString("0123456789"); // {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}
    const String& nums = *numsptr;

    const String::size_type len = s.getSize();
    if(!len)
      return String::npos;
    if (end>len)
      end=len;

    for (String::size_type j = beg; j<end; ++j) {
      const String::char_type& c = s[j];
      unsigned int i=0;
      for(;i<10; ++i) {
	if (nums[i]==c) {
	  break;
	}
      }
      if (i==10)
	return j;
    }
    return String::npos;
  }


  bool
  get_dec_int(const String& s, unsigned int* val,
	      String::size_type beg/*=0*/, String::size_type end/*=String::npos*/, String::size_type* after/*=0*/)
  {
    static const String* numsptr = String::internString("0123456789"); // {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}
    const String& nums = *numsptr;

    const String::size_type len = s.getSize();
    if(!len)
      return false;
    if (end>len)
      end=len;

    unsigned int v=0;
    bool res=false;
    String::size_type j;

    for (j = beg; j<end; ++j) {
      const String::char_type& c = s[j];
      unsigned int i=0;
      for(;i<10; ++i) {
	if (nums[i]==c){
	  v*=10;
	  v+=i;
	  res=true;	
	  break;
	}
      }
      if(i==10) {
	break;
      }
    }

    if(after) {
      //std::cerr << "yes " << j << std::endl;
      *after=j;
    }


    *val=v;
    return res;
  }


  bool
  get_dec_signed_int(const String& s, int* val,
	      String::size_type beg/*=0*/, String::size_type end/*=String::npos*/, String::size_type* after/*=0*/)
  {
    static const String::char_type minus='-';

    const String::size_type len = s.getSize();
    if(!len)
      return false;
    if (end>len)
      end=len;

    if(beg>end)
      return false;

    unsigned int v=0;
    bool res;

    if(s[beg]==minus) {
      res = get_dec_int(s, &v, beg+1, end, after);
      *val=-v;
    }
    else {
      res = get_dec_int(s, &v, beg, end, after);
      *val=static_cast<int>(v);
    }

    return res;
  }


#define DBG std::cerr << "\"" << s.substring(beg,end) << "\" is not a float " <<__FILE__ ":" << __LINE__ << std::endl;

  bool
  get_float(const String& s, float* val,
	    String::size_type beg/*=0*/, String::size_type end/*=String::npos*/, String::size_type* after/*=0*/)
  {
    static const String::char_type dot='.', minus='-', plus='+', expon='e';

    const String::size_type len = s.getSize();
    if(!len)
      return false;
    if (end>len)
      end=len;

    //std::cerr << "\"" << s.substring(beg,end) << "\" " << __FL__;

    bool neg=false;
    if (s[beg]==minus) {
      neg=true;
      ++beg;
    } else if (s[beg]==plus)
      ++beg;

    // v1.v2ev3
    unsigned int v1=0, v2=0;
    int v3=0;  
    String::size_type pos = find_not_dec_int(s, beg, end);
    String::size_type __after;
    // if it's equivalent to an int
    if(pos>=end) {
        if(!get_dec_int(s, &v1, beg, pos, &__after)) {
            DBG;
            return false;
        }
        *val=v1;
    }
    // of the form aa.bbb and maybe aa.bbecc or aaebb
    else {
        int component=0;
        if(get_dec_int(s, &v1, beg, pos, &__after)) // get 'aa' if any
            ++component;
        float div=1;
        bool exponentSet = false;
        String::size_type pos2 = find_not_dec_int(s,pos+1, end);
        
        //std::cerr << DBGVAR(v1) << DBGVAR(v2) << DBGVAR(v3) << DBGVAR(beg) << DBGVAR(pos) << DBGVAR(pos2) << __FL__;

        //if(pos!=String::npos) {
				if(pos!=end) {
            //std::cerr << (char) s[pos] << __FL__;
            if(s[pos]==dot) {
            
                if(!get_dec_int(s, &v2, pos+1, pos2, &__after)) // handle the 'aa.' case
                    v2=0;
                else {
                    ++component;
                    if(pos2==String::npos)
									      pos2=end;
                    for(unsigned int i=0; i<(pos2-pos-1); ++i)
                        div*=.1;
                }
                pos = pos2;
								//STDDBG;
                pos2 = find_not_dec_int(s,pos+1, end);
            }
        }
        //STDDBG;
        //if(pos!=String::npos && s[pos]==expon) {
				if(pos!=end && s[pos]==expon) {
            bool neg=false;
            beg = pos+1;
            //std::cerr << DBGVAR(v1) << DBGVAR(v2) << DBGVAR(v3) << DBGVAR(beg) << DBGVAR(pos) << DBGVAR(pos2) << __FL__;

            if (s[beg]==minus) {
                neg=true;
                ++beg;
                pos2 = find_not_dec_int(s,beg, end);
            } else if (s[beg]==plus) {
                ++beg;
                pos2 = find_not_dec_int(s,beg, end);
            }
            //std::cerr << DBGVAR(v1) << DBGVAR(v2) << DBGVAR(v3) << DBGVAR(beg) << DBGVAR(pos) << DBGVAR(pos2) << DBGVAR(len) << DBGVAR(end) <<__FL__;
            if(pos2==String::npos)
                pos2=end;
            //for(String::size_type toto=beg; toto <pos2; ++toto)
                //std::cerr << (char) s[toto];
            //std::cerr << __FL__;
						unsigned int uv3;
            if(get_dec_int(s, &uv3, beg, pos2, &__after)) { // handle the 1e case
                //std::cerr << DBGVAR(v3) << DBGVAR(beg) << DBGVAR(pos2) << DBGVAR(__after) << __FL__;
                exponentSet = true;
                ++component;
            }
            if(neg)
                v3=-uv3;
						else
							v3=uv3;
            //std::cerr << DBGVAR(v3) << __FL__;
        }
        //std::cerr << DBGVAR(v1) << DBGVAR(v2) << DBGVAR(v3) << DBGVAR(beg) << DBGVAR(pos) << DBGVAR(pos2) << __FL__;
#if 0
        if(pos2!=String::npos) {
            //end=pos2;
            __after=pos2;
        } else {
            __after=pos;
        }
#endif        
        if (!component) { // maybe a "."
            DBG;
            return false;
        }
        
        *val = v1+v2*div;
        if(exponentSet) {
            double e = pow(10,v3);
            //std::cerr << DBGVAR(*val) << DBGVAR(v3) << DBGVAR(e) << __FL__;
            *val *= e;
            //std::cerr << DBGVAR(*val) << __FL__;
        }
    }

    if(neg)
      *val=-*val;

    if(after)
      *after = __after;

		//std::cerr << DBGVAR(*val) << __FL__;

    return true;
  }

  bool
  get_bool(const String& s, bool* val,
	      String::size_type beg/*=0*/, String::size_type end/*=String::npos*/, String::size_type* after/*=0*/)
  {
    static const String
      *true_stringptr=String::internString("true"),
      *false_stringptr=String::internString("false");

    const String
      &true_string = *true_stringptr,
      &false_string = *false_stringptr;

    if(end>s.getSize())
      end=s.getSize();

    if(beg>=s.getSize())
      return false;

    beg = eat_whitespaces(s,beg, end);
    if(beg>=s.getSize())
      return false;
    String::size_type pos = find_whitespace(s,beg,end);
    const String* subsptr = s.substring(beg,pos);
    const String& subs = *subsptr;
    if(subs==true_string) {
      *val=true;
      return true;
    }
    else
    if(subs==false_string) {
      *val=false;
      return true;
    }

    if(after)
      *after=pos;

    return false;
   
  }


#if 1

  bool
  get_float_list(const String& s, std::vector<float>* v, String::size_type beg/*=0*/, String::size_type end/*=String::npos*/)
  {
    // minus acts as a seperator and is significant for a float
    // example: a list of 2 floats (5,-6) can be written as "5-6"
    static const String::char_type minus='-';
    static const String::char_type expon='e';
    static const String& seps = *String::internString(",;- \t\r\n");
    static const String& sepswominus = *String::internString(",; \t\r\n");

    if (end > s.getSize())
        end=s.getSize();

    v->empty();
    //  std::cerr << "-> " << s.substr(beg,end) << std::endl;;
    beg = eat_whitespaces(s, beg);
    while (beg<end) {
        String::size_type pos;
        
        if(s[beg]==minus)
            pos = find_among(s, seps, beg+1);
        else
            pos = find_among(s, seps, beg);
        
        if(pos!=String::npos && pos>1 && s[pos]==minus && s[pos-1]==expon)
            pos = find_among(s, sepswominus, beg);

        if (pos>end)
            pos=end;
        float val;
        if(!get_float(s, &val, beg, pos)) {
            std::cerr << val << std::endl;
            return false;
        }
        v->push_back(val);
        
        // minus is among seps...
        if(pos<end && s[pos]!=minus)
            ++pos;
        beg = eat_whitespaces(s, pos);
        //std::cerr << s.substring(beg) << std::endl;
    }
    //  std::cerr << v->size() << "<-" << std::endl;
    return true;
  }

  // for <fecolormatrix> (list with %)
  bool
  get_float_and_percentage_list(const String& s, std::vector<float>* v, String::size_type beg/*=0*/, String::size_type end/*=String::npos*/)
  {
    static const String::char_type minus='-', percentage='%';
    static const String& seps=*String::internString(", -\t\r\n");

    if (end > s.getSize())
      end=s.getSize();

    v->empty();
    //  std::cerr << "-> " << s.substr(beg,end) << std::endl;;
    beg = eat_whitespaces(s, beg);
    while (beg<end) {
      String::size_type pos;
      if(s[beg]==minus)
	pos = find_among(s, seps, beg+1);
      else
	pos = find_among(s, seps, beg);
      if (pos>end)
	pos=end;
      float val;
      unsigned int after;
      if(!get_float(s, &val, beg, pos, &after)) {
	std::cerr <<  val << std::endl;
	return false;
      }
      if(after<end && s[after]==percentage) {
	val/=100.;
	++pos;
      }
      v->push_back(val);

      // minus is among seps...
      if(pos<end && s[pos]!=minus)
	++pos;
      beg = eat_whitespaces(s, pos);
      //    std::cerr << s.substr(beg) << std::endl;
    }
    //  std::cerr << v->size() << "<-" << std::endl;
    return true;
  }

#endif


}
