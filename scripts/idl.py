class class_decl:
    def __init__(self, name=None, inherits=[], methods=[], enums=[], attributes=[]):
        self.name = name
        self.inherits = inherits
        self.methods = methods
        self.enums = enums
        self.attributes = attributes

    def __str__(self):
        output = '''class %s\n'''%self.name

        output = output + 'inherits:\n'
        for i in self.inherits:
            output = output + '\t' + str(i) +'\n'

        output = output + 'methods:\n'
        for i in self.methods:
            output = output + '\t' + str(i) +'\n'

        output = output + 'enums:\n'
        for i in self.enums:
            output = output + '\t' + str(i) +'\n'

        output = output + 'attributes:\n'
        for i in self.attributes:
            output = output + '\t' + str(i) +'\n'

        return output


class method_decl:
    def __init__(self, return_type=None, name=None, args=[], exceptions=[], class_decl=None):
        self.name = name
        self.return_type = return_type
        self.args=args
        self.class_decl=class_decl
        self.exceptions = exceptions

    def __str__(self):
        output = str(self.return_type) + ' ' + self.name + '( '
        for i in self.args:
            output = output + str(i) + ','
        output = output + ') raises ('
        for i in self.exceptions:
            output = output + str(i)
        output = output + ')'
        return output

class arg_decl:
    def __init__(self, name=None, type=None, inout=None):
        self.name = name
        self.type = type
        self.inout=inout

    def __str__(self):
        output = ''
        if(self.inout):
            output = self.inout
        output = output + ' ' + str(self.type)
        if (self.name):
            output = output + ' ' + self.name
        return output
    
class enum_decl:
    def __init__(self, name='', const_decls=[], class_decl=None):
        self.name = name
        self.const_decls = const_decls
        self.class_decl = class_decl

    def __str__(self):
        output = 'enum ' + self.name + ' {\n'
        for i in self.const_decls:
            output = output + '\t' + str(i) + ',\n'
        output = output + '};'
        return output

class const_decl:
    def __init__(self, name, value):
        self.name = name
        self.value = value

    def __str__(self):
        return str(self.name) + ' = ' + self.value

class type_decl:
    def __init__(self, name, const=0, pointer=0, reference=0):
        self.name = name
        self.const = const
        self.pointer = pointer
        self.reference = reference
        
    def __str__(self):
        output=''
        if self.const==1:
            output = output + 'readonly '
        return output + self.name


