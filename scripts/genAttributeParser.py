import parse_idl
import enum_map

import parse_attr
import parse_dtd

import config

from make_cpp_string_name import *

import GenFile

import string




translate_to_classname = {
    "Tspan": "TSpan",
    "Tref": "TRef",
    "Mpath": "MPath",
    "Svg": "SVG",
    "Vkern": "VKern",
    "Hkern": "HKern"
    }

def make_cppname(name):
    beg=0
    while 1:
        pos = string.find(name, '-', beg)
        if pos==-1:
            pos = string.find(name, ':', beg)
        if pos>0:
            res = name[:pos]+ string.upper(name[pos+1]) + name[pos+2:]
            name=res
            beg=pos
        else:
            break

    if name[:2]=="Fe":
        name = "FE" + name[2:]
    else:
        try:
            tmp=translate_to_classname[name]
            name = tmp
        except KeyError:
            pass
    return name



#  # idl
#  if len(parse_idl.class_decls):
#      for (classname, classdecl) in parse_idl.class_decls.items():
#          for enum in classdecl.enums:
#              #            print classname
#              #            print enum
#              pass


#  #dtd
#  if len(parse_dtd.entity_type_decls):
#  	for name, entity_type in parse_dtd.entity_type_decls.items():
#              pass

def print_enum_in_inherit(classdecl):
    if len(classdecl.enums):
        print '--'
        print classdecl.name
        for enum in classdecl.enums:
            print enum
    else:
        for inh in  classdecl.inherits:
            try:
                print_enum_in_inherit(parse_idl.class_decls[inh])
            except KeyError:
                pass
            
def find_enum_in_inherit(enum_literals, classdecl):
    if len(classdecl.enums):
        for enum in classdecl.enums:
            const_lit=[]
            pos = 0
            tmpname = enum.const_decls[0].name
            while 1:
                newpos = string.find(tmpname, '_', pos)
                if newpos==-1:
                    break
                prefix = tmpname[:newpos]
                #print prefix
                stop=0
                for i in enum.const_decls:
                    if i.name[:newpos]!=prefix:
                        stop=1
                        break
                if stop==1:
                    break
                pos=newpos+1

            for lit in enum.const_decls:                
                tmpname = string.lower(lit.name[pos:])
                tmpname = string.replace(tmpname, '_', '-')
                #print tmpname
                const_lit.append(tmpname)
            match=0
            #print enum_literals
            #print const_lit
            for i in enum_literals:
                if string.lower(i) in const_lit:
                    match=match+1
            if match==len(enum_literals):
                return (classdecl, enum)

    for inh in classdecl.inherits:
        try:
            (c, e) = find_enum_in_inherit(enum_literals, parse_idl.class_decls[inh])
            if c and e:
                return (c,e)
        except KeyError:
            pass
    return (None,None)
    
output_cpp_read = ""
output_cpp_print = ""
output_hpp = ""
output_hpp_in_operator = ""
output_hpp_out_operator = ""

includes = ["String"]
attr_strings=[]
already_done=[]

