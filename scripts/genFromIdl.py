import parse_idl
import cppHeader
import enum_map
import string
import mapDtdIdl
import interfaces
from make_cpp_string_name import *
import config
import cplusplus
import GenFile

all_headers=[]

attr_to_include=["SVGLength","SVGString"]

includes_cpp=[]
output_cpp = """"""

used_lists = []
used_animated = []
#used_attributes = []
#used_pointer_attributes = []

destructors=[]

output_get_attr_by_name=''
includes_get_attr_by_name=[]


def find_dtd_attr_in_inherit(classdecl):
    if len(classdecl.attributes):
        for attr in classdecl.attributes:
            if mapDtdIdl.attributes_idl_dtd.has_key(attr):
                return 1

    for inh in classdecl.inherits:
        try:
            res = find_dtd_attr_in_inherit(parse_idl.class_decls[inh])
            if res > 0:
                return 1
        except KeyError:
            pass
    return 0


if len(parse_idl.class_decls):
    for (classname, classdecl) in parse_idl.class_decls.items():

        if string.find(classname, "Animated") >=0 and classname not in ["SVGAnimatedPathData","SVGAnimatedPoints",""]:
            used_animated.append(classname)
            continue
        if string.find(classname, "List") >=0:
            used_lists.append(classname)
            continue

        # print classdecl
        header = cppHeader.Header(classname, prefix="svg")
        all_headers.append(classname)

        includes=[]
        fwd_decls=[]
        doGetAttrByName=0

        output = ''

        # inheritance
        output = output + "namespace svg {\n"        
        output = output + "\tclass %s"%(classname)
        if len(classdecl.inherits):
            output=output+' :\n'
            output = output + '\t\tpublic %s'%(classdecl.inherits[0])
            includes.append(classdecl.inherits[0])
            if len(classdecl.inherits[1:]):
                for inherit in classdecl.inherits[1:]:
                    output = output + ',\n\t\tpublic %s'%(inherit)
                    includes.append(inherit)

        # enums
        output = output + ' {\n'
        #        if len(classdecl.enums)>1:
        #            print classname
        #if len(classdecl.inherits)==1:
        #    output = output + '\ttypedef %s super;\n'%(classdecl.inherits[0])

        output = output +'\tpublic:\n'

        for enum in classdecl.enums:
            output = output + '\n\t\tenum %s {\n'%(enum.name)
            if len(enum.const_decls):
                output = output + '\t\t\t%s'%(enum.const_decls[0])
                if len(enum.const_decls[1:]):
                    for const_decl in enum.const_decls[1:]:
                        output = output + ',\n\t\t\t%s'%(const_decl)
            output = output + '\n\t\t};\n\n'
           

        # attributes        
        if len(classdecl.attributes)>0:
            includes.append("Attribute")

        attributes=[]

        exclude_attributes=[] # really method that set/get
        try:
            exclude_attributes=interfaces.interfaces[classname].exclude_attributes
        except KeyError:
            pass


        for attr in classdecl.attributes:                
            typename = attr.type.name
            pos = string.find(attr.type.name, 'Animated')
            # undef (and 0)
            if pos>=0 and 0:
                realtypename = "SVG"+attr.type.name[pos+len('Animated'):]
                typename = "Animated< %s >"%(realtypename)                
                if "Animated" not in includes:
                    includes.append("Animated")
                if realtypename in attr_to_include:
                    if realtypename not in includes:
                          includes.append(realtypename)

