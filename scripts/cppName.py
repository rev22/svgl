import string

def make_cppname(name):
	out = string.replace(name, '-', '_')
	out = string.replace(out, ':', '_')
	if out in ['class', 'switch', 'operator']:
		out = out+'_'
	return out
