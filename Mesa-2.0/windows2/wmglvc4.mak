LIBDIR = ..\lib
LIBCMD = lib
LIBOPTS = /OUT:$(LIBDIR)\wmgl20.lib
CC = cl
CCOPTS = /c /Zi
INCDIR = ..\include /Id:\wing\include

CORE_OBJS = accum.obj alpha.obj alphabuf.obj api.obj attrib.obj bitmap.obj \
	blend.obj bresenhm.obj clip.obj context.obj copypix.obj depth.obj \
	dlist.obj draw.obj drawpix.obj enable.obj eval.obj feedback.obj fog.obj \
	fortran.obj get.obj interp.obj image.obj light.obj lines.obj logic.obj \
	masking.obj matrix.obj misc.obj pb.obj pixel.obj points.obj pointers.obj \
	polygon.obj readpix.obj scissor.obj span.obj stencil.obj teximage.obj \
	texobj.obj texture.obj triangle.obj varray.obj vb.obj vertex.obj winpos.obj \
	xform.obj

DRIVER_OBJS = wmesa.obj

wmesa32.lib : $(CORE_OBJS) $(DRIVER_OBJS)
	$(LIBCMD) $(LIBOPTS) $(CORE_OBJS) $(DRIVER_OBJS)

.c.obj:
	$(CC) /c $(CCOPTS) /I$(INCDIR) $*.c

