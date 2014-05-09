#
# configure.ac --
#
# Copyright (C) Nicolas Roussel
#
# See the file LICENSE for information on usage and redistribution of
# this file, and for a DISCLAIMER OF ALL WARRANTIES.
#

# See http://www.gnu.org/manual/manual.html

# --------------------------------------------------------

AC_PREREQ([2.69])
AC_INIT([glft],[0.1],[conversy@enac.fr])
AC_CONFIG_AUX_DIR(build/automake/utils)

LT_INIT([disable-static])
AC_SUBST([LIBTOOL_DEPS])

AM_INIT_AUTOMAKE([dist-bzip2])
AC_CONFIG_HEADERS([glft/config.hpp])

AC_CONFIG_SRCDIR([glft/glft.hpp])

AC_CANONICAL_HOST

AC_LANG([C++])
AC_PROG_CXX
AC_PROG_INSTALL

# --------------------------------------------------------
# for setting pkgincludedir in each source subdirectory

GLFT_INCLUDEDIR='${includedir}'/glft
AC_SUBST(GLFT_INCLUDEDIR)

# --------------------------------------------------------

# set to nop to disable silent make mode (we may add --disable-silent if needed)
DISABLE_SILENT=
AC_SUBST(DISABLE_SILENT)

# --------------------------------------------------------

OS_IS_MACOSX=0
OS_IS_CYGWIN=0

case $host in
  *-*-darwin*)
    OS_IS_MACOSX=1
    AC_DEFINE(OS_IS_MACOSX,1,[Mac OS X support])
    OS=MACOSX
   ;; 
  *cygwin*)
    OS_IS_CYGWIN=1
    AC_DEFINE(OS_IS_CYGWIN,1,[Cygwin support])
    OS=CYGWIN
   ;;
  *)
   ;;
esac

AM_CONDITIONAL(OS_IS_MACOSX,[test "x$OS_IS_MACOSX" = "x1"])
AM_CONDITIONAL(OS_IS_CYGWIN,[test "x$OS_IS_CYGWIN" = "x1"])

# --------------------------------------------------------

case $OS in
  MACOSX)
	GLINC=`pwd`/local/include
	(mkdir -p $GLINC/GL && cd $GLINC/GL && ln -fs /System/Library/Frameworks/OpenGL.framework/Headers/* .)
    CPPFLAGS="$CPPFLAGS -I/usr/X11R6/include -I/sw/include -I$GLINC -I/System/Library/Frameworks/AGL.framework/Headers"
    LDFLAGS="$LDFLAGS -L/usr/X11R6/lib -L/sw/lib"
    LIBS="$LIBS -framework OpenGL"
    ;;
  CYGWIN)
    CPPFLAGS="-DWIN32 $CPPFLAGS"
    LIBS="-lglu32 -lopengl32 $LIBS"
    ;;
  *)
    AC_CHECK_HEADERS(GL/gl.h)
    # glut is only used for the demos
    # AC_CHECK_LIB(glut, glutInit)
    AC_CHECK_LIB(GL, glFlush)
    AC_CHECK_LIB(GLU, gluSphere)
    # Potential dependencies...
    AC_CHECK_LIB(Xmu, XmuDrawLogo)
    AC_CHECK_LIB(Xi, XOpenDevice)
    AC_CHECK_LIB(X11, XOpenDisplay)
    ;;
esac

# --------------------------------------------------------

AC_DEFUN([TEST_GLFT_USE_GLLIST],
[AC_ARG_WITH([usegllist],
             AS_HELP_STRING([--with-usegllist],[use gllist (default is YES)]),
             [ac_cv_use_gllist=$withval], [ac_cv_use_gllist=yes])
AC_CACHE_CHECK([whether to use gllist],
               [ac_cv_use_gllist], [ac_cv_use_gllist=yes])])

TEST_GLFT_USE_GLLIST
#echo $ac_cv_use_gllist
if test "$ac_cv_use_gllist" = yes; then
   CPPFLAGS="-DGLFT_USE_GLLIST=1 $CPPFLAGS"
fi

# --------------------------------------------------------

AC_CHECK_PROG(FREETYPE_CONFIG, freetype-config, yes, no)
if test $FREETYPE_CONFIG = yes; then
  # These are in fact CPPFLAGS
  CPPFLAGS="$CPPFLAGS `freetype-config --cflags`"
  # This is a mix of LDFLAGS and LIBS...
  FT_BORDEL=`freetype-config --libs`
  FT_LIBS=""
  FT_LDFLAGS=""
  for word in $FT_BORDEL ; do
    beginning=`echo $word | cut -c -2`
    if test ".$beginning" = ".-L"; then
      FT_LDFLAGS="$FT_LDFLAGS $word"
    else
      FT_LIBS="$FT_LIBS $word"
    fi
  done
  LDFLAGS="$LDFLAGS $FT_LDFLAGS"
  # We don't need these...
  #LIBS="$LIBS -lfreetype $FT_LIBS"
fi

AC_CHECK_HEADER(freetype/config/ftconfig.h,
   ,
   echo "" ; echo "FreeType 2 is required to build glft" ; exit 1)

AC_CHECK_LIB(freetype, FT_Init_FreeType,
   ,
   echo "" ; echo "FreeType 2 is required to build glft" ; exit 1)

# --------------------------------------------------------

AC_CONFIG_FILES(
  Makefile
  glft/Makefile
  demos/Makefile
  glft.pc
  glft-uninstalled.pc
)

AC_OUTPUT
