import popen2
import sys
import string
import re
import os

if len(sys.argv)<2:
    raise "usage: pythonv filename"

import_re = re.compile('# ([^\s]+) matches (.+)')

stdout, stdin, stderr = popen2.popen3("python -v %s"%(string.join(sys.argv[1:])))

out = stdout.read()
if len(out):
    print out


depfile = open(".deps/%s.Plo"%(sys.argv[1]),'w')

cwd = os.getcwd()

content = stderr.read()
beg=0
while(1):
    m = import_re.search(content, beg)
    if m==None:
        break
    mod = m.group(2)
    beg = m.end()+1

    if mod[0]!='/':
        mod = cwd+'/'+mod

    depfile.write('*.hpp *.cpp:'+mod+'\n')
    

#if len(out):
#    print out




