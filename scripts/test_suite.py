import os
import popen2

HOME = '/home/conversy'
#TEST_SUITE_DIR = HOME+'/Archives/svgtests'
TEST_SUITE_DIR = HOME+'/Archives/svgtoolkit-20001010/samples'


lfiles = os.listdir(TEST_SUITE_DIR)

tmpfile = '/tmp/conversysvgtest'

excludes = ['SVGAnimat', 'SVGSVGElement::xmlns', 'SVGTitleElement::content', 'SVGDescElement::content', 'xmlns:xlink']
filter = ' && cat ' + tmpfile
for i in excludes:
    filter = filter + ' | grep -v "%s" '%i


for filename in lfiles:
    prefix, ext = os.path.splitext(filename)
    if ext=='.svg':
        longname = TEST_SUITE_DIR+ '/' + filename
        print longname
        cmd = "./svgtest " + longname + ' 2>' + tmpfile + filter
        #print cmd
        stdout, stdin, stderr = popen2.popen3(cmd)
        print stdout.read()

