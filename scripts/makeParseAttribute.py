import parse_idl
import enum_map
import parse_attr
import parse_dtd
import mapDtdIdl
from make_cpp_string_name import *
import string
import mapAttributeType
import config
import GenFile

# idl
#if len(parse_idl.class_decls):
#    for (classname, classdecl) in parse_idl.class_decls.items():
#        for enum in classdecl.enums:
            #            print classname
            #            print enum
#            pass


#dtd
#if len(parse_dtd.entity_type_decls):
#	for name, entity_type in parse_dtd.entity_type_decls.items():
#            pass

customParser = ["SVGFEGaussianBlurElement", "SVGMarkerElement", "SVGStylable"]


def find_enum_in_inherit(enum_literals, classdecl):
    if len(classdecl.enums):
        for enum in classdecl.enums:
            const_lit=[]
            for lit in enum.const_decls:
                pos = string.rfind(lit.name, '_')
                const_lit.append(string.lower(lit.name[pos+1:]))
            match=0
            #print enum_literals
            #print const_lit
            for i in enum_literals:
                if string.lower(i) in const_lit: match=match+1
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

def find_name_in_inherit(name, classdecl):
    if len(classdecl.attributes):
        for attr in classdecl.attributes:
            if string.lower(attr.name)==name:
                return (classdecl, attr)

    for inh in classdecl.inherits:
        try:
            (c, e) = find_name_in_inherit(name, parse_idl.class_decls[inh])
            if c and e:
                return (c,e)
        except KeyError:
            pass
    return (None,None)

output_cpps = {}
output_get_attr_by_names = {}
print_output_cpps={}

includes = ["String"]
attr_strings=[]
already_done=[]

#elements = parse_dtd.entity_common_attrs
#elements = parse_dtd.attlists
#if len(elements):
#    for name, entity_types in elements.items():
#        try:
#            classdecl = mapDtdIdl.elements_dtd_idl[name]
#        except:
#            if name=="mpath":
#                continue
#            else:
#                raise

#elements = parse_dtd.attlists

def make_cpp(name):
    if name in ['operator']:
        return name + '_'

    return name


class AlreadyProcessed:
    def __init__(self):
        pass

def process(classdecl):

    if classdecl.name in includes:
        raise AlreadyProcessed

    if classdecl in already_done:
        return 0;
    already_done.append(classdecl)

    nattr=0

    begin_string = '''
// %s
    
int
%s::parse_attributes(const String& attrName, const String& attrValue)
{
  int recognized=0;
//  std::cerr << "%s::parse" << std::endl;
  {
'''%(classdecl.name, classdecl.name, classdecl.name)

    begin_print_string = '''
// %s
    
void
%s::print_attributes(xmlostream& out)
{
'''%(classdecl.name, classdecl.name)

    begin_get_attr_by_name_string = '''
// %s
    
svgl::AnimateAdapter*
%s::getAttributeByName(const String& attrName)
{
//  std::cerr << "%s::getAttributeByName " << attrName << std::endl;

'''%(classdecl.name, classdecl.name, classdecl.name)


    for attr in classdecl.attributes:
        try:
            entity_name = mapDtdIdl.attributes_idl_dtd[attr]
        except KeyError:
            #print classdecl.name, attr
            #print classdecl
            #print mapDtdIdl.attributes_idl_dtd
            #raise ""
            continue

        if nattr==0:
            includes.append(classdecl.name)
            output_cpp = begin_string
            print_output_cpp = begin_print_string + '   out '
            output_get_attr_by_name = begin_get_attr_by_name_string

        nattr=nattr+1
        output_cpp = output_cpp + '''
    if (*attrName==*%s_attribute_string) {
      *attrValue >> %s;
      ++recognized;
    } else
'''%(make_cpp_string_name(entity_name), make_cpp(attr.name))

        print_output_cpp = print_output_cpp + '''<< " %s=\\"" << %s << "\\"" '''%(entity_name, make_cpp(attr.name))

        (typename, ispointer)=mapAttributeType.mapAttributeType(classdecl, attr)

        output_get_attr_by_name = output_get_attr_by_name + '''
    if (*attrName==*%s_attribute_string) {
      return new svgl::AnimateAdapterTemplate< Attribute< %s > >(&%s);
    } else
'''%(make_cpp_string_name(entity_name), typename, make_cpp(attr.name))

    if nattr>0:
        print_output_cpp = print_output_cpp + ' ;\n'

    resDeclared = 0
    for inh in classdecl.inherits:
        try:
            res = process(parse_idl.class_decls[inh])
        except AlreadyProcessed:
            res=1
        except KeyError:
            if inh not in ["Element", "events::EventTarget", "events::DocumentEvent", "css::ViewCSS", "css::DocumentCSS",
                           "css::CSSValue", "smil::ElementTimeControl", "Document", "events::UIEvent", "css::CSSRule",
                           "events::Event"]:
                raise
            else:
                continue
        if res>0:
            if nattr==0:
                includes.append(classdecl.name)
                output_cpp = begin_string
                print_output_cpp = begin_print_string

                output_get_attr_by_name = begin_get_attr_by_name_string

            if resDeclared==0:
                output_get_attr_by_name = output_get_attr_by_name + '''
    {
       svgl::AnimateAdapter* res = 0;
'''
                resDeclared=1


            nattr = nattr+res
            output_cpp = output_cpp + '''
    if (%s::parse_attributes(attrName, attrValue)) {
      ++recognized;
    } else
'''%(inh)
            print_output_cpp= print_output_cpp+'''   %s::print_attributes(out);\n'''%(inh)

            output_get_attr_by_name = output_get_attr_by_name + '''
       res = %s::getAttributeByName(attrName);
       if (res) {
         return res;
       }
'''%(inh)


    if nattr>0:
        if classdecl.name in customParser:
             #print classdecl.name
            output_cpp = output_cpp + '''
    if (customParseAttributes(attrName, attrValue)) {
      ++recognized;
    } else
'''

        #if it's an element
        #if string.find(classdecl.name,"Element")>0 and classdecl.name != "SVGElement":
        if mapDtdIdl.elements_idl_dtd.has_key(classdecl):
                
                

            output_cpp = output_cpp + '''
    {
      // std::cerr << "unknown attribute %s::" << attrName << std::endl;
    }
'''%(classdecl.name)
            output_get_attr_by_name = output_get_attr_by_name + '''
      {
         std::cerr << "unknown attribute %s::" << attrName << std::endl;
      }
'''%(classdecl.name)

        else:
            output_cpp = output_cpp +'''
    {
    }
'''

        output_cpp = output_cpp+'''
  }
  return recognized;
}      
'''

        if(resDeclared):
            output_get_attr_by_name = output_get_attr_by_name + '''
  }
'''        
        output_get_attr_by_name = output_get_attr_by_name + '''
  return 0;
}
'''

        print_output_cpp = print_output_cpp + '''}\n'''
        
        output_cpps[classdecl.name]=output_cpp
        print_output_cpps[classdecl.name]=print_output_cpp
        output_get_attr_by_names[classdecl.name]=output_get_attr_by_name

        #print classdecl.name, len(output_get_attr_by_name)

    else:
        #print classdecl
        pass

    
    return nattr