#elements = parse_dtd.entity_common_attrs
elements = parse_dtd.attlists
if len(elements):
    for name, entity_types in elements.items():
        # name = cppName.make_cppname(name+class_suffix)
        # header = cppHeader.Header(name, prefix='svg')
        # name = make_cppname(name+class_suffix)
        # all_elements.append(name)

        tmp=[]
        for entity_type in entity_types:
            lflat = entity_type.expand(parse_dtd.entity_type_decls, parse_dtd.entity_common_attrs)
            tmp.extend(lflat)
        
        #for entity_type in entity_types:
        for entity_type in tmp:
            if isinstance(entity_type, parse_attr.attr_named_enum):
                
                classname = make_cppname(string.upper(name[0]) + name[1:])
                classname = "SVG" + classname + "Element"
                try:
                    classdecl = parse_idl.class_decls[classname]
                except KeyError:
                    if classname == 'SVGMPathElement':
                        continue
                    else:
                        raise
                c,e = find_enum_in_inherit(entity_type.enums, classdecl)
                if entity_type.name =='xml:space':
                    #print classdecl.name, entity_type
                    #print e
                    pass

                if c!=None and e!=None:
                    #if c.name == "SVGComponentTransferFunctionElement":
                    #    print name, entity_type
                    #print c.name
                    #print e

                    i = entity_type.name
                    #print entity_type.name, e.name
                    iname = make_cpp_string_name(i)
                    if (iname,i) not in attr_strings:
                            attr_strings.append((iname, i))
                    
                    if c.name+e.name in already_done:
                        continue

                    already_done.append(c.name+e.name)

                    if c.name not in includes:
                        includes.append(c.name)


                    output_cpp_read = output_cpp_read + """
svg::%s::%s
make_%s(const svg::String& s)
{
"""%(c.name, e.name, c.name+'_'+ e.name)

                    output_cpp_print = output_cpp_print + """
const svg::String&
print_%s(%s val)
{
   switch (val) {
"""%(c.name+'_'+ e.name, c.name+'::'+e.name)
                    
                    output_hpp = output_hpp + """
svg::%s::%s
make_%s(const svg::String& s);

const svg::String&
print_%s(svg::%s val);
"""%(c.name, e.name, c.name+'_'+ e.name, c.name+'_'+e.name, c.name+'::'+e.name)

                    output_hpp_in_operator = output_hpp_in_operator + """
inline
void
read(const svg::String& in, svg::%s::%s& e)
{
  e = svgl::make_%s(in);
}

"""%(c.name, e.name, c.name+'_'+ e.name)

                    output_hpp_out_operator = output_hpp_out_operator + """
inline
std::ostream&
operator << (std::ostream& out, const svg::%s::%s& val)
{
   out << svgl::print_%s(val);
   return out;
}

"""%(c.name, e.name, c.name+'_'+ e.name)

                    #print entity_type
                    default_value=None
                    if isinstance(entity_type.type, parse_attr.attr_type_default_value)==1:
                        default_value = entity_type.type.default_value
                        iname = make_cpp_string_name(default_value)
                        attrname = string.replace(string.upper(default_value),'-','_')
                        output_cpp_read = output_cpp_read + '''
   if (!s || !s->getLength() || *s==*svg::%s_attribute_string) { // default value
      return %s::%s;
   } else
'''%( iname, c.name, e.name+'_'+ attrname)
                        
                    else:
                        output_cpp_read = output_cpp_read + '''
   if(!s || !s->getLength()) {
      std::cerr << "unknown enum ";
      if(s)
         std::cerr << "\\"" << s << "\\"";
      std::cerr << " for %s::%s (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
      return %s::%s;
   } else
'''%(c.name, e.name, c.name, e.name+'_'+'UNKNOWN')

                    for dtdstring in entity_type.enums:
                        iname = make_cpp_string_name(dtdstring)
                        if (iname, dtdstring) not in attr_strings:
                            attr_strings.append((iname, dtdstring))
                        attrname = string.replace(string.upper(dtdstring),'-','_')
                        if dtdstring!=default_value:
                            output_cpp_read = output_cpp_read + '''
   if (*s==*svg::%s_attribute_string) {
      return %s::%s;
   } else
'''%( iname, c.name, e.name+'_'+ attrname)

                        output_cpp_print = output_cpp_print + '''
      case %s::%s:
         return svg::%s_attribute_string;
'''%(c.name, e.name+'_'+attrname, iname)



                    output_cpp_read = output_cpp_read + '''
   {
      std::cerr << "unknown enum " << s << " for %s::%s (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
      return %s::%s;
   }
}
'''%(c.name, e.name, c.name, e.name+'_'+'UNKNOWN')

                    output_cpp_print = output_cpp_print + '''
      default:
         return unknown_string;
   }
}
'''
                    
                else:
                    pass
                    #if string.find(entity_type.name, ':')>0:
                        #print classdecl.name, entity_type.name
                
                    
            elif isinstance(entity_type, parse_attr.attr_named):
                i=entity_type.name
                iname = make_cpp_string_name(i)
                #print i.name
                if (iname, i) not in attr_strings:
                    attr_strings.append((iname, i))
                    
                #if name=='type':
                #print '*** ', name, entity_type
                #print classdecl
                pass


                
            
            # if str(entity_type.__class__) == 'parse_attr.attr_entity_ref':
                # lflat = entity_type.expand(parse_dtd.entity_type_decls, parse_dtd.entity_common_attrs)
                #if parse_dtd.entity_type_decls.has_key(entity_type.enity_name):
                    #en = parse_dtd.entity_type_decls[entity_type.entity_name]


            #if str(entity_type.__class__) == 'parse_attr.attr_entity_ref':
            #    entity_name = cppName.make_cppname(entity_type.entity_name)
            #    inherits = inherits +'\t\t\t%s,\n'%(entity_name)
            #    includes = includes +'#include <svg/%s.hpp>\n'%(entity_name)

            #elif isinstance(entity_type, parse_attr.attr_named_entity_ref):
            #    entity_name = cppName.make_cppname(entity_type.entity_name)
            #    includes = includes + '#include <svg/%s.hpp>\n'%(entity_name)
            #    fields = fields + '\t\t\t%s %s;\n'%(entity_name, cppName.make_cppname(entity_type.name))

            #elif isinstance(entity_type, parse_attr.attr_named_enum):
            #    entity_name = cppName.make_cppname(entity_type.name)

includestr='#include <w3c/svg/strings.hpp>\n'
for i in includes:
    includestr = includestr+'#include <w3c/svg/%s.hpp>\n'%i


#f = open(config.genDir+"/makeEnumType.cpp", "w")
f = GenFile.gfopen(config.genDir+"/makeEnumType.cpp")
f.write('''
// generated by genAttributeParser.py

#include <w3c/svg/makeEnumType.hpp>
#include <iostream>

namespace svgl {
using namespace svg;
'''+output_cpp_read+'''
'''+output_cpp_print+'''
}
''')

#f = open(config.genDir+"/makeEnumType.hpp", "w")
f = GenFile.gfopen(config.genDir+"/makeEnumType.hpp")
f.write('''
#ifndef __makeEnumType__
#define __makeEnumType__
// generated by genAttributeParser.py\n\n''' + includestr+'''

#include <iostream>
namespace svgl {
'''+output_hpp+'''
'''+output_hpp_in_operator+'''
'''+output_hpp_out_operator+'''
}
#endif //__makeEnumType__
''')


#f=open(config.genDir+"/attribute_strings_def.hpp",'w')
f=GenFile.gfopen(config.genDir+"/attribute_strings_def.hpp",'w')
f.write('// generated by genAttributeParser.py\n')
for (iname, i) in attr_strings:
    f.write('STRDEF(%s,"%s")\n'%(iname,i))




