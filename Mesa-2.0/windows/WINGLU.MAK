ORIGIN		= Symantec C++
ORIGIN_VER	= Version 7.00
VERSION		= DEBUG

!IFDEF SUB_DEBUG
DEBUG		= $(SUB_DEBUG)
NDEBUG		= !$(SUB_DEBUG)
!ELSE
DEBUG		= 1
NDEBUG		= 0
!ENDIF

PROJ		= winglu
APPTYPE		= WIN32 LIB
PROJTYPE	= LIB

CC		= SC
CPP		= SPP
MAKE		= SMAKE
RC		= RCC
HC		= HC31
ASM		= SC
DISASM		= OBJ2ASM
LNK		= LINK
DLLS		= 

HEADERS		= ..\include\gl\gl.h ..\include\gl\glu.h ..\src-glu\glup.h  \
		..\src-glu\tess.h ..\include\gl/glu.h ..\include\gl/gl.h d:\sc\include\math.h  \
		d:\sc\include\stdio.h d:\sc\include\stdlib.h d:\sc\include\assert.h d:\sc\include\string.h 

DEFFILE		= WINGLU.DEF

!IF $(DEBUG)
OUTPUTDIR	= \mesa\windows
!IF EXIST (\mesa\windows)
CREATEOUTPUTDIR	=
!ELSE
CREATEOUTPUTDIR	= if not exist $(OUTPUTDIR)\*.* md $(OUTPUTDIR)
!ENDIF
TARGETDIR	= \mesa\windows
!IF EXIST (\mesa\windows)
CREATETARGETDIR	=
!ELSE
CREATETARGETDIR	= if not exist $(TARGETDIR)\*.* md $(TARGETDIR)
!ENDIF

LIBS		= 

CFLAGS		=  -J -mn -C -D_WINDOWS -S -s -3 -a1 -c -H -HO- -g 
LFLAGS		=  /CO /NOI /DE /PACKF /XN /A:512
DEFINES		= -DFILE_NAMES_8_3 -D__WIN32__ -D__SYMANTEC_BUGS 
!ELSE
OUTPUTDIR	= .
CREATEOUTPUTDIR	=
TARGETDIR	= .
CREATETARGETDIR	=

LIBS		= 

CFLAGS		=  -mn -o+time -D_WINDOWS -3 -a8 -c 
LFLAGS		=  /NOI /DE /E /PACKF /XN /A:512
DEFINES		= 
!ENDIF

HFLAGS		= $(CFLAGS) 
MFLAGS		= MASTERPROJ=$(PROJ) 
LIBFLAGS	=  /C 
RESFLAGS	=  -32 
DEBUGGERFLAGS	=  
AFLAGS		= $(CFLAGS) 
HELPFLAGS	= 

MODEL		= N

PAR		= PROJS BATS OBJS

RCDEFINES	= 

LIBDIRS		= 

INCLUDES	= -I..\include -I..\src -Ic:\wing\include 

INCLUDEDOBJS	= 

OBJS		=  $(OUTPUTDIR)\glu.OBJ  $(OUTPUTDIR)\mipmap.OBJ  $(OUTPUTDIR)\nurbs.OBJ  \
		 $(OUTPUTDIR)\project.OBJ  $(OUTPUTDIR)\quadric.OBJ  $(OUTPUTDIR)\tess.OBJ 

RCFILES		= 

RESFILES	= 

SYMS		= 

HELPFILES	= 

BATS		= 

.SUFFIXES: .C .CP .CPP .CXX .CC .H .HPP .HXX .COM .EXE .DLL .LIB .RTF .DLG .ASM .RES .RC .OBJ 

.C.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.c

.CPP.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cpp

.CXX.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cxx

.CC.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cc

.CP.OBJ:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.cp

.H.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o.\$(*B).sym $*.h

.HPP.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o.\$(*B).sym $*.hpp

.HXX.SYM:
	$(CC) $(HFLAGS) $(DEFINES) $(INCLUDES) -HF -o.\$(*B).sym $*.hxx

.C.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES)   $*.c   -o$*.lst

.CPP.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES) $*.cpp -o$*.lst

.CXX.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES) $*.cxx -o$*.lst

