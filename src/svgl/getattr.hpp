#ifndef __svgl_getattr__
#define __svgl_getattr__

#define GET_ANIMATED_VAL(x) dom_get##x ().getValue().getAnimatedVal().getValue()
#define MYGET_ANIMATED_VAL(x) x.getValue().getAnimatedVal().getValue()

#define GET_BASE_VAL(x) dom_get##x ().getValue().getBaseVal().getValue()
#define MYGET_BASE_VAL(x) x.getValue().getBaseVal().getValue()

#define GETVAL(x) GET_ANIMATED_VAL(x)
#define MYGETVAL(x) MYGET_ANIMATED_VAL(x)

#define SETVAL(x,v) \
x.getValue().getBaseVal().setValue(v); \
x.getValue().getAnimatedVal().setValue(v);

#define SETVAL_AS_STRING(x,v) \
MYGET_BASE_VAL(x).dom_setValueAsString(unicode::String::internString(v)); \
MYGET_ANIMATED_VAL(x).dom_setValueAsString(unicode::String::internString(v));

#define GET_SIMPLE_VAL(x) dom_get##x ().getValue()
#define SET_SIMPLE_VAL(x,v) dom_get##x ().setValue(v)


#endif // __svgl_getattr__
