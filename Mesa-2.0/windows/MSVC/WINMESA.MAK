# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "winmesa.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
CPP=cl.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/winmesa.lib $(OUTDIR)/winmesa.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /I "..\..\include" /I "\wing\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WIN32__" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /I "..\..\include" /I "\wing\include" /D\
 "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WIN32__" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"winmesa.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"winmesa.bsc" 
BSC32_SBRS= \
	$(INTDIR)/span.sbr \
	$(INTDIR)/scissor.sbr \
	$(INTDIR)/light.sbr \
	$(INTDIR)/stencil.sbr \
	$(INTDIR)/points.sbr \
	$(INTDIR)/draw.sbr \
	$(INTDIR)/alpha.sbr \
	$(INTDIR)/drawpix.sbr \
	$(INTDIR)/lines.sbr \
	$(INTDIR)/attrib.sbr \
	$(INTDIR)/readpix.sbr \
	$(INTDIR)/pixel.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/accum.sbr \
	$(INTDIR)/pb.sbr \
	$(INTDIR)/bitmap.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/context.sbr \
	$(INTDIR)/vertex.sbr \
	$(INTDIR)/copypix.sbr \
	$(INTDIR)/logic.sbr \
	$(INTDIR)/interp.sbr \
	$(INTDIR)/blend.sbr \
	$(INTDIR)/fog.sbr \
	$(INTDIR)/polygons.sbr \
	$(INTDIR)/enable.sbr \
	$(INTDIR)/wmesa.sbr \
	$(INTDIR)/get.sbr \
	$(INTDIR)/texture.sbr \
	$(INTDIR)/clip.sbr \
	$(INTDIR)/bresenhm.sbr \
	$(INTDIR)/depth.sbr \
	$(INTDIR)/feedback.sbr \
	$(INTDIR)/eval2.sbr \
	$(INTDIR)/xform.sbr \
	$(INTDIR)/dd.sbr \
	$(INTDIR)/vb.sbr

$(OUTDIR)/winmesa.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"winmesa.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/span.obj \
	$(INTDIR)/scissor.obj \
	$(INTDIR)/light.obj \
	$(INTDIR)/stencil.obj \
	$(INTDIR)/points.obj \
	$(INTDIR)/draw.obj \
	$(INTDIR)/alpha.obj \
	$(INTDIR)/drawpix.obj \
	$(INTDIR)/lines.obj \
	$(INTDIR)/attrib.obj \
	$(INTDIR)/readpix.obj \
	$(INTDIR)/pixel.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/accum.obj \
	$(INTDIR)/pb.obj \
	$(INTDIR)/bitmap.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/context.obj \
	$(INTDIR)/vertex.obj \
	$(INTDIR)/copypix.obj \
	$(INTDIR)/logic.obj \
	$(INTDIR)/interp.obj \
	$(INTDIR)/blend.obj \
	$(INTDIR)/fog.obj \
	$(INTDIR)/polygons.obj \
	$(INTDIR)/enable.obj \
	$(INTDIR)/wmesa.obj \
	$(INTDIR)/get.obj \
	$(INTDIR)/texture.obj \
	$(INTDIR)/clip.obj \
	$(INTDIR)/bresenhm.obj \
	$(INTDIR)/depth.obj \
	$(INTDIR)/feedback.obj \
	$(INTDIR)/eval2.obj \
	$(INTDIR)/xform.obj \
	$(INTDIR)/dd.obj \
	$(INTDIR)/vb.obj \
	$(INTDIR)/alphabuf.obj \
	$(INTDIR)/masking.obj

$(OUTDIR)/winmesa.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/winmesa.lib $(OUTDIR)/winmesa.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Z7 /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Z7 /YX /Od /I "..\..\include" /I "\wing\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__WIN32__" /FR /c
CPP_PROJ=/nologo /W3 /GX /Z7 /YX /Od /I "..\..\include" /I "\wing\include" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__WIN32__" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"winmesa.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinDebug/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"winmesa.bsc" 
BSC32_SBRS= \
	$(INTDIR)/span.sbr \
	$(INTDIR)/scissor.sbr \
	$(INTDIR)/light.sbr \
	$(INTDIR)/stencil.sbr \
	$(INTDIR)/points.sbr \
	$(INTDIR)/draw.sbr \
	$(INTDIR)/alpha.sbr \
	$(INTDIR)/drawpix.sbr \
	$(INTDIR)/lines.sbr \
	$(INTDIR)/attrib.sbr \
	$(INTDIR)/readpix.sbr \
	$(INTDIR)/pixel.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/accum.sbr \
	$(INTDIR)/pb.sbr \
	$(INTDIR)/bitmap.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/context.sbr \
	$(INTDIR)/vertex.sbr \
	$(INTDIR)/copypix.sbr \
	$(INTDIR)/logic.sbr \
	$(INTDIR)/interp.sbr \
	$(INTDIR)/blend.sbr \
	$(INTDIR)/fog.sbr \
	$(INTDIR)/polygons.sbr \
	$(INTDIR)/enable.sbr \
	$(INTDIR)/wmesa.sbr \
	$(INTDIR)/get.sbr \
	$(INTDIR)/texture.sbr \
	$(INTDIR)/clip.sbr \
	$(INTDIR)/bresenhm.sbr \
	$(INTDIR)/depth.sbr \
	$(INTDIR)/feedback.sbr \
	$(INTDIR)/eval2.sbr \
	$(INTDIR)/xform.sbr \
	$(INTDIR)/dd.sbr \
	$(INTDIR)/vb.sbr