#            if pos>=0 and 0:
#                realtypename = "SVG"+attr.type.name[pos+len('Animated'):]
#                typename = "Animated%s"%(realtypename)                

            # is Attribute an Element ?
            ispointer = 0
            pos = string.find(attr.type.name, 'Element')
            if pos > 0:
                fwd_decls.append(typename)
                typename = typename+'*'
                ispointer=1
                #if(typename not in used_pointer_attributes):
                #    used_pointer_attributes.append(typename)
                #typename = typename+'p'

            else:

                if attr.type.name=="unsigned short" or string.find(attr.type.name, 'Enumeration')>=0:
                    try:
                        enumname = enum_map.enum_map[classname+'::'+attr.name]
                        if attr.type.name=="unsigned short":
                            typename = enumname
                        else:
                            pos = string.find(attr.type.name, 'Animated')
                            if pos >= 0:
                                typename = 'Animated<%s>'%enumname
                                #typename = 'SVGAnimated%s'%enumname
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
                        print "enum_map.py is not up-to-date. Please remove it, copy and paste what's dumped by this script in enum_map.py then rerun again"
                        print '"'+classname+'::'+attr.name +'": "' + tmp + '" ,'

                elif typename=="boolean":
                    typename="bool"
                else:
                    if typename not in cplusplus.builtin_types:
                        #pass
                        if typename not in includes:
                            includes.append(typename)

                # is it a simple List ?
                if typename=="SVGList":
                    typename="SVGList<>"

                #if typename not in used_attributes:
                #    used_attributes.append(typename)


            
            typestr = 'Attribute< %s >'%(typename)
            #typestr = '%sAttribute'%(typename)
            #if typestr not in includes:
            #    includes.append(typestr)

            attributes.append( (typename, attr.name, attr.type.const, ispointer) )
            # get by name
            try:
                entity_name = mapDtdIdl.attributes_idl_dtd[attr]
                doGetAttrByName=1
            except KeyError:
                pass


        if len(attributes)>0:            
            output = output + '\tprotected:\n'
            # fields
            for (typestr, attrname, const, ispointer) in attributes:
                attrname = cplusplus.make_name(attrname)
                # new scheme
                output = output +'\t\tAttribute< '+typestr+' > ' + attrname + ';\n'
                # output = output +'\t\t'+typestr+' ' + attrname + ';\n'
            output = output + '\tpublic:\n'
            
            # get/set methods
            for (typestr, attrname, const, ispointer) in attributes:
                # get
                # new scheme
                get_ret_str = 'Attribute< ' + typestr + ' >& ' + 'dom_get' + string.upper(attrname[0])+attrname[1:]+'()'
                # get_ret_str = typestr + ' & ' + 'get' + string.upper(attrname[0])+attrname[1:]+'()'

                attrname_cpp = cplusplus.make_name(attrname)
                get_body_str = ' { return %s; }'%attrname_cpp;

                output = output + '\t\tconst ' + get_ret_str + ' const' + get_body_str + '\n'
                if const==0:
                    output = output + '\t\t' + get_ret_str + get_body_str + '\n'

                # set
                # new scheme
                if const==0:
                # if 1:
                    output = output + '\t\tvoid dom_set'+string.upper(attrname[0])+attrname[1:]
                    if ispointer==0:
                        output=output+'(const %s& n) '%(typestr)
                    else:
                        output=output+'(%s n) '%(typestr)

                    if attrname not in ['valueAsString']:
                        output = output + ' { %s.setValue(n); }'%attrname_cpp;
                    else:
                        output = output +' ;'
                    output = output+'\n'
                output=output+'\n'

                

        # new scheme
        if len(attributes)>0:            
            #output = output + '\tprotected:\n'
            # fields
            # for (typestr, attrname, const, ispointer) in attributes:
            #    attrname = cplusplus.make_name(attrname)
                # new scheme
                #output = output +'\t\tAttribute< '+typestr+' > ' + attrname + ';\n'
            #    output = output +'\t\t'+typestr+' ' + attrname + ';\n'
            output = output + '\tpublic:\n'
            
            # get/set methods
            for (typestr, attrname, const, ispointer) in attributes:
                # get
                # new scheme
                # get_ret_str = 'Attribute< ' + typestr + ' >& ' + 'get' + string.upper(attrname[0])+attrname[1:]+'()'
                realtypename = typestr
                pos = string.find(typestr, 'Animated')
                animated = 0
                if pos>=0:
                    if pos==0: # Animated<>
                        pos1 = string.find(typestr, "<")
                        pos2 = string.find(typestr, ">",pos1)
                        realtypename = typestr[pos1+1:pos2]
                    else: # SVGAnimatedTypename
                        realtypename = "SVG"+typestr[pos+len('Animated'):]
                    animated = 1

                methodName = 'get' + string.upper(attrname[0])+attrname[1:]
                if(methodName in exclude_attributes):
                    continue

                ref = ' & '
                if ispointer:
                    ref = ' '
                get_ret_str = realtypename + ref + methodName + '()'

                attrname_cpp = cplusplus.make_name(attrname)
                #get_body_str = ' { return %s; }'%attrname_cpp;
                get_body_str = ' { return %s.getValue(); }'%attrname_cpp;
                if animated:
                    get_body_str = ' { return %s.getValue().getAnimatedVal().getValue(); }'%attrname_cpp;

                output = output + '\t\tconst ' + get_ret_str + ' const' + get_body_str + '\n'
                if const==0:
                    output = output + '\t\t' + get_ret_str + get_body_str + '\n'

                # set
                # new scheme
                # if const==0:
                if 1:
                    methodName = 'set' + string.upper(attrname[0])+attrname[1:]
                    if(methodName in exclude_attributes):
                        continue

                    output = output + '\t\tvoid '+methodName
                    if ispointer==0:
                        #output=output+'(const %s& n) '%(typestr)
                        output=output+'(const %s& n) '%(realtypename)
                    else:
                        output=output+'(%s n) '%(typestr)

                    if attrname not in ['valueAsString']:
                        if animated:
                            output = output + ' { %s.getValue().getAnimatedVal().setValue(n); %s.getValue().getBaseVal().setValue(n); '%(attrname_cpp, attrname_cpp);
                        else:
                            output = output + ' { %s.setValue(n); '%attrname_cpp;
			posElement = string.find(classname, "Element")
			isElement = (posElement > 0) and (len(classname) == (posElement + len("Element")))			
			if isElement:
			    output = output + ' setDamaged(true); '
			output = output + '}'
                    else:
                        output = output +' ;'
                    output = output+'\n'
                output=output+'\n'

    
        try:
            include_attributes=interfaces.interfaces[classname].include_attributes
            for i in include_attributes:
                output = output+i
            
        except KeyError:
            pass


        # methods
        exclude_methods=[]
        try:
            exclude_methods=interfaces.interfaces[classname].exclude_methods
        except KeyError:
            pass
        output = output +'\tpublic:\n'

        for meth in classdecl.methods:
            if meth.return_type.name=='boolean':
                meth.return_type.name='bool'
            if meth.return_type.name not in cplusplus.builtin_types:
                output = output + '\t\tvirtual ' + meth.return_type.name + '* '
                if meth.return_type.name in ["DOMString"]: # confusion between typedef and class
                    includes.append(meth.return_type.name)
                else:
                    if meth.return_type.name not in fwd_decls:
                        fwd_decls.append(meth.return_type.name)
            else:
                output = output + '\t\tvirtual ' + meth.return_type.name + ' '

            output = output + meth.name + ' ( '
            count=0
            for arg in meth.args:
                if count>0:
                    output = output +', '
                if arg.type.name=='boolean':
                    arg.type.name = 'bool'
                if arg.type.name not in cplusplus.builtin_types:
                    if arg.inout == 'in':
                        output = output + 'const %s& %s'%(arg.type.name, arg.name)
                    elif arg.inout == 'inout':
                        output = output + '%s* %s'%(arg.type.name, arg.name)
                    elif arg.inout == 'out':
                        output = output + '%s* %s'%(arg.type.name, arg.name)

                    if arg.type.name in ["DOMString"]: # confusion between typedef and class
                        includes.append(arg.type.name)
                    else:
                        if arg.type.name not in fwd_decls:
                            fwd_decls.append(arg.type.name)
                else:
                    output = output + '%s %s'%(arg.type.name, arg.name)
                count = count+1
                
            output = output  + ' ) ;\n'


            #if "::DOM_Node" not in fwd_decls:
            #    fwd_decls.append("::DOM_Node");

