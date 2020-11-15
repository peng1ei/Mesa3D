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
!MESSAGE NMAKE /f "glaux.mak" CFG="Win32 Debug"
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

ALL : $(OUTDIR)/glaux.lib $(OUTDIR)/glaux.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /I "\wing\include" /I "..\..\include" /I "..\..\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WIN32__" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /I "\wing\include" /I "..\..\include" /I\
 "..\..\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WIN32__" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"glaux.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"glaux.bsc" 
BSC32_SBRS= \
	$(INTDIR)/image.sbr \
	$(INTDIR)/shapes.sbr \
	$(INTDIR)/teapot.sbr \
	$(INTDIR)/glaux.sbr \
	$(INTDIR)/vect3d.sbr \
	$(INTDIR)/xform.sbr \
	$(INTDIR)/font.sbr

$(OUTDIR)/glaux.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"glaux.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/image.obj \
	$(INTDIR)/shapes.obj \
	$(INTDIR)/teapot.obj \
	$(INTDIR)/glaux.obj \
	$(INTDIR)/vect3d.obj \
	$(INTDIR)/xform.obj \
	$(INTDIR)/font.obj

$(OUTDIR)/glaux.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
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

ALL : $(OUTDIR)/glaux.lib $(OUTDIR)/glaux.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Z7 /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Z7 /YX /Od /I "\wing\include" /I "..\..\include" /I "..\..\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__WIN32__" /FR /c
CPP_PROJ=/nologo /W3 /GX /Z7 /YX /Od /I "\wing\include" /I "..\..\include" /I\
 "..\..\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__WIN32__" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"glaux.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinDebug/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"glaux.bsc" 
BSC32_SBRS= \
	$(INTDIR)/image.sbr \
	$(INTDIR)/shapes.sbr \
	$(INTDIR)/teapot.sbr \
	$(INTDIR)/glaux.sbr \
	$(INTDIR)/vect3d.sbr \
	$(INTDIR)/xform.sbr \
	$(INTDIR)/font.sbr

$(OUTDIR)/glaux.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO
LIB32_FLAGS=/NOLOGO /OUT:$(OUTDIR)\"glaux.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	$(INTDIR)/image.obj \
	$(INTDIR)/shapes.obj \
	$(INTDIR)/teapot.obj \
	$(INTDIR)/glaux.obj \
	$(INTDIR)/vect3d.obj \
	$(INTDIR)/xform.obj \
	$(INTDIR)/font.obj

$(OUTDIR)/glaux.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
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

SOURCE="\Mesa\src-aux\image.c"

$(INTDIR)/image.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Mesa\src-aux\shapes.c"
DEP_SHAPE=\
	C:\msdev\INCLUDE\gl\GL.H\
	C:\msdev\INCLUDE\gl\GLU.H\
	"\Mesa\src-aux\3d.h"

$(INTDIR)/shapes.obj :  $(SOURCE)  $(DEP_SHAPE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Mesa\src-aux\teapot.c"
DEP_TEAPO=\
	C:\msdev\INCLUDE\gl\GL.H

$(INTDIR)/teapot.obj :  $(SOURCE)  $(DEP_TEAPO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Mesa\src-aux\glaux.c"
DEP_GLAUX=\
	C:\msdev\INCLUDE\gl\GL.H

$(INTDIR)/glaux.obj :  $(SOURCE)  $(DEP_GLAUX) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Mesa\src-aux\vect3d.c"
DEP_VECT3=\
	C:\msdev\INCLUDE\gl\GL.H\
	"\Mesa\src-aux\3d.h"

$(INTDIR)/vect3d.obj :  $(SOURCE)  $(DEP_VECT3) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Mesa\src-aux\xform.c"
DEP_XFORM=\
	C:\msdev\INCLUDE\gl\GL.H\
	"\Mesa\src-aux\3d.h"

$(INTDIR)/xform.obj :  $(SOURCE)  $(DEP_XFORM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE="\Mesa\src-aux\font.c"

$(INTDIR)/font.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
# End Group
# End Project
################################################################################
