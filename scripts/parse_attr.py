import re, string
import cppName

parenthesis_re = re.compile('\(|\)')

def get_parenthesis_expr(content, pos=0):
	depth=1
	while depth!=0:
		m = parenthesis_re.search(content, pos)
		if m==None:
			raise "pas bon "+defi[begdef:]
		if m.group()=='(':
			depth=depth+1
		else:
			depth=depth-1
		pos = m.end()
	return pos



#------------
# attr_type

#returns (implied, required, fixed, defaultvalue)

class attr_type:
	pass

class attr_type_implied(attr_type):
	def __str__(self):
		return "#IMPLIED"

class attr_type_required(attr_type):
	def __str__(self):
		return "#REQUIRED"

class attr_type_fixed(attr_type):
	def __init__(self, default_value):
		self.default_value = default_value

	def __str__(self):
		return "#FIXED '" + self.default_value + "'"

class attr_type_default_value(attr_type):
	def __init__(self, default_value):
		self.default_value = default_value

	def __str__(self):
		return "'" + self.default_value + "'"





class attribute_def:
	def __init__(self):
		pass

	def expand(self, entity_type_decls={}, entity_common_attrs={} ):
		return [self]


class attr_entity_ref (attribute_def):
	def __init__(self, entity_name):
		self.entity_name = entity_name
		self.expandlist=None

	def __str__(self):
		return '%' + self.entity_name + ';'

	def expand(self, entity_type_decls={}, entity_common_attrs={} ):
		
		if entity_type_decls.has_key(self.entity_name):
			return entity_type_decls[self.entity_name]
		else:
			entities = entity_common_attrs[self.entity_name]
			l=[]
			for i in entities:
				l.extend(i.expand(entity_type_decls, entity_common_attrs))
			return l


class attr_named (attribute_def):
	def __init__(self, name):
		attribute_def.__init__(self)
		self.name = name
		self.type = None
		
	def __str__(self):
		return self.name + ' (' + str(self.type) + ') '

class attr_named_entity_ref (attr_named, attr_entity_ref):
	def __init__(self, name, entity_name):
		attr_named.__init__(self, name)
		attr_entity_ref.__init__(self, entity_name)

	def __str__(self):
		return attr_named.__str__(self) + ' ' + attr_entity_ref.__str__(self)


class attr_named_simple_type (attr_named):
	def __init__(self, name, simple_type):
		attr_named.__init__(self, name)
		self.simple_type = simple_type

	def __str__(self):
		return attr_named.__str__(self) + ' ' + self.simple_type

	def cppheader(self):
		return '''
#include <w3c/dom/string.hpp>

namespace svg {
	typedef dom::string %s;
}
'''%self.name



class attr_named_enum (attr_named):
	def __init__(self, name, enums):
		attr_named.__init__(self, name)
		self.enums = enums

	def __str__(self):
		return attr_named.__str__(self) + ' ' + str(self.enums)

	def cppheader(self):
		output= '''
namespace svg {
	struct %s {
		enum %sEnum {
'''%(self.name, self.name)
		for i in self.enums:
			output = output+'\t\t\t%s,\n'%(self.name+'_'+cppName.make_cppname(i))
		output = output + '''\t\t};
	};
}
'''
		return output

class attr_expr (attr_named):
	def __init__(self, name, expr):
		attr_named.__init__(self, name)
		self.expr = expr
	
	def __str__(self):
		return attr_named.__str__(self) + ' ' + self.expr



implied_re = re.compile('#IMPLIED')
required_re = re.compile('#REQUIRED')
fixed_re = re.compile('#FIXED')
default_value_re = re.compile("['\"]([^'\"]*)['|\"]")

#returns (implied, required, fixed, defaultvalue)
def get_attr_typedef(content):
	m = implied_re.search(content)
	if m:
		return attr_type_implied()
	else:
		m = required_re.search(content)
		if m:
			return attr_type_required()
		else:
			m = fixed_re.search(content)
			if m:
				n = default_value_re.search(content, m.end())
				return attr_type_fixed(n.group(1))
			else:
				m = default_value_re.search(content)
				if m:
					return attr_type_default_value(m.group(1))
				else:
					raise "error attr type", content


def get_enums(content, beg=0):
	pos=get_parenthesis_expr(content, beg)
	tmp=string.split(content[beg:pos-1],'|')
	enums=[]
	for i in tmp:
		enums.append(string.strip(i))
	#print enums
	return enums, pos


name_re = re.compile('\s*((\w|:|-|_)+)\s*')
entity_ref_re = re.compile('\s*%([\w\-_]+);\s*')
eat_re = re.compile('[^\n]+\n?', re.MULTILINE)


def parse_attr(defi):
	defilen=len(defi)
	begdefi=0

	attributes=[]

	while begdefi<defilen:
		# check if entity ref %blah;
		m = entity_ref_re.match(defi, begdefi)
		if m:
			entity_ref = m.group(1)
			begdefi = m.end()
			#print entity_ref
			attributes.append(attr_entity_ref(entity_ref))
			continue

		# otherwise it's maybe a 'name type default'
		m = name_re.match(defi, begdefi)
		if m:
			attrname = m.group(1)
			#print attrname
			begdefi = m.end()
			# is it a 'name %blah; something' ?
			m = entity_ref_re.match(defi, begdefi)
			if m:
				entity_ref = m.group(1)
				#print entity_ref
				theattr=attr_named_entity_ref(attrname, entity_ref)
				attributes.append(theattr)
				begdefi = m.end()
			else:
				# is it a 'name CDATA something' ?
				m=name_re.match(defi, begdefi)
				if m:
					thetype = m.group(1)
					#print thetype
					theattr = attr_named_simple_type(attrname, thetype)
					attributes.append(theattr)
					begdefi=m.end()
				else:
					# expression with parenthesis
					if defi[begdefi]=='(':
						enums, pos = get_enums(defi, begdefi+1)
						begdefi=pos
						#print enums
						theattr = attr_named_enum(attrname, enums)
						attributes.append(theattr)

					else:
						raise "aie"+defi[begdefi:]
			m = eat_re.match(defi, begdefi)
			theattrtype = get_attr_typedef(m.group())				
			theattr.type = theattrtype
			begdefi=m.end()+1

		else:
			# so it must be an (expr)
			if defi[begdefi]=='(':
				pos=get_parenthesis_expr(defi, begdefi+1)
#				enums, pos = get_enums(defi, begdefi+1)
				begdefi=pos
			else:		
				raise "ouie"+defi[begdefi:]
			if (len(defi[begdefi:])):
				m = eat_re.match(defi, begdefi)
				#print len(defi[begdefi:])
				begdefi=m.end()+1

	return attributes



