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

PROJ		= winmesa
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

HEADERS		= ..\src\context.h ..\src\quickpol.h ..\src\wmesap.h  \
		..\include\gl\gl.h ..\include\gl\wmesa.h ..\src\scissor.h ..\src\blend.h  \
		..\src\alpha.h ..\src\logic.h ..\src\enable.h ..\src\drawpixe.h  \
		..\src\copypixe.h ..\src\bitmap.h ..\src\attrib.h ..\src\accum.h  \
		..\src\interp.h ..\src\bresenha.h ..\src\dither.h ..\src\texture.h  \
		..\src\polygons.h ..\src\points.h ..\src\lines.h ..\src\light.h  \
		..\src\fog.h ..\src\draw.h ..\src\stencil.h ..\src\span.h  \
		..\src\depth.h ..\src\eval.h ..\src\xform.h ..\src\vb.h  \
		..\src\clip.h ..\src\pixel.h ..\src\pb.h ..\src\feedback.h  \
		..\src\macros.h ..\src\list.h ..\src\dd.h ..\src\config.h  \
		..\include\gl/gl.h d:\sc\include\stdlib.h d:\sc\include\string.h d:\sc\include\assert.h  \
		d:\sc\include\stdio.h d:\sc\include\math.h d:\sc\include\windows.h d:\sc\include\win32\scdefs.h  \
		d:\sc\include\win32\windows.h d:\sc\include\excpt.h d:\sc\include\stdarg.h d:\sc\include\windef.h  \
		d:\sc\include\win32\windef.h d:\sc\include\winnt.h d:\sc\include\win32\winnt.h d:\sc\include\ctype.h  \
		d:\sc\include\win32\pshpack4.h d:\sc\include\win32\poppack.h d:\sc\include\win32\pshpack1.h d:\sc\include\winbase.h  \
		d:\sc\include\win32\winbase.h d:\sc\include\winerror.h d:\sc\include\win32\winerror.h d:\sc\include\wingdi.h  \
		d:\sc\include\win32\wingdi.h d:\sc\include\win32\pshpack2.h d:\sc\include\winuser.h d:\sc\include\win32\winuser.h  \
		d:\sc\include\winnls.h d:\sc\include\win32\winnls.h d:\sc\include\wincon.h d:\sc\include\win32\wincon.h  \
		d:\sc\include\winver.h d:\sc\include\win32\winver.h d:\sc\include\winreg.h d:\sc\include\win32\winreg.h  \
		d:\sc\include\winnetwk.h d:\sc\include\win32\winnetwk.h d:\sc\include\cderr.h d:\sc\include\win32\cderr.h  \
		d:\sc\include\dde.h d:\sc\include\win32\dde.h d:\sc\include\ddeml.h d:\sc\include\win32\ddeml.h  \
		d:\sc\include\dlgs.h d:\sc\include\win32\dlgs.h d:\sc\include\lzexpand.h d:\sc\include\win32\lzexpand.h  \
		d:\sc\include\mmsystem.h d:\sc\include\win32\mmsystem.h d:\sc\include\nb30.h d:\sc\include\win32\nb30.h  \
		d:\sc\include\rpc.h d:\sc\include\win32\rpc.h d:\sc\include\win32\rpcdce.h d:\sc\include\win32\rpcdcep.h  \
		d:\sc\include\win32\rpcnsi.h d:\sc\include\win32\rpcnterr.h d:\sc\include\shellapi.h d:\sc\include\win32\shellapi.h  \
		d:\sc\include\winperf.h d:\sc\include\win32\winperf.h d:\sc\include\winsock.h d:\sc\include\win32\winsock.h  \
		d:\sc\include\commdlg.h d:\sc\include\win32\commdlg.h d:\sc\include\drivinit.h d:\sc\include\win32\drivinit.h  \
		d:\sc\include\winspool.h d:\sc\include\win32\winspool.h d:\sc\include\ole2.h d:\sc\include\win32\ole2.h  \
		d:\sc\include\win32\pshpack8.h d:\sc\include\objerror.h d:\sc\include\win32\objerror.h d:\sc\include\objbase.h  \
		d:\sc\include\win32\objbase.h d:\sc\include\rpcndr.h d:\sc\include\win32\rpcndr.h d:\sc\include\win32\rpcnsip.h  \
		d:\sc\include\win32\cguid.h d:\sc\include\oleauto.h d:\sc\include\win32\oleauto.h d:\sc\include\winsvc.h  \
		d:\sc\include\win32\winsvc.h c:\wing\include\wing.h 

DEFFILE		= winmesa.def

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

