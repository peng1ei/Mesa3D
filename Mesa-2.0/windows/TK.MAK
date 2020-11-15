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

PROJ		= tk
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

HEADERS		= ..\include\gl\glu.h ..\src-glu\glup.h ..\src-glu\tess.h  \
		..\include\gl\gl.h ..\include\gl\wmesa.h ..\include\gl/gl.h ..\include\tk.h  \
		d:\sc\include\windows.h d:\sc\include\win32\scdefs.h d:\sc\include\win32\windows.h d:\sc\include\excpt.h  \
		d:\sc\include\stdarg.h d:\sc\include\windef.h d:\sc\include\win32\windef.h d:\sc\include\winnt.h  \
		d:\sc\include\win32\winnt.h d:\sc\include\ctype.h d:\sc\include\win32\pshpack4.h d:\sc\include\win32\poppack.h  \
		d:\sc\include\win32\pshpack1.h d:\sc\include\string.h d:\sc\include\winbase.h d:\sc\include\win32\winbase.h  \
		d:\sc\include\winerror.h d:\sc\include\win32\winerror.h d:\sc\include\wingdi.h d:\sc\include\win32\wingdi.h  \
		d:\sc\include\win32\pshpack2.h d:\sc\include\winuser.h d:\sc\include\win32\winuser.h d:\sc\include\winnls.h  \
		d:\sc\include\win32\winnls.h d:\sc\include\wincon.h d:\sc\include\win32\wincon.h d:\sc\include\winver.h  \
		d:\sc\include\win32\winver.h d:\sc\include\winreg.h d:\sc\include\win32\winreg.h d:\sc\include\winnetwk.h  \
		d:\sc\include\win32\winnetwk.h d:\sc\include\cderr.h d:\sc\include\win32\cderr.h d:\sc\include\dde.h  \
		d:\sc\include\win32\dde.h d:\sc\include\ddeml.h d:\sc\include\win32\ddeml.h d:\sc\include\dlgs.h  \
		d:\sc\include\win32\dlgs.h d:\sc\include\lzexpand.h d:\sc\include\win32\lzexpand.h d:\sc\include\mmsystem.h  \
		d:\sc\include\win32\mmsystem.h d:\sc\include\nb30.h d:\sc\include\win32\nb30.h d:\sc\include\rpc.h  \
		d:\sc\include\win32\rpc.h d:\sc\include\win32\rpcdce.h d:\sc\include\win32\rpcdcep.h d:\sc\include\win32\rpcnsi.h  \
		d:\sc\include\win32\rpcnterr.h d:\sc\include\shellapi.h d:\sc\include\win32\shellapi.h d:\sc\include\winperf.h  \
		d:\sc\include\win32\winperf.h d:\sc\include\winsock.h d:\sc\include\win32\winsock.h d:\sc\include\commdlg.h  \
		d:\sc\include\win32\commdlg.h d:\sc\include\drivinit.h d:\sc\include\win32\drivinit.h d:\sc\include\winspool.h  \
		d:\sc\include\win32\winspool.h d:\sc\include\ole2.h d:\sc\include\win32\ole2.h d:\sc\include\win32\pshpack8.h  \
		d:\sc\include\objerror.h d:\sc\include\win32\objerror.h d:\sc\include\objbase.h d:\sc\include\win32\objbase.h  \
		d:\sc\include\rpcndr.h d:\sc\include\win32\rpcndr.h d:\sc\include\win32\rpcnsip.h d:\sc\include\win32\cguid.h  \
		d:\sc\include\stdlib.h d:\sc\include\oleauto.h d:\sc\include\win32\oleauto.h d:\sc\include\winsvc.h  \
		d:\sc\include\win32\winsvc.h d:\sc\include\stdio.h 

DEFFILE		= TK.DEF

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

OBJS		=  $(OUTPUTDIR)\tkwndws.OBJ 

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
 +\mesa\windows\tkwndws
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
 +\mesa\windows\tkwndws
<<
		-del $(TARGETDIR)\$(PROJ).$(PROJTYPE)
		-ren $(TARGETDIR)\$$SCW$$.$(PROJTYPE) $(PROJ).$(PROJTYPE)




!IF EXIST (tk.dpd)
!INCLUDE tk.dpd
!ENDIF



$(OUTPUTDIR)\tkwndws.OBJ:	..\src-tk\tkwndws.c
		$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o$(OUTPUTDIR)\tkwndws.obj ..\src-tk\tkwndws.c




