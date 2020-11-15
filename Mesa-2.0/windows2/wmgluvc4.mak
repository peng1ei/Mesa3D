LIBDIR = ..\lib
LIBCMD = lib
LIBOPTS = /OUT:$(LIBDIR)\wmglu20.lib
CC = cl
CCOPTS = /c /Zi
INCDIR = ..\include

CORE_OBJS = glu.obj quadric.obj mipmap.obj nurbs.obj \
	project.obj tess.obj


$(LIBDIR)\wmglu20.lib : $(CORE_OBJS)
	$(LIBCMD) $(LIBOPTS) $(CORE_OBJS)

.c.obj:
	$(CC) /c $(CCOPTS) /I$(INCDIR) $*.c
