import os, stat, sys
import config

ld_lib = "LD_LIBRARY_PATH=/home/conversy/Work/svgl/.libs:/home/conversy/Archives/gl-si/main/gfx/lib/glu:/home/conversy/Archives/Mesa-3.5/local/lib2:/usr/lib/gcc-lib/i386-linux/3.0/:/home/conversy/Work/sdom/.libs:$LD_LIBRARY_PATH"

testCommand = ld_lib + " " + config.svg2raster_cmd + " -q -d %s %s >& /tmp/svglnonreg.log"%(config.resultImagesDir, "%s")
compareCommand = "diff %s/%s.ppm %s/%s.ppm"%(config.testImagesDir, "%s", config.resultImagesDir, "%s")
diffCommand = "pnmarith -difference %s/%s.ppm %s/%s.ppm > %s/%s.ppm"%(config.testImagesDir, "%s", config.resultImagesDir, "%s", config.diffImagesDir, "%s")


passTestLines = open(config.passTestFileName, 'r').readlines()

problemAtGeneration = []
differs = []

#load passtest file
for line in passTestLines:

    # remove empty and commented lines
    line = line.strip()
    if len(line)==0:
        continue
    if line[0]=='#':
        continue

    # remove comment
    pos = line.find('#')
    if pos>0:
        testname = line[:pos].strip()
        comment = line[pos+1:].strip()
    else:
        testname = line
        comment = None

    #print (testname, comment)

    svgfilename = config.SVGTestSuiteDir+"/"+testname+".svg"

    # generate pixmap
    print 'generating', testname
    res = os.system(testCommand%(svgfilename))
    if res==0:
        print 'generated.'
    else:
        print 'problem at generation. giving up this one.'
        #print open('/tmp/svglnonreg.log','r').read()
        problemAtGeneration.append(testname)
        continue

    # if a previous version exists, compare against it
    # otherwise it becomes the new version
    testImageName = config.testImagesDir+"/"+testname+".ppm"
    resultImageName = config.resultImagesDir+"/"+testname+".ppm"
    try:
        res = os.stat(testImageName)
    except OSError:
        print 'creating', testImageName
        os.system("cp %s %s > /dev/null"%(resultImageName,testImageName))
    
    res = os.system(compareCommand%(testname, testname))
    if res:
        differs.append(testname)
        print 'differs, running diff command'
        os.system(diffCommand%(testname, testname, testname))
        

print '--------'
if len(problemAtGeneration):
    print 'files with a generation problem:'
    for i in problemAtGeneration:
        print i
    print '--------'
        
if len(differs):
    print 'files that differ:'
    for i in differs:
        print i
else:
    print 'no file differs.'
    

if len(problemAtGeneration) or len(differs):
    sys.exit(1)
else:
    sys.exit(0)
    