OBJS		=  $(OUTPUTDIR)\accum.OBJ  $(OUTPUTDIR)\alpha.OBJ  $(OUTPUTDIR)\attrib.OBJ  \
		 $(OUTPUTDIR)\bitmap.OBJ  $(OUTPUTDIR)\blend.OBJ  $(OUTPUTDIR)\bresenha.OBJ  $(OUTPUTDIR)\clip.OBJ  \
		 $(OUTPUTDIR)\context.OBJ  $(OUTPUTDIR)\copypixe.OBJ  $(OUTPUTDIR)\depth.OBJ  $(OUTPUTDIR)\dither.OBJ  \
		 $(OUTPUTDIR)\draw.OBJ  $(OUTPUTDIR)\drawpixe.OBJ  $(OUTPUTDIR)\enable.OBJ  $(OUTPUTDIR)\eval2.OBJ  \
		 $(OUTPUTDIR)\feedback.OBJ  $(OUTPUTDIR)\fog.OBJ  $(OUTPUTDIR)\get.OBJ  $(OUTPUTDIR)\interp.OBJ  \
		 $(OUTPUTDIR)\light.OBJ  $(OUTPUTDIR)\lines.OBJ  $(OUTPUTDIR)\list.OBJ  $(OUTPUTDIR)\logic.OBJ  \
		 $(OUTPUTDIR)\misc.OBJ  $(OUTPUTDIR)\pb.OBJ  $(OUTPUTDIR)\pixel.OBJ  $(OUTPUTDIR)\points.OBJ  \
		 $(OUTPUTDIR)\polygons.OBJ  $(OUTPUTDIR)\quickpol.OBJ  $(OUTPUTDIR)\readpixe.OBJ  $(OUTPUTDIR)\scissor.OBJ  \
		 $(OUTPUTDIR)\span.OBJ  $(OUTPUTDIR)\stencil.OBJ  $(OUTPUTDIR)\texture.OBJ  $(OUTPUTDIR)\vertex.OBJ  \
		 $(OUTPUTDIR)\wmesa.OBJ  $(OUTPUTDIR)\xform.OBJ 

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
 +\mesa\windows\accum +\mesa\windows\alpha +\mesa\windows\attrib &
		 +\mesa\windows\bitmap +\mesa\windows\blend +\mesa\windows\bresenha +\mesa\windows\clip &
		 +\mesa\windows\context +\mesa\windows\copypixe +\mesa\windows\depth +\mesa\windows\dither &
		 +\mesa\windows\draw +\mesa\windows\drawpixe +\mesa\windows\enable +\mesa\windows\eval2 &
		 +\mesa\windows\feedback +\mesa\windows\fog +\mesa\windows\get +\mesa\windows\interp &
		 +\mesa\windows\light +\mesa\windows\lines +\mesa\windows\list +\mesa\windows\logic &
		 +\mesa\windows\misc +\mesa\windows\pb +\mesa\windows\pixel +\mesa\windows\points &
		 +\mesa\windows\polygons +\mesa\windows\quickpol +\mesa\windows\readpixe +\mesa\windows\scissor &
		 +\mesa\windows\span +\mesa\windows\stencil +\mesa\windows\texture +\mesa\windows\vertex &
		 +\mesa\windows\wmesa +\mesa\windows\xform
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
 +\mesa\windows\accum +\mesa\windows\alpha +\mesa\windows\attrib &
		 +\mesa\windows\bitmap +\mesa\windows\blend +\mesa\windows\bresenha +\mesa\windows\clip &
		 +\mesa\windows\context +\mesa\windows\copypixe +\mesa\windows\depth +\mesa\windows\dither &
		 +\mesa\windows\draw +\mesa\windows\drawpixe +\mesa\windows\enable +\mesa\windows\eval2 &
		 +\mesa\windows\feedback +\mesa\windows\fog +\mesa\windows\get +\mesa\windows\interp &
		 +\mesa\windows\light +\mesa\windows\lines +\mesa\windows\list +\mesa\windows\logic &
		 +\mesa\windows\misc +\mesa\windows\pb +\mesa\windows\pixel +\mesa\windows\points &
		 +\mesa\windows\polygons +\mesa\windows\quickpol +\mesa\windows\readpixe +\mesa\windows\scissor &
		 +\mesa\windows\span +\mesa\windows\stencil +\mesa\windows\texture +\mesa\windows\vertex &
		 +\mesa\windows\wmesa +\mesa\windows\xform
<<
		-del $(TARGETDIR)\$(PROJ).$(PROJTYPE)
		-ren $(TARGETDIR)\$$SCW$$.$(PROJTYPE) $(PROJ).$(PROJTYPE)