$(OUTDIR)/winmesa.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"winmesa.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/span.obj \
	$(INTDIR)/scissor.obj \
	$(INTDIR)/light.obj \
	$(INTDIR)/stencil.obj \
	$(INTDIR)/points.obj \
	$(INTDIR)/draw.obj \
	$(INTDIR)/alpha.obj \
	$(INTDIR)/drawpix.obj \
	$(INTDIR)/lines.obj \
	$(INTDIR)/attrib.obj \
	$(INTDIR)/readpix.obj \
	$(INTDIR)/pixel.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/accum.obj \
	$(INTDIR)/pb.obj \
	$(INTDIR)/bitmap.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/context.obj \
	$(INTDIR)/vertex.obj \
	$(INTDIR)/copypix.obj \
	$(INTDIR)/logic.obj \
	$(INTDIR)/interp.obj \
	$(INTDIR)/blend.obj \
	$(INTDIR)/fog.obj \
	$(INTDIR)/polygons.obj \
	$(INTDIR)/enable.obj \
	$(INTDIR)/wmesa.obj \
	$(INTDIR)/get.obj \
	$(INTDIR)/texture.obj \
	$(INTDIR)/clip.obj \
	$(INTDIR)/bresenhm.obj \
	$(INTDIR)/depth.obj \
	$(INTDIR)/feedback.obj \
	$(INTDIR)/eval2.obj \
	$(INTDIR)/xform.obj \
	$(INTDIR)/dd.obj \
	$(INTDIR)/vb.obj \
	$(INTDIR)/alphabuf.obj \
	$(INTDIR)/masking.obj