.CP.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES)  $*.cp  -o$*.lst

.CC.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES)  $*.cc  -o$*.lst

.ASM.EXP:
	$(CPP) $(CFLAGS) $(DEFINES) $(INCLUDES) $*.asm -o$*.lst

.OBJ.COD:
	$(DISASM) $*.OBJ >$*.cod

.OBJ.EXE:
	$(LNK) $(LFLAGS) @$(PROJ).LNK

.RTF.HLP:
	$(HC) $(HELPFLAGS) $*.HPJ

.ASM.OBJ:
	$(ASM) $(AFLAGS) $(DEFINES) $(INCLUDES) -o$*.obj $*.asm

.RC.RES: 
	$(RC) $(RCDEFINES) $(RESFLAGS) $(INCLUDES) $*.rc -o$*.res

.DLG.RES:
	echo ^#include "windows.h" >$$$*.rc
	echo ^IF EXIST "$*.h" >>$$$*.rc
	echo ^#include "$*.h" >>$$$*.rc
	echo ^#include "$*.dlg" >>$$$*.rc
	$(RC) $(RCDEFINES) $(RESFLAGS) $(INCLUDES) $$$*.rc
	-del $*.res
	-ren $$$*.res $*.res



all:	createdir $(PRECOMPILE) $(SYMS) $(OBJS) $(INCLUDEDOBJS) $(POSTCOMPILE) $(TARGETDIR)\$(PROJ).$(PROJTYPE) $(POSTLINK) _done

createdir:
	$(CREATEOUTPUTDIR)
	$(CREATETARGETDIR)

$(TARGETDIR)\$(PROJ).$(PROJTYPE): $(OBJS) $(INCLUDEDOBJS) $(RCFILES) $(RESFILES) $(HELPFILES) 
		LIB $(TARGETDIR)\$$SCW$$.$(PROJTYPE) $(LIBFLAGS)\
				@<<
 +\mesa\windows\glu +\mesa\windows\mipmap +\mesa\windows\nurbs &
		 +\mesa\windows\project +\mesa\windows\quadric +\mesa\windows\tess
<<
			-del $(TARGETDIR)\$(PROJ).$(PROJTYPE)
			-ren $(TARGETDIR)\$$SCW$$.$(PROJTYPE) $(PROJ).$(PROJTYPE)
			-echo $(TARGETDIR)\$(PROJ).$(PROJTYPE) built

_done:
		-echo $(PROJ).$(PROJTYPE) done

buildall:	clean	all


clean:
		-del $(TARGETDIR)\$$SCW$$.$(PROJTYPE)
		-del $(TARGETDIR)\$(PROJ).CLE
		-del $(OUTPUTDIR)\SCPH.SYM
		-del $(OBJS)

cleanres:

res:		cleanres $(RCFILES) all


link:
		LIB $$SCW$$.$(PROJTYPE) $(LIBFLAGS)\
				@<<
 +\mesa\windows\glu +\mesa\windows\mipmap +\mesa\windows\nurbs &
		 +\mesa\windows\project +\mesa\windows\quadric +\mesa\windows\tess
<<
		-del $(TARGETDIR)\$(PROJ).$(PROJTYPE)
		-ren $(TARGETDIR)\$$SCW$$.$(PROJTYPE) $(PROJ).$(PROJTYPE)




!IF EXIST (winglu.dpd)
!INCLUDE winglu.dpd
!ENDIF



$(OUTPUTDIR)\glu.OBJ:	..\src-glu\glu.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\glu.obj ..\src-glu\glu.c



$(OUTPUTDIR)\mipmap.OBJ:	..\src-glu\mipmap.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\mipmap.obj ..\src-glu\mipmap.c



$(OUTPUTDIR)\nurbs.OBJ:	..\src-glu\nurbs.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\nurbs.obj ..\src-glu\nurbs.c



$(OUTPUTDIR)\project.OBJ:	..\src-glu\project.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\project.obj ..\src-glu\project.c



$(OUTPUTDIR)\quadric.OBJ:	..\src-glu\quadric.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\quadric.obj ..\src-glu\quadric.c



$(OUTPUTDIR)\tess.OBJ:	..\src-glu\tess.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\tess.obj ..\src-glu\tess.c




