import enum_map
import parse_idl

import string

def mapAttributeType(classdecl, attr):
    ispointer = 0
    typename = attr.type.name
    pos = string.find(attr.type.name, 'Element')
    if pos > 0:
        #fwd_decls.append(typename)
        typename = typename+'*'
        ispointer=1
    else:

        if attr.type.name=="unsigned short" or string.find(attr.type.name, 'Enumeration')>=0:
            try:
                enumname = enum_map.enum_map[classdecl.name+'::'+attr.name]
                if attr.type.name=="unsigned short":
                    typename = enumname
                else:
                    pos = string.find(attr.type.name, 'Animated')
                    if pos >= 0:
                        typename = 'Animated<%s>'%enumname
                    else:
                        typename = string.replace(typename, 'Enumeration', enumname)
                    # typename = classdecl.enums[numtype].name
            except KeyError: # gen enum_map
                try:
                    tmp = classdecl.enums[0].name
                except IndexError:
                    # maybe a UnitType
                    for inh in ["SVGUnitTypes", "SVGRenderingIntent"]:                            
                        if inh in classdecl.inherits:
                            tmp = parse_idl.class_decls[inh].enums[0].name
                            break
                        else:
                            tmp = "0"
                print '"'+classname+'::'+attr.name +'": "' + tmp + '" ,'

        elif typename=="boolean":
            typename="bool"
#          else:
#              if typename not in cppbuiltin_types:
#                  if typename not in includes:
#                      includes.append(typename)


        # is it a simple List ?
        if typename=="SVGList":
            typename="SVGList<>"

    return (typename, ispointer)
