LIBDIR = ..\lib
LIBCMD = lib
LIBOPTS = /OUT:$(LIBDIR)\wtk.lib
CC = cl
CCOPTS = /c /D__WIN32__ /Zi
INCDIR = ..\include

CORE_OBJS = tkwndws.obj shapes.obj


$(LIBDIR)\wtk.lib : $(CORE_OBJS)
	$(LIBCMD) $(LIBOPTS) $(CORE_OBJS)

.c.obj:
	$(CC) /c $(CCOPTS) /I$(INCDIR) $*.c

