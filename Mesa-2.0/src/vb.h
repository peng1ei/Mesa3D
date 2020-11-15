/* $Id: vb.h,v 1.2 1996/09/27 01:31:17 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  2.0
 * Copyright (C) 1995-1996  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
 * $Log: vb.h,v $
 * Revision 1.2  1996/09/27 01:31:17  brianp
 * added gl_init_vb() prototype
 *
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


/*
 * Vertex buffer:  vertices from glVertex* are accumulated here until
 * the buffer is full or glEnd() is called.  Then the buffer is flushed
 * (rendered) and reset.
 */


#ifndef VB_H
#define VB_H


#include "types.h"



/*
 * Used in the bitmask for recording glMaterial calls between glBegin/glEnd:
 */
#define FRONT_AMBIENT_BIT     0x1
#define BACK_AMBIENT_BIT      0x2
#define FRONT_DIFFUSE_BIT     0x4
#define BACK_DIFFUSE_BIT      0x8
#define FRONT_SPECULAR_BIT   0x10
#define BACK_SPECULAR_BIT    0x20
#define FRONT_EMISSION_BIT   0x40
#define BACK_EMISSION_BIT    0x80
#define FRONT_SHININESS_BIT 0x100
#define BACK_SHININESS_BIT  0x200
#define FRONT_INDEXES_BIT   0x400
#define BACK_INDEXES_BIT    0x800



/* Flush VB when this number of vertices is accumulated:  (a multiple of 12) */
#define VB_MAX 480

/* Arrays must also accomodate new vertices from clipping: */
#define VB_SIZE  (VB_MAX + 2 * (6 + MAX_CLIP_PLANES))


/*
 * Vertex buffer (not saved/restored on context switches)
 */
struct vertex_buffer {
        GLfloat Obj[VB_SIZE][4];        /* Object coords */
	GLfloat Eye[VB_SIZE][4];	/* Eye coords */
	GLfloat Clip[VB_SIZE][4];	/* Clip coords */
	GLfloat Win[VB_SIZE][3];	/* Window coords */
#ifdef MONDELLO
        GLint   Win2[VB_SIZE][3];       /* Integer window coords -PFM */
                                        /* NOTE: on mondello uses correctly */
#endif

        GLfloat Normal[VB_SIZE][3];     /* Normal vectors */

        /* Colors are values in [0..RedScale], [0..GreenScale], [0,BlueScale],
         * [0,AlphScale] and stored as integers if flat shading or as fixed
         * point numbers if smooth shading.
         */
	GLfixed Fcolor[VB_SIZE][4];	/* Front colors */
	GLfixed Bcolor[VB_SIZE][4];	/* Back colors */
	GLfixed (*Color)[4];		/* == Fcolor or Bcolor */

	GLuint Findex[VB_SIZE];         /* Front color indexes */
	GLuint Bindex[VB_SIZE];         /* Back color indexes */
	GLuint *Index;			/* == Findex or Bindex */

	GLboolean Edgeflag[VB_SIZE];	/* Polygon edge flag */

        GLfloat TexCoord[VB_SIZE][4];   /* Texture coords */

        GLubyte Unclipped[VB_SIZE];	/* 0=clipped, 1=not clipped */
        GLboolean AnyClipped;		/* Were any vertices clipped? */

	GLuint Start;			/* First vertex to process */
	GLuint Count;			/* Number of vertexes in buffer */
	GLuint Free;			/* Next empty position for clipping */

        /* to handle glMaterial calls inside glBegin/glEnd: */
	GLboolean MaterialChanges;	/* True if any glMaterial was called */
        GLuint MaterialMask[VB_SIZE];	/* What material values to change */
	struct gl_material Material[VB_SIZE][2]; /* New material settings */

        GLboolean MonoColor;		/* Do all vertices have same color? */
};



extern void gl_init_vb( struct vertex_buffer* VB );


#endif

