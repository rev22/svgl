import os

#svgl_home = "@svgl_srcdir@"
#build_dir = "@svgl_builddir@"

#svgl_home = "../../src/svgl"
#build_dir = "/Users/conversy/build/xcode"
svgl_home=os.getenv("SVGL_HOME")
build_dir=os.getenv("SVGL_BUILD_DIR")

#print svgl_home
#print build_dir

genDir = build_dir
genSrcDir = genDir+"/svgl.build/preall.build/DerivedSources/svg"
genSvgDir = genDir+"/local/include/svg"

# non-regression test
passTestFileName = svgl_home+"/share/passtest"
SVGTestSuiteDir = "/home/conversy/Archives/svgtests" 
testImagesDir = svgl_home+"/tests/non-regression"
resultImagesDir = svgl_home+"/tests/results"
diffImagesDir = svgl_home+"/tests/diffs"
svg2raster_cmd = svgl_home+"/svg2raster"