if len(parse_idl.class_decls):
    w = parse_idl.class_decls.keys()
    v = []
    for name in w:
	v.append(name)
    v.sort()
    for name in v:
        classdecl = parse_idl.class_decls[name]
        try:
            process(classdecl)
        except AlreadyProcessed:
            continue


#keys = output_cpps.keys()
#keys.sort()

output_cpp=""
if len(output_cpps):
    for key,out in output_cpps.items():
    #for key in keys:
        #out = output_cpps[key]
        output_cpp=output_cpp+out

output_get_attr_by_name=""
if len(output_get_attr_by_names):
    for key,out in output_get_attr_by_names.items():
        output_get_attr_by_name=output_get_attr_by_name+out


print_output_cpp=""
if len(print_output_cpps):
    for key,out in print_output_cpps.items():
        print_output_cpp=print_output_cpp+out

includestr=''
for i in includes:
    includestr = includestr+'#include <w3c/svg/%s.hpp>\n'%i


parse_includestr='''//#include <w3c/svg/makeEnumType.hpp>
#include <svgl/parseHelper.hpp>
//#include <w3c/dom/DOM.hpp>
#include <w3c/svg/String.hpp>
'''

f = GenFile.gfopen(config.genDir+"/ElementParseAttributesMethods.cpp", 'w')
f.write('// generated by makeParseAttribute.py\n\n' + parse_includestr + includestr + '''

namespace svg {
'''+
output_cpp
+'''
}
''')

parse_includestr='''#include <w3c/svg/makeEnumType.hpp>
#include <svgl/parseHelper.hpp>
//#include <dom/DOM.hpp>
#include <w3c/svg/String.hpp>
#include <svgl/AnimateAdapter.hpp>
#include <svgl/AnimateAdapterDouble.hpp>
#include <svgl/AnimateAdapterColor.hpp>
'''

f = GenFile.gfopen(config.genDir+"/ElementGetAttributeByNameMethods.cpp", 'w')
f.write('// generated by makeParseAttribute.py\n\n' + parse_includestr + includestr + '''

namespace svg {
'''+
output_get_attr_by_name
+'''
}
''')


parse_includestr='''#include <svgl/xmlostream.hpp>
#include <svgl/printHelper.hpp>
#include <w3c/svg/makeEnumType.hpp>
'''

f = GenFile.gfopen(config.genDir+"/ElementPrintMethods.cpp", 'w')
f.write('// generated by makeParseAttribute.py\n\n' + parse_includestr + includestr + '''

namespace svg {
'''+
print_output_cpp
+'''
}
''')








