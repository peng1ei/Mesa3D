LIBDIR = ..\lib
LIBCMD = lib
LIBOPTS = /OUT:$(LIBDIR)\wmglaux20.lib
CC = cl
CCOPTS = /c /D__WIN32__ /Zi
INCDIR = ..\include

CORE_OBJS = image.obj shapes.obj teapot.obj glaux.obj \
	vect3d.obj xform.obj font.obj


$(LIBDIR)\wmglaux20.lib : $(CORE_OBJS)
	$(LIBCMD) $(LIBOPTS) $(CORE_OBJS)

.c.obj:
	$(CC) /c $(CCOPTS) /I$(INCDIR) $*.c

