/* $Id: logic.c,v 1.1 1996/09/13 01:38:16 brianp Exp $ */

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
 * $Log: logic.c,v $
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


#include "context.h"
#include "dlist.h"
#include "logic.h"
#include "macros.h"
#include "pb.h"
#include "types.h"



void gl_LogicOp( GLcontext *ctx, GLenum opcode )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glLogicOp" );
      return;
   }
   switch (opcode) {
      case GL_CLEAR:
      case GL_SET:
      case GL_COPY:
      case GL_COPY_INVERTED:
      case GL_NOOP:
      case GL_INVERT:
      case GL_AND:
      case GL_NAND:
      case GL_OR:
      case GL_NOR:
      case GL_XOR:
      case GL_EQUIV:
      case GL_AND_REVERSE:
      case GL_AND_INVERTED:
      case GL_OR_REVERSE:
      case GL_OR_INVERTED:
         ctx->Color.LogicOp = opcode;
         ctx->NewState |= NEW_RASTER_OPS;
	 return;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glLogicOp" );
	 return;
   }
}




/*
 * Apply the current logic operator to a span of pixels.  This is only
 * used if the device driver can't do logic ops.
 */
void gl_logic_span( GLcontext *ctx, GLuint n, GLint x, GLint y, GLuint index[],
		    GLubyte mask[] )
{
   GLuint dest[MAX_WIDTH];
   GLuint i;

   /* Read dest values from frame buffer */
   (*ctx->Driver.ReadIndexSpan)( ctx, n, x, y, dest );

   switch (ctx->Color.LogicOp) {
      case GL_CLEAR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = 0;
	    }
	 }
	 break;
      case GL_SET:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = 1;
	    }
	 }
	 break;
      case GL_COPY:
	 /* do nothing */
	 break;
      case GL_COPY_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !index[i];
	    }
	 }
	 break;
      case GL_NOOP:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = dest[i];
	    }
	 }
	 break;
      case GL_INVERT:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !dest[i];
	    }
	 }
	 break;
      case GL_AND:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] &= dest[i];
	    }
	 }
	 break;
      case GL_NAND:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !(index[i] & dest[i]);
	    }
	 }
	 break;
      case GL_OR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] |= dest[i];
	    }
	 }
	 break;
      case GL_NOR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !(index[i] | dest[i]);
	    }
	 }
	 break;
      case GL_XOR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] ^= dest[i];
	    }
	 }
	 break;
      case GL_EQUIV:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !(index[i] ^ dest[i]);
	    }
	 }
	 break;
      case GL_AND_REVERSE:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = index[i] & !dest[i];
	    }
	 }
	 break;
      case GL_AND_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !index[i] & dest[i];
	    }
	 }
	 break;
      case GL_OR_REVERSE:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = index[i] | !dest[i];
	    }
	 }
	 break;
      case GL_OR_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !index[i] | dest[i];
	    }
	 }
	 break;
      default:
	 gl_error( ctx, GL_INVALID_ENUM, "gl_logic error" );
   }
}



/*
 * Apply the current logic operator to an array of pixels.  This is only
 * used if the device driver can't do logic ops.
 */
void gl_logic_pixels( GLcontext *ctx,
                      GLuint n, const GLint x[], const GLint y[],
		      GLuint index[], GLubyte mask[] )
{
   GLuint dest[PB_SIZE];
   GLuint i;

   /* Read dest values from frame buffer */
   (*ctx->Driver.ReadIndexPixels)( ctx, n, x, y, dest, mask );

   switch (ctx->Color.LogicOp) {
      case GL_CLEAR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = 0;
	    }
	 }
	 break;
      case GL_SET:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = 1;
	    }
	 }
	 break;
      case GL_COPY:
	 /* do nothing */
	 break;
      case GL_COPY_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !index[i];
	    }
	 }
	 break;
      case GL_NOOP:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = dest[i];
	    }
	 }
	 break;
      case GL_INVERT:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !dest[i];
	    }
	 }
	 break;
      case GL_AND:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] &= dest[i];
	    }
	 }
	 break;
      case GL_NAND:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !(index[i] & dest[i]);
	    }
	 }
	 break;
      case GL_OR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] |= dest[i];
	    }
	 }
	 break;
      case GL_NOR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !(index[i] | dest[i]);
	    }
	 }
	 break;
      case GL_XOR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] ^= dest[i];
	    }
	 }
	 break;
      case GL_EQUIV:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !(index[i] ^ dest[i]);
	    }
	 }
	 break;
      case GL_AND_REVERSE:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = index[i] & !dest[i];
	    }
	 }
	 break;
      case GL_AND_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !index[i] & dest[i];
	    }
	 }
	 break;
      case GL_OR_REVERSE:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = index[i] | !dest[i];
	    }
	 }
	 break;
      case GL_OR_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = !index[i] | dest[i];
	    }
	 }
	 break;
      default:
	 gl_error( ctx, GL_INVALID_ENUM, "gl_logic_pixels error" );
   }
}