#        if len(classdecl.attributes) or string.find(classdecl.name, "Element")>0:
            #has_dtd_attributes=1

        has_dtd_attributes=find_dtd_attr_in_inherit(classdecl)
        if has_dtd_attributes==1: # or string.find(classdecl.name, "Element")>0:
            output = output + '\t\tvirtual int parse_attributes(const String&, const String&);\n';
            if "String" not in includes:
                includes.append("String")
            output = output + '\t\tvirtual void print_attributes(xmlostream&);\n';
            if "::xmlostream" not in fwd_decls:
                fwd_decls.append("::xmlostream")
            # get by name
            doGetAttrByName=1


        # get by name
        if doGetAttrByName:
            #doDestructor=1
            output = output + '\tpublic:\n\t\tvirtual svgl::AnimateAdapter* getAttributeByName(const String&);\n'
            tmp=["svgl::AnimateAdapter"];
            for i in tmp:
                if i not in fwd_decls:
                    fwd_decls.append(i)
            if "String" not in includes:
                includes.append("String")


        has_destructor = 0
        try:
            has_destructor=interfaces.interfaces[classname].user_defined_destructor
        except KeyError:
            pass

        doDestructor = 1
        if has_destructor==0 and doDestructor:
            output = output + '\n\tpublic:\n\t\tvirtual ~%s();\n'%(classdecl.name)
            destructors.append(classdecl)


        element_string=None
        if mapDtdIdl.elements_idl_dtd.has_key(classdecl):
            element_string=cplusplus.make_name(mapDtdIdl.elements_idl_dtd[classdecl])
            output = output + '\t\tvirtual const String getDtdElement() const { return %s_element_string; }\n'%(classname) #%(element_string)
            #output = output + '\t\tvirtual void glTraverse();\n'

        # if it's an element
        has_constructor = 0
        try:
            has_constructor=interfaces.interfaces[classname].user_defined_constructor
        except KeyError:
            pass
        if has_constructor==0 and string.find(classname, "Element")>0:
            # first find the directly inherited "Element" class
            # generally SVGElement, but it can be  SVGGradientElement for ex.
            element_inherit=None
            for inh in classdecl.inherits:
                if string.find(inh, "Element")>0:
                    element_inherit=inh
                    break
            if element_string:
                output = output + '\t\t%s(dom::Document* doc) : %s(doc, %s_element_string) {}\n'%(classname, element_inherit, classname)
            else:
                output = output + '\t\t%s(dom::Document* doc, unicode::String* tagName) : %s(doc, tagName) {}\n'%(classname, element_inherit)

        #if classdecl.name=="SVGElement" and 0:
            #output = output + '\t\tvirtual const String& getDtdElement() const=0; \n'
            #output = output + '\t\tvirtual void parse(const DOM_Node&, SVGSVGElement* owner, SVGElement* viewport);\n'
            #output = output + '\t\tvirtual void print(xmlostream&);\n';

            #for toto in ["::DOM_Node", "::xmlostream", "SVGSVGElement"]:
                #if toto not in fwd_decls:
                    #fwd_decls.append(toto)
                    
        includes_cpp.append(classdecl.name)


        try:
            fwds = interfaces.interfaces[classname].include_fwd_decls
            for toto in fwds:
                if toto not in fwd_decls:
                    fwd_decls.append(toto)
        except KeyError:
            pass
            
            # includes.append("getNextSVGElement");

        # end struct
        output = output + '\t};\n'

        # end namespace
        output = output + "}\n"


        includestr=""
        for inc in includes:
            if inc==classname:
                continue
            realname = inc
            if string.find(inc, "::")>0:
                realname = string.replace(inc, "::", "/")
            else:
                realname = 'svg/' + realname
            includestr = includestr + "#include <w3c/%s.hpp>\n"%(realname) # JR try to generate correct headers

        try:
            incs = interfaces.interfaces[classname].include_includes
            for i in incs:
                includestr = includestr + "#include <%s>\n"%(i)
        except KeyError:
            pass

        fwd_decls_namespace = { "svg":[] }
        for i in fwd_decls:
            pos = string.find(i, "::")
            nspace = i[:pos]
            if pos>=0:
                if fwd_decls_namespace.has_key(nspace):
                    fwd_decls_namespace[nspace].append(i[pos+2:])
                else:
                    fwd_decls_namespace[nspace]=[i[pos+2:]]
            else:
                if i!='Element':
                    fwd_decls_namespace['svg'].append(i)

        if len(fwd_decls_namespace) > 0:
            fwd_decl_str = ''
            for (nspace, classnames) in fwd_decls_namespace.items():
                if nspace!="":
                    fwd_decl_str = fwd_decl_str + '''
namespace %s {
'''%(nspace)
        
                for i in classnames:
                    fwd_decl_str = fwd_decl_str + '\tclass %s;\n'%i
                if nspace!="":
                    fwd_decl_str = fwd_decl_str + '}\n'

        fwd_element_str=''
        if element_string!=None:
            fwd_element_str = '''
namespace svg {
            extern unicode::String * %s_element_string;
}
'''%(classname)
#%(element_string)
        header.add_content('// generated by genFromIdl.py\n' + fwd_decl_str + '\n'+includestr+fwd_element_str+'\n' + output)
        header.dump(path=config.genDir)

