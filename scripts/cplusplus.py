import string

builtin_types = ["float", "double", "int", "void", "unsigned long", "bool", "long", "unsigned short"]
keywords = ["operator"]

def make_name(name):
    tmp=name
    tmp = string.replace(tmp,'-','_')
    tmp = string.replace(tmp,':','_')
    if tmp in keywords:
        tmp = tmp + '_'
    return tmp
