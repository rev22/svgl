import string

def make_cpp_string_name(name):
    tmp=name
    tmp = string.replace(tmp,'-','_')
    tmp = string.replace(tmp,':','_')
    return tmp
