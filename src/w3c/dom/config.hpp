#ifndef config_hpp
#define config_hpp

#define BEGIN_NAMESPACE(ns) namespace ns {
#define END_NAMESPACE(ns) }

# if (_MSC_VER > 1000)
// #pragma warning ( disable : 4251 )	// ignore template classes being exported in .dll's
/* 
 * "this used in base member initializer list"
 * arrow operator warning
 * copy constr & assignment cannot be generated
 * "forcing value to bool 'true' or 'false'
 * typedef used instaead of full type
 * 4018 : signed/unsigned mismatch, 4146 - result still unsigned 
 */
#  pragma warning ( disable : 4250 4290 4355 4284  4231 4511 4512 4097 4786 4800 4018 4146 4244 4514 4127 )
#  pragma warning ( disable : 4245 ) // conversion from enum to unsigned int signed/unsigned mismatch
#  if (_MSC_VER > 1200)
// multiple copy constructors/assignment operators specified,
// with member templates are bogus...
#   pragma warning ( disable : 4521 4522)
#  endif  
# endif

#endif
