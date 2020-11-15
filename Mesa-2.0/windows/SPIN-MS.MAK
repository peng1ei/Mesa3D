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

PROJ		= spin-ms
APPTYPE		= WIN32 CONSOLE
PROJTYPE	= EXE

CC		= SC
CPP		= SPP
MAKE		= SMAKE
RC		= RCC
HC		= HC31
ASM		= SC
DISASM		= OBJ2ASM
LNK		= LINK
DLLS		= 

HEADERS		= ..\include\gl\glu.h d:\sc\include\windows.h d:\sc\include\win32\scdefs.h  \
		d:\sc\include\win32\windows.h d:\sc\include\excpt.h d:\sc\include\stdarg.h d:\sc\include\windef.h  \
		d:\sc\include\win32\windef.h d:\sc\include\winnt.h d:\sc\include\win32\winnt.h d:\sc\include\ctype.h  \
		d:\sc\include\string.h d:\sc\include\winbase.h d:\sc\include\win32\winbase.h d:\sc\include\winerror.h  \
		d:\sc\include\win32\winerror.h d:\sc\include\wingdi.h d:\sc\include\win32\wingdi.h d:\sc\include\winuser.h  \
		d:\sc\include\win32\winuser.h d:\sc\include\winnls.h d:\sc\include\win32\winnls.h d:\sc\include\wincon.h  \
		d:\sc\include\win32\wincon.h d:\sc\include\winver.h d:\sc\include\win32\winver.h d:\sc\include\winreg.h  \
		d:\sc\include\win32\winreg.h d:\sc\include\winnetwk.h d:\sc\include\win32\winnetwk.h d:\sc\include\cderr.h  \
		d:\sc\include\win32\cderr.h d:\sc\include\dde.h d:\sc\include\win32\dde.h d:\sc\include\ddeml.h  \
		d:\sc\include\win32\ddeml.h d:\sc\include\dlgs.h d:\sc\include\win32\dlgs.h d:\sc\include\lzexpand.h  \
		d:\sc\include\win32\lzexpand.h d:\sc\include\mmsystem.h d:\sc\include\win32\mmsystem.h d:\sc\include\nb30.h  \
		d:\sc\include\win32\nb30.h d:\sc\include\rpc.h d:\sc\include\win32\rpc.h d:\sc\include\shellapi.h  \
		d:\sc\include\win32\shellapi.h d:\sc\include\winperf.h d:\sc\include\win32\winperf.h d:\sc\include\winsock.h  \
		d:\sc\include\win32\winsock.h d:\sc\include\commdlg.h d:\sc\include\win32\commdlg.h d:\sc\include\drivinit.h  \
		d:\sc\include\win32\drivinit.h d:\sc\include\winspool.h d:\sc\include\win32\winspool.h d:\sc\include\ole2.h  \
		d:\sc\include\win32\ole2.h d:\sc\include\objerror.h d:\sc\include\win32\objerror.h d:\sc\include\objbase.h  \
		d:\sc\include\win32\objbase.h d:\sc\include\rpcndr.h d:\sc\include\win32\rpcndr.h d:\sc\include\stdlib.h  \
		d:\sc\include\oleauto.h d:\sc\include\win32\oleauto.h d:\sc\include\winsvc.h d:\sc\include\win32\winsvc.h  \
		d:\sc\include\math.h d:\sc\include\stdio.h d:\sc\include\time.h d:\sc\include\win32\pshpack4.h  \
		..\include\gl\gl.h ..\include\gl\wmesa.h ..\include\gl/gl.h ..\include\tk.h  \
		d:\sc\include\win32\cguid.h d:\sc\include\win32\rpcnsip.h d:\sc\include\win32\pshpack8.h d:\sc\include\win32\rpcnterr.h  \
		d:\sc\include\win32\rpcnsi.h d:\sc\include\win32\rpcdcep.h d:\sc\include\win32\rpcdce.h d:\sc\include\win32\pshpack2.h  \
		d:\sc\include\win32\pshpack1.h d:\sc\include\win32\poppack.h 

DEFFILE		= spin-ms.def

!IF $(DEBUG)
OUTPUTDIR	= .
CREATEOUTPUTDIR	=
TARGETDIR	= .
CREATETARGETDIR	=

LIBS		= c:\wing\lib\wing32.lib winmesa.lib KERNEL32.LIB GDI32.LIB USER32.LIB 

CFLAGS		=  -mn -C -WA -S -3 -a8 -c -H -HO- -g 
LFLAGS		=  /CO /NOI /DE /PACKF /XN /NT /ENTRY:_mainCRTStartup /VERS:1.0 /BAS:4194304 /A:512
DEFINES		= -D__WIN32__ -D_CONSOLE=1 
!ELSE
OUTPUTDIR	= .
CREATEOUTPUTDIR	=
TARGETDIR	= .
CREATETARGETDIR	=

LIBS		= c:\wing\lib\wing32.lib winmesa.lib KERNEL32.LIB GDI32.LIB USER32.LIB 

CFLAGS		=  -mn -o+time -WA -3 -a8 -c 
LFLAGS		=  /NOI /DE /E /PACKF /XN /NT /ENTRY:_mainCRTStartup /VERS:1.0 /BAS:4194304 /A:512
DEFINES		= -D\mesa\include -D_CONSOLE=1
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

INCLUDES	= -I\mesa\include 

INCLUDEDOBJS	= 

OBJS		= ..\src-tk\tk-wndws.OBJ ..\demos\spin.OBJ 

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

$(TARGETDIR)\$(PROJ).$(PROJTYPE): $(OBJS) $(INCLUDEDOBJS) $(RCFILES) $(RESFILES) $(HELPFILES) $(DEFFILE)
			$(LNK) $(LFLAGS) @$(PROJ).LNK;
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
		$(LNK) $(LFLAGS) @$(PROJ).LNK;
		-del $(TARGETDIR)\$(PROJ).$(PROJTYPE)
		-ren $(TARGETDIR)\$$SCW$$.$(PROJTYPE) $(PROJ).$(PROJTYPE)




!IF EXIST (spin-ms.dpd)
!INCLUDE spin-ms.dpd
!ENDIF


