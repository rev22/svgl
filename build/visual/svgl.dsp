# Microsoft Developer Studio Project File - Name="svgl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=svgl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "svgl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "svgl.mak" CFG="svgl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "svgl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "svgl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "svgl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "../zlib" /I "../libpng" /I "../freetype/include" /I "gen" /I "include/w3c" /I "include" /I "../xerces-c-src1_4_0/src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "svgl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W1 /Gm /GR /GX /ZI /Od /I "../zlib" /I "../libpng" /I "../freetype/include" /I "gen" /I "include/w3c" /I "include" /I "../sdom/include" /I "../sdom/include/gc" /I "../sdom/include/utils" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "svgl - Win32 Release"
# Name "svgl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\svgl\AnimationManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\bezierpath.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\BezierPathDistanceIterator.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\BezierPathGlDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\BezierPathLength.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\BoundingBoxManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\colors.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\css\CSSRGBColor.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\css\CSSStyle.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\css\CSSURI.cpp
# End Source File
# Begin Source File

SOURCE=.\gen\src\destructors.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\DisplayManager.cpp
# End Source File
# Begin Source File

SOURCE=.\gen\src\ElementGetAttributeByNameMethods.cpp
# End Source File
# Begin Source File

SOURCE=.\gen\src\ElementParseAttributesMethods.cpp
# End Source File
# Begin Source File

SOURCE=.\gen\src\ElementPrintMethods.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\ExternalEntityManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\FontManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\glft.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\GLInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\gluEllipse.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\gluShadedDisk.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\ImageManager.cpp
# End Source File
# Begin Source File

SOURCE=.\gen\src\makeEnumType.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\MultiPassManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\parseHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\parsePathSegList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\parsePoints.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\parser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\parseTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\PickManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\dom\string.cpp

!IF  "$(CFG)" == "svgl - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "svgl - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\w3c\dom\string_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\strings.cpp

!IF  "$(CFG)" == "svgl - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "svgl - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGAltGlyphElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGAnimateColorElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGAnimateElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGAnimateMotionElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGAnimateTransformElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGAnimationElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGCircleElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGClipPathElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGDefsElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGEllipseElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGFEBlendElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGFEColorMatrixElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGFEGaussianBlurElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGFilterElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGFilterPrimitiveStandardAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGGElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGGradientElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGImageElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGLength.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGLinearGradientElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGLineElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGMarkerElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGPathElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGPathSeg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGPatternElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGPolygonElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGPolylineElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGPreserveAspectRatio.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGRadialGradientElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGRectElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGStylable.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGSVGElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGSymbolElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGTests.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGTextContentElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGTextElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGTextPathElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGTransformable.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGTransforms.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGTRefElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGTSpanElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w3c\svg\SVGUseElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\TextureSnapshot.cpp
# End Source File
# Begin Source File

SOURCE=.\src\svgl\TimeManager.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\svgl\AnimateAdapter.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\AnimateAdapterColor.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\AnimateAdapterDouble.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\Animated.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\AnimationInfo.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\AnimationManager.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\Attribute.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\bezier.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\bezierpath.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\BezierPathDistanceIterator.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\BezierPathLength.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\BoundingBoxManager.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\Context.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\css\CSSRule.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\css\CSSStyleDeclaration.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\css\CSSURI.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\css\CSSValue.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\css\CurrentColor.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\debug.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\dom\Document.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\Document.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\css\DocumentCSS.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\events\DocumentEvent.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\DOMString.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\dom\Element.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\Element.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\events\Event.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\events\EventTarget.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\ExternalEntityManager.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\FontManager.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\getNextSVGElement.hpp
# End Source File
# Begin Source File

SOURCE=.\include\glft.h
# End Source File
# Begin Source File

SOURCE=.\include\svgl\GLInfo.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\gluEllipse.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\gluShadedDisk.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\ImageManager.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\Integer.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\KeyFrame.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\MultiPassManager.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\dom\Node.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\dom\NodeList.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\NodeList.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\parseHelper.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\parser.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\parseTransform.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\PickManager.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\point.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\point2d.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\printHelper.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\css\RGBColor.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\StopElements.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\dom\string.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\String.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\dom\string_helpers.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\strings.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\css\style_defs.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\svg.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\SVGList.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\SVGPCDATAElement.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\SVGString.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\svg\SVGTransforms.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\TextInfo.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\TextureSnapshot.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\TimeManager.hpp
# End Source File
# Begin Source File

SOURCE=.\include\svgl\timeval_helpers.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\events\UIEvent.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\css\URIColor.hpp
# End Source File
# Begin Source File

SOURCE=.\include\w3c\css\ViewCSS.hpp
# End Source File
# Begin Source File

SOURCE=.\include\xmlostream.hpp
# End Source File
# End Group
# End Target
# End Project
