import os, os.path

cmd1 = 'sed -e "s/\\(Traverse\\)\\([^(]*\\)(\\(svgl::GLInfo\\)/\\1\\2(svgl::Context * svglContext, \\3/" %s | '
cmd2 = 'sed -e "s/\\(Traverse\\)\\([^(]*\\)(\\(&glinfonew\\)/\\1\\2(svglContext, \\3/" | '
cmd3 = 'sed -e "s/\\(Traverse\\)\\([^(]*\\)(\\(glinfo\\)/\\1\\2(svglContext, \\3/" > %s'

cmd=cmd1+cmd2+cmd3

src_dir = 'src.sav/'
dst_dir = 'src/'

files = os.listdir(src_dir)

for filename in files:
    prefix, ext = os.path.splitext(filename)
    if ext=='.cpp':
        os.system(cmd%(src_dir+filename, dst_dir+filename))
