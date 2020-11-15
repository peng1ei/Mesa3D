/* $Id: image.h,v 1.1 1996/09/13 01:38:16 brianp Exp $ */

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
 * $Log: image.h,v $
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


#ifndef IMAGE_H
#define IMAGE_H


#include "types.h"


extern void gl_flip_bytes( GLubyte *p, GLuint n );


extern void gl_swap2( GLushort *p, GLuint n );

extern void gl_swap4( GLuint *p, GLuint n );


extern GLint gl_sizeof_type( GLenum type );


extern GLint gl_components_in_format( GLenum format );


extern GLvoid *gl_pixel_addr_in_image( GLcontext *ctx,
                                const GLvoid *image, GLsizei width,
                                GLsizei height, GLenum format, GLenum type,
                                GLboolean packing,
                                GLint row, GLint column );


extern struct gl_image *gl_unpack_image( GLcontext *ctx,
                                  GLint width, GLint height,
                                  GLint components, GLenum srctype,
                                  GLenum desttype,
                                  const GLvoid *pixels,
                                  GLboolean interleave );

extern void gl_free_image( struct gl_image *image );


#endif
