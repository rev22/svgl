import cPickle

f=open("pickledbs/class_decls.cPickle", 'r')
class_decls = cPickle.load(f)


if __name__=='__main__':
	if len(class_decls):
		for name, i in class_decls.items():
			print name



