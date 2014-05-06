SVGL is a library that displays SVG documents using OpenGL.

It is a direct SVG -> OpenGL rendering engine written in C++.

The absence of an intermediate raster step makes it very performant.

## Authors and licensing ##

Copyright (c) 2000-2004 Stephane Conversy <conversy@enac.fr>, http://www.tls.cena.fr/~conversy

Copyright (c) 2000-2004 Jean-Daniel Fekete <fekete@inria.fr>, http://www.lri.fr/~conversy

- see license.txt for the GNU LGPL license
- see docs/agglicense.txt for agg license

## Building and installation ##

### building ###

A build script for Unix/Linux is available in build/build.sh; simply execute it after unpacking.

The following software needs to be installed for the build process:

- a C++ compiler, for example g++ or clang
- autoconf
- xutils
- automake
- libtool
- libexpat1
- libpng12
- pkg-config

### testing ###

After compiling, you can test the library using the tests available in `demos/tutorial`, for example

   $ demos/tutorial/00simplest

Many of the tests require you to set the GLFT_FONTDIR variable to to a dir containing compatible TrueType font files.

For example, in a Debian system with `ttf-liberation` installed:

   $ env GLFT_FONTDIR=/usr/share/fonts/truetype/ttf-liberation/ demos/tutorial/05text

### installation ###

   $ make install

See docs/INSTALL for further explanations

### documentation ###

See docs/* files.

## History ##

Part of svgl was developed during SC's post-doc period at
the Ecole des Mines de Nantes (http://www.emn.fr)
from October 2000 to September 2001.

Stephane Conversy is now at Ecole Nationale de l'Aviation Civile
and Centre d'Etudes de la Navigation Aerienne, Toulouse, France

Jean-Daniel Fekete is now at the
Laboratoire de Recherche en Informatique,
Universite Paris-Sud an INRIA, Orsay, France

The software was further developed until 2004.

In 2013 Michele Bini tweaked the code for modern Unix/Linux
systems and modern C++ compilers and published the update on GitHub.

### Contributors ###

Important contributions were made by:
- Nicolas Roussel
- Renaud Blanch
- Jean-Rene Courtois
- Michele Bini <michele.bini@gmail.com>