!IF EXIST (winmesa.dpd)
!INCLUDE winmesa.dpd
!ENDIF



$(OUTPUTDIR)\accum.OBJ:	..\src\accum.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\accum.obj ..\src\accum.c



$(OUTPUTDIR)\alpha.OBJ:	..\src\alpha.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\alpha.obj ..\src\alpha.c



$(OUTPUTDIR)\attrib.OBJ:	..\src\attrib.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\attrib.obj ..\src\attrib.c



$(OUTPUTDIR)\bitmap.OBJ:	..\src\bitmap.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\bitmap.obj ..\src\bitmap.c



$(OUTPUTDIR)\blend.OBJ:	..\src\blend.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\blend.obj ..\src\blend.c



$(OUTPUTDIR)\bresenha.OBJ:	..\src\bresenha.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\bresenha.obj ..\src\bresenha.c



$(OUTPUTDIR)\clip.OBJ:	..\src\clip.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\clip.obj ..\src\clip.c



$(OUTPUTDIR)\context.OBJ:	..\src\context.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\context.obj ..\src\context.c



$(OUTPUTDIR)\copypixe.OBJ:	..\src\copypixe.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\copypixe.obj ..\src\copypixe.c



$(OUTPUTDIR)\depth.OBJ:	..\src\depth.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\depth.obj ..\src\depth.c



$(OUTPUTDIR)\dither.OBJ:	..\src\dither.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\dither.obj ..\src\dither.c



$(OUTPUTDIR)\draw.OBJ:	..\src\draw.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\draw.obj ..\src\draw.c



$(OUTPUTDIR)\drawpixe.OBJ:	..\src\drawpixe.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\drawpixe.obj ..\src\drawpixe.c



$(OUTPUTDIR)\enable.OBJ:	..\src\enable.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\enable.obj ..\src\enable.c



$(OUTPUTDIR)\eval2.OBJ:	..\src\eval2.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\eval2.obj ..\src\eval2.c



$(OUTPUTDIR)\feedback.OBJ:	..\src\feedback.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\feedback.obj ..\src\feedback.c



$(OUTPUTDIR)\fog.OBJ:	..\src\fog.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\fog.obj ..\src\fog.c



$(OUTPUTDIR)\get.OBJ:	..\src\get.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\get.obj ..\src\get.c



$(OUTPUTDIR)\interp.OBJ:	..\src\interp.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\interp.obj ..\src\interp.c



$(OUTPUTDIR)\light.OBJ:	..\src\light.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\light.obj ..\src\light.c



$(OUTPUTDIR)\lines.OBJ:	..\src\lines.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\lines.obj ..\src\lines.c



$(OUTPUTDIR)\list.OBJ:	..\src\list.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\list.obj ..\src\list.c



$(OUTPUTDIR)\logic.OBJ:	..\src\logic.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\logic.obj ..\src\logic.c



$(OUTPUTDIR)\misc.OBJ:	..\src\misc.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\misc.obj ..\src\misc.c



$(OUTPUTDIR)\pb.OBJ:	..\src\pb.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\pb.obj ..\src\pb.c



$(OUTPUTDIR)\pixel.OBJ:	..\src\pixel.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\pixel.obj ..\src\pixel.c



$(OUTPUTDIR)\points.OBJ:	..\src\points.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\points.obj ..\src\points.c



$(OUTPUTDIR)\polygons.OBJ:	..\src\polygons.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\polygons.obj ..\src\polygons.c



$(OUTPUTDIR)\quickpol.OBJ:	..\src\quickpol.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\quickpol.obj ..\src\quickpol.c



$(OUTPUTDIR)\readpixe.OBJ:	..\src\readpixe.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\readpixe.obj ..\src\readpixe.c



$(OUTPUTDIR)\scissor.OBJ:	..\src\scissor.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\scissor.obj ..\src\scissor.c



$(OUTPUTDIR)\span.OBJ:	..\src\span.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\span.obj ..\src\span.c



$(OUTPUTDIR)\stencil.OBJ:	..\src\stencil.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\stencil.obj ..\src\stencil.c



$(OUTPUTDIR)\texture.OBJ:	..\src\texture.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\texture.obj ..\src\texture.c



$(OUTPUTDIR)\vertex.OBJ:	..\src\vertex.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\vertex.obj ..\src\vertex.c



$(OUTPUTDIR)\wmesa.OBJ:	..\src\wmesa.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\wmesa.obj ..\src\wmesa.c



$(OUTPUTDIR)\xform.OBJ:	..\src\xform.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\xform.obj ..\src\xform.c




