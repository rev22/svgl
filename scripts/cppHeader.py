import os.path
import GenFile

class Header:
	def __init__(self, name, prefix=None):
		self.name = name
		self.prefix = prefix
		
		if self.prefix:
			self.guard = '__'+self.prefix+'_'+self.name+'__'
		else:
			self.guard = '__'+self.name+'__'

		self.content=''

	def add_content(self, content):
		self.content = self.content + content

	def dump(self, path=''):
		data = '''#ifndef %s
#define %s

%s

#endif // %s
'''%(self.guard, self.guard, self.content, self.guard)

		filename = os.path.join(path, self.name+'.hpp')
		f=GenFile.gfopen(filename,'w')
		f.write(data)
