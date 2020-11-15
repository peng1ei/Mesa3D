# Makefile for Mesa - Linux - Cirrus Logic GD547x (Mondello) 3-D card



##### MACROS #####

VPATH = RCS

INCDIR = ../include
LIBDIR = ../lib

OBJECTS = clgd5470.o clgd5471.o clgd5472.o clgd547x.o graphics.o misc.o



##### RULES #####

.c.o:
	$(CC) -c -I$(INCDIR) $(CFLAGS) $<



##### TARGETS #####

default:
	@echo "Specify a target configuration"

clean:
	-rm *.o *~

targets: $(LIBDIR)/libclgd547x.so
#targets: tst

setmode: setmode.c
	$(CC) -I$(INCDIR) $(CFLAGS) setmode.c -o setmode

graphics.o: graphics.c linetemp.h tritemp.h
	$(CC) -c -I$(INCDIR) $(CFLAGS) graphics.c

# Make the library
$(LIBDIR)/libclgd547x.so: $(OBJECTS)
	$(MAKELIB) libclgd547x.so $(OBJECTS)
	$(RANLIB) libclgd547x.so
	mv libclgd547x.so* $(LIBDIR)

tst: $(OBJECTS) tst.c $(OBJECTS)
	gcc -g -I../include tst.c $(OBJECTS) -L../lib libclgd547x.a -lm -o tst

realclean:
	rm *.o; rm *.a; rm *~; rm tst

include ../Make-config


