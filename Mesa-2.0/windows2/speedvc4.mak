LIBDIR = ..\lib
LIBCMD = lib
LIBOPTS = /OUT:$(LIBDIR)\wmgl20.lib
CC = cl
CCOPTS = /c /Zi
INCDIR = ..\include /Id:\wing\include
MESA_LIBS = ..\lib\wmgl20.lib ..\lib\wmglu20.lib ..\lib\wmglaux20.lib ..\lib\wtk.lib
WIN_LIBS = \wing\lib\wing32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib

speed.exe : speed.obj

.c.obj:
	$(CC) /c $(CCOPTS) /I$(INCDIR) $*.c

.c.exe:
	$(CC) /c $(CCOPTS) /I$(INCDIR) $*.c
	link -debug $*.obj $(MESA_LIBS) $(WIN_LIBS)