#header = cppHeader.Header("svg")
#content='// generated by genFromIdl.py\n'
#for h in all_headers:
#	content=content+'#include <svg/%s.hpp>\n'%(h)
#header.add_content(content)
#header.dump(path=config.genDir)


if len(destructors):
    includestr=''
    f = GenFile.gfopen(config.genDir+'/destructors.cpp', 'w')
    output_cpp=''
    for i in destructors:
        output_cpp = output_cpp + '   %s::~%s() {}\n'%(i.name, i.name)
        includestr = includestr + '#include<w3c/svg/%s.hpp>\n'%(i.name)

    f.write('// generated by genFromIdl.py\n' + includestr+'''
        namespace svg {
        '''+output_cpp + '''
        }
''')


#includestr='''#include <svg/getNextSVGElement.hpp>
##include <dom/DOM.hpp>
#'''

import makeAnimated
#import makeAttribute
import makeList

for i in used_animated:
    makeAnimated.make(string.replace(i,'SVGAnimated',''))

#for i in used_attributes:
#    makeAttribute.make(i)

#for i in used_pointer_attributes:
#    makeAttribute.make(i, pointer=1)

for i in used_lists:
    name = string.replace(string.replace(i,'List',''),'SVG','')
    as_pointer=0
    if name in ['Transform', 'PathSeg']:
        as_pointer=1
    makeList.make(name, as_pointer)



import makeBasicTypes