$(OUTDIR)/winmesa.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=\Mesa\src\span.c
DEP_SPAN_=\
	\Mesa\src\alpha.h\
	\Mesa\src\blend.h\
	\Mesa\src\context.h\
	\Mesa\src\depth.h\
	\Mesa\src\dd.h\
	\Mesa\src\fog.h\
	\Mesa\src\logic.h\
	\Mesa\src\macros.h\
	\Mesa\src\scissor.h\
	\Mesa\src\span.h\
	\Mesa\src\stencil.h\
	\Mesa\src\texture.h\
	c:\MSdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/span.obj :  $(SOURCE)  $(DEP_SPAN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\scissor.c
DEP_SCISS=\
	\Mesa\src\context.h\
	\Mesa\src\macros.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/scissor.obj :  $(SOURCE)  $(DEP_SCISS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\light.c
DEP_LIGHT=\
	\Mesa\src\context.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\xform.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/light.obj :  $(SOURCE)  $(DEP_LIGHT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\stencil.c
DEP_STENC=\
	\Mesa\src\context.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\pb.h\
	\Mesa\src\stencil.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/stencil.obj :  $(SOURCE)  $(DEP_STENC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\points.c
DEP_POINT=\
	\Mesa\src\context.h\
	\Mesa\src\dd.h\
	\Mesa\src\feedback.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\pb.h\
	\Mesa\src\span.h\
	\Mesa\src\vb.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/points.obj :  $(SOURCE)  $(DEP_POINT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\draw.c
DEP_DRAW_=\
	\Mesa\src\clip.h\
	\Mesa\src\context.h\
	\Mesa\src\draw.h\
	\Mesa\src\feedback.h\
	\Mesa\src\fog.h\
	\Mesa\src\light.h\
	\Mesa\src\lines.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\pb.h\
	\Mesa\src\points.h\
	\Mesa\src\polygons.h\
	\Mesa\src\texture.h\
	\Mesa\src\vb.h\
	\Mesa\src\xform.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/draw.obj :  $(SOURCE)  $(DEP_DRAW_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\alpha.c
DEP_ALPHA=\
	\Mesa\src\context.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/alpha.obj :  $(SOURCE)  $(DEP_ALPHA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\drawpix.c
DEP_DRAWP=\
	\Mesa\src\context.h\
	\Mesa\src\feedback.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\pixel.h\
	\Mesa\src\span.h\
	\Mesa\src\stencil.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/drawpix.obj :  $(SOURCE)  $(DEP_DRAWP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\lines.c
DEP_LINES=\
	\Mesa\src\bresenhm.h\
	\Mesa\src\context.h\
	\Mesa\src\dd.h\
	\Mesa\src\feedback.h\
	\Mesa\src\interp.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\pb.h\
	\Mesa\src\vb.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/lines.obj :  $(SOURCE)  $(DEP_LINES) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\attrib.c
DEP_ATTRI=\
	\Mesa\src\context.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/attrib.obj :  $(SOURCE)  $(DEP_ATTRI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\readpix.c
DEP_READP=\
	\Mesa\src\context.h\
	\Mesa\src\depth.h\
	\Mesa\src\dd.h\
	\Mesa\src\feedback.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\span.h\
	\Mesa\src\stencil.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/readpix.obj :  $(SOURCE)  $(DEP_READP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\pixel.c
DEP_PIXEL=\
	\Mesa\src\context.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\span.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/pixel.obj :  $(SOURCE)  $(DEP_PIXEL) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\misc.c
DEP_MISC_=\
	\Mesa\src\accum.h\
	\Mesa\src\context.h\
	\Mesa\src\depth.h\
	\Mesa\src\dd.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\stencil.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\accum.c
DEP_ACCUM=\
	\Mesa\src\context.h\
	\Mesa\src\dd.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/accum.obj :  $(SOURCE)  $(DEP_ACCUM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\pb.c
DEP_PB_C1c=\
	\Mesa\src\alpha.h\
	\Mesa\src\blend.h\
	\Mesa\src\context.h\
	\Mesa\src\dd.h\
	\Mesa\src\depth.h\
	\Mesa\src\fog.h\
	\Mesa\src\logic.h\
	\Mesa\src\pb.h\
	\Mesa\src\scissor.h\
	\Mesa\src\stencil.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/pb.obj :  $(SOURCE)  $(DEP_PB_C1c) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\bitmap.c
DEP_BITMA=\
	\Mesa\src\context.h\
	\Mesa\src\feedback.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\pb.h\
	\Mesa\src\pixel.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/bitmap.obj :  $(SOURCE)  $(DEP_BITMA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\list.c
DEP_LIST_=\
	\Mesa\src\accum.h\
	\Mesa\src\attrib.h\
	\Mesa\src\bitmap.h\
	\Mesa\src\context.h\
	\Mesa\src\copypix.h\
	\Mesa\src\draw.h\
	\Mesa\src\drawpix.h\
	\Mesa\src\enable.h\
	\Mesa\src\eval.h\
	\Mesa\src\feedback.h\
	\Mesa\src\fog.h\
	\Mesa\src\light.h\
	\Mesa\src\logic.h\
	\Mesa\src\macros.h\
	\Mesa\src\pixel.h\
	\Mesa\src\texture.h\
	\Mesa\src\xform.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/list.obj :  $(SOURCE)  $(DEP_LIST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\context.c
DEP_CONTE=\
	\Mesa\src\context.h\
	\Mesa\src\dd.h\
	\Mesa\src\eval.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\pb.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/context.obj :  $(SOURCE)  $(DEP_CONTE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\vertex.c
DEP_VERTE=\
	\Mesa\src\context.h\
	\Mesa\src\draw.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/vertex.obj :  $(SOURCE)  $(DEP_VERTE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\copypix.c
DEP_COPYP=\
	\Mesa\src\context.h\
	\Mesa\src\dd.h\
	\Mesa\src\depth.h\
	\Mesa\src\feedback.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\span.h\
	\Mesa\src\stencil.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/copypix.obj :  $(SOURCE)  $(DEP_COPYP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\logic.c
DEP_LOGIC=\
	\Mesa\src\context.h\
	\Mesa\src\dd.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\pb.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/logic.obj :  $(SOURCE)  $(DEP_LOGIC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File
#
#SOURCE=\Mesa\src\quikpoly.c
#DEP_QUIKP=\
#	C:\msdev\INCLUDE\gl\GL.H\
#	\Mesa\src\config.h\
#	\Mesa\src\dd.h
#
#$(INTDIR)/quikpoly.obj :  $(SOURCE)  $(DEP_QUIKP) $(INTDIR)
#   $(CPP) $(CPP_PROJ)  $(SOURCE) 
#
# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\interp.c
DEP_INTER=\
	\Mesa\src\context.h\
	\Mesa\src\macros.h\
	\Mesa\src\interp.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/interp.obj :  $(SOURCE)  $(DEP_INTER) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\blend.c
DEP_BLEND=\
	\Mesa\src\context.h\
	\Mesa\src\dd.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\pb.h\
	\Mesa\src\span.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/blend.obj :  $(SOURCE)  $(DEP_BLEND) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\fog.c
DEP_FOG_C=\
	\Mesa\src\context.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/fog.obj :  $(SOURCE)  $(DEP_FOG_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File

# Begin Source File

SOURCE=\Mesa\src\polygons.c
DEP_POLYG=\
	\Mesa\src\context.h\
	\Mesa\src\dd.h\
	\Mesa\src\feedback.h\
	\Mesa\src\interp.h\
	\Mesa\src\lines.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\points.h\
	\Mesa\src\span.h\
	\Mesa\src\vb.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/polygons.obj :  $(SOURCE)  $(DEP_POLYG) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\enable.c
DEP_ENABL=\
	\Mesa\src\context.h\
	\Mesa\src\depth.h\
	\Mesa\src\draw.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\stencil.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/enable.obj :  $(SOURCE)  $(DEP_ENABL) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\wmesa.c
DEP_WMESA=\
	\Mesa\include\GL\wmesa.h\
	\Mesa\src\bresenhm.h\
	\Mesa\src\context.h\
	\Mesa\src\dd.h\
#	\Mesa\src\quikpoly.h\
	\Mesa\src\xform.h\
	\WING\INCLUDE\WING.H\
	\Mesa\src\vb.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/wmesa.obj :  $(SOURCE)  $(DEP_WMESA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\get.c
DEP_GET_C=\
	\Mesa\src\context.h\
	\Mesa\src\dd.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/get.obj :  $(SOURCE)  $(DEP_GET_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\texture.c
DEP_TEXTU=\
	\Mesa\src\context.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\pb.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/texture.obj :  $(SOURCE)  $(DEP_TEXTU) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\clip.c
DEP_CLIP_=\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\clip.h\
	\Mesa\src\context.h\
	\Mesa\src\macros.h\
	\Mesa\src\vb.h\
	\Mesa\src\xform.h\
	\Mesa\src\config.h

$(INTDIR)/clip.obj :  $(SOURCE)  $(DEP_CLIP_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\bresenhm.c
DEP_BRESE=\
	\Mesa\src\context.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/bresenhm.obj :  $(SOURCE)  $(DEP_BRESE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\depth.c
DEP_DEPTH=\
	\Mesa\src\context.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/depth.obj :  $(SOURCE)  $(DEP_DEPTH) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\feedback.c
DEP_FEEDB=\
	\Mesa\src\context.h\
	\Mesa\src\feedback.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/feedback.obj :  $(SOURCE)  $(DEP_FEEDB) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\eval2.c
DEP_EVAL2=\
	\Mesa\src\context.h\
	\Mesa\src\draw.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/eval2.obj :  $(SOURCE)  $(DEP_EVAL2) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\xform.c
DEP_XFORM=\
	\Mesa\src\context.h\
	\Mesa\src\depth.h\
	\Mesa\src\dd.h\
	\Mesa\src\list.h\
	\Mesa\src\macros.h\
	\Mesa\src\stencil.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/xform.obj :  $(SOURCE)  $(DEP_XFORM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\dd.c
DEP_DD_C4a=\
	\Mesa\src\dd.h\
	C:\msdev\INCLUDE\gl\GL.H

$(INTDIR)/dd.obj :  $(SOURCE)  $(DEP_DD_C4a) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\vb.c
DEP_VB_C4c=\
	\Mesa\src\vb.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/vb.obj :  $(SOURCE)  $(DEP_VB_C4c) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\alphabuf.c
DEP_VB_C4c=\
	\Mesa\src\context.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\config.h

$(INTDIR)/alphabuf.obj :  $(SOURCE)  $(DEP_VB_C4c) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Mesa\src\masking.c
DEP_VB_C4c=\
	\Mesa\src\alphabuf.h\
	C:\msdev\INCLUDE\gl\GL.H\
	\Mesa\src\dd.h\
	\Mesa\src\context.h\
	\Mesa\src\macros.h\
	\Mesa\src\masking.h\
	\Mesa\src\pb.h

$(INTDIR)/masking.obj :  $(SOURCE)  $(DEP_VB_C4c) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
# End Group
# End Project
################################################################################
