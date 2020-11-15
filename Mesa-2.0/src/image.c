/* $Id: image.c,v 1.3 1996/09/27 01:27:10 brianp Exp $ */

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
 * $Log: image.c,v $
 * Revision 1.3  1996/09/27 01:27:10  brianp
 * removed unused variables
 *
 * Revision 1.2  1996/09/26 22:35:10  brianp
 * fixed a few compiler warnings from IRIX 6 -n32 and -64 compiler
 *
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "macros.h"
#include "pixel.h"
#include "types.h"




/*
 * Flip the 8 bits in each byte of the given array.
 */
void gl_flip_bytes( GLubyte *p, GLuint n )
{
   register GLuint i, a, b;

   for (i=0;i<n;i++) {
      b = (GLuint) p[i];
      a = ((b & 0x01) << 7) |
	  ((b & 0x02) << 5) |
	  ((b & 0x04) << 3) |
	  ((b & 0x08) << 1) |
	  ((b & 0x10) >> 1) |
	  ((b & 0x20) >> 3) |
	  ((b & 0x40) >> 5) |
	  ((b & 0x80) >> 7);
      p[i] = (GLubyte) a;
   }
}


/*
 * Flip the order of the 2 bytes in each word in the given array.
 */
void gl_swap2( GLushort *p, GLuint n )
{
   register GLuint i;

   for (i=0;i<n;i++) {
      p[i] = (p[i] >> 8) | ((p[i] << 8) & 0xff00);
   }
}



/*
 * Flip the order of the 4 bytes in each word in the given array.
 */
void gl_swap4( GLuint *p, GLuint n )
{
   register GLuint i, a, b;

   for (i=0;i<n;i++) {
      b = p[i];
      a =  (b >> 24)
	| ((b >> 8) & 0xff00)
	| ((b << 8) & 0xff0000)
	| ((b << 24) & 0xff000000);
      p[i] = a;
   }
}




/*
 * Return the size, in bytes, of the given GL datatype.
 * Return 0 if GL_BITMAP.
 * Return -1 if invalid type enum.
 */
GLint gl_sizeof_type( GLenum type )
{
   switch (type) {
      case GL_BITMAP:
	 return 0;
      case GL_UNSIGNED_BYTE:
         return sizeof(GLubyte);
      case GL_BYTE:
	 return sizeof(GLbyte);
      case GL_UNSIGNED_SHORT:
	 return sizeof(GLushort);
      case GL_SHORT:
	 return sizeof(GLshort);
      case GL_UNSIGNED_INT:
	 return sizeof(GLuint);
      case GL_INT:
	 return sizeof(GLint);
      case GL_FLOAT:
	 return sizeof(GLfloat);
      default:
         return -1;
   }
}



/*
 * Return the number of components in a GL enum pixel type.
 * Return -1 if bad format.
 */
GLint gl_components_in_format( GLenum format )
{
   switch (format) {
      case GL_COLOR_INDEX:
      case GL_STENCIL_INDEX:
      case GL_DEPTH_COMPONENT:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
         return 1;
      case GL_LUMINANCE_ALPHA:
	 return 2;
      case GL_RGB:
	 return 3;
      case GL_RGBA:
	 return 4;
      default:
         return -1;
   }
}


/*
 * Return the address of a pixel in an image.  Pixel unpacking/packing
 * parameters are observed according to 'packing'.
 * Input:  image - start of image data
 *         width, height - size of image
 *         format - image format
 *         type - pixel component type
 *         packing - GL_TRUE = use packing params
 *                   GL_FALSE = use unpacking params.
 *         row, column - location of pixel whose address is to be returned
 * Return:  address of pixel at (row,column) in image or NULL if error.
 */
GLvoid *gl_pixel_addr_in_image( GLcontext *ctx,
                                const GLvoid *image, GLsizei width,
                                GLsizei height, GLenum format, GLenum type,
                                GLboolean packing,
                                GLint row, GLint column )
{
   GLint bytes_per_comp;   /* bytes per component */
   GLint comp_per_pixel;   /* components per pixel */
   GLint comps_per_row;    /* components per row */
   GLint pixels_per_row;   /* pixels per row */
   GLint alignment;        /* 1, 2 or 4 */
   GLint skiprows;
   GLint skippixels;
   GLubyte *pixel_addr;

   /* Compute bytes per component */
   bytes_per_comp = gl_sizeof_type( type );
   if (bytes_per_comp<0) {
      return NULL;
   }

   /* Compute number of components per pixel */
   comp_per_pixel = gl_components_in_format( format );
   if (comp_per_pixel<0) {
      return NULL;
   }

   if (packing) {
      /* Use PACKING parameters */
      alignment = ctx->Pack.Alignment;
      if (ctx->Pack.RowLength>0) {
         pixels_per_row = ctx->Pack.RowLength;
      }
      else {
         pixels_per_row = width;
      }
      skiprows = ctx->Pack.SkipRows;
      skippixels = ctx->Pack.SkipPixels;
   }
   else {
      /* Use UNPACKING parameters */
      alignment = ctx->Unpack.Alignment;
      if (ctx->Unpack.RowLength>0) {
         pixels_per_row = ctx->Unpack.RowLength;
      }
      else {
         pixels_per_row = width;
      }
      skiprows = ctx->Unpack.SkipRows;
      skippixels = ctx->Unpack.SkipPixels;
   }

   if (type==GL_BITMAP) {
      /* BITMAP data */
      GLint bytes_per_row;

      bytes_per_row = alignment
                    * CEILING( comp_per_pixel*pixels_per_row, 8*alignment );

      pixel_addr = (GLubyte *) image
                 + (skiprows + row) * bytes_per_row
                 + (skippixels + column) / 8;
   }
   else {
      /* Non-BITMAP data */

      if (bytes_per_comp>=alignment) {
	 comps_per_row = comp_per_pixel * pixels_per_row;
      }
      else {
         GLint bytes_per_row = bytes_per_comp * comp_per_pixel
                             * pixels_per_row;

	 comps_per_row = alignment / bytes_per_comp
                       * CEILING( bytes_per_row, alignment );
      }

      /* Copy/unpack pixel data to buffer */
      pixel_addr = (GLubyte *) image
                 + (skiprows + row) * comps_per_row * bytes_per_comp
                 + (skippixels + column) * comp_per_pixel * bytes_per_comp;
   }

   return (GLvoid *) pixel_addr;
}



/*
 * Unpack a 2-D image from user-supplied address, returning a pointer to
 * a new gl_image struct.
 * This function is always called by a higher-level unpack function such
 * as gl_unpack_texsubimage() or gl_unpack_bitmap().
 *
 * Input:  width, height - size in pixels
 *         components - number of components per pixel, ignored
 *                      if srctype and desttype is BITMAP.
 *         srctype - GL_UNSIGNED_BYTE .. GL_FLOAT
 *         desttype - store image as GL_UNSIGNED_BYTE, GL_FLOAT, or GL_BITMAP.
 *                    if GL_UNSIGNED_BYTE, srctype must be GL_UNSIGNED_BYTE.
 *                    if GL_BITMAP, srctype must be GL_BITMAP.
 *         interleave - if TRUE, srctype and desttype must be GL_UNSIGNED_BYTE
 *         pixels - pointer to unpacked image.
 */
struct gl_image *gl_unpack_image( GLcontext *ctx,
                                  GLint width, GLint height,
                                  GLint components, GLenum srctype,
                                  GLenum desttype,
                                  const GLvoid *pixels,
                                  GLboolean interleave )
{
   if (srctype==GL_BITMAP || desttype==GL_BITMAP) {
      struct gl_image *image;
      GLint bytes, i, width_in_bytes;
      GLubyte *buffer, *dst;

      assert( srctype==GL_BITMAP );
      assert( desttype==GL_BITMAP );

      /* Alloc dest storage */
      bytes = (width+7)/8 * height;
      if (bytes>0 && pixels!=NULL) {
         buffer = (GLubyte *) malloc( bytes );
         if (!buffer) {
            return NULL;
         }
         /* Copy/unpack pixel data to buffer */
         width_in_bytes = CEILING( width, 8 );
         dst = buffer;
         for (i=0;i<height;i++) {
            GLvoid *src = gl_pixel_addr_in_image( ctx, pixels, width, height,
                                                  GL_COLOR_INDEX, srctype,
                                                  GL_FALSE, i, 0 );
            if (!src) {
               free(buffer);
               return NULL;
            }
            MEMCPY( dst, src, width_in_bytes );
            dst += width_in_bytes;
         }
         /* Bit flipping */
         if (ctx->Unpack.LsbFirst) {
            gl_flip_bytes( buffer, bytes );
         }
      }
      else {
         /* a 'null' bitmap */
         buffer = NULL;
      }

      image = (struct gl_image *) malloc( sizeof(struct gl_image) );
      if (image) {
         image->Width = width;
         image->Height = height;
         image->Components = 0;
         image->Type = GL_BITMAP;
         image->Interleaved = GL_FALSE;
         image->Data = buffer;
      }
      else {
         free( buffer );
         return NULL;
      }
      return image;
   }
   else if (desttype==GL_UNSIGNED_BYTE) {
      struct gl_image *image;
      GLint width_in_bytes;
      GLubyte *buffer, *dst;
      GLint i;
      GLenum format;

      assert( srctype==GL_UNSIGNED_BYTE );

      width_in_bytes = width * components * sizeof(GLubyte);
      buffer = malloc( height * width_in_bytes );
      if (!buffer) {
         return NULL;
      }

      switch (components) {
         case 1:  format = GL_LUMINANCE;  break;
         case 2:  format = GL_LUMINANCE_ALPHA;  break;
         case 3:  format = GL_RGB;  break;
         case 4:  format = GL_RGBA;  break;
         default:  abort();
      }

      /* Copy/unpack pixel data to buffer */
      dst = buffer;
      for (i=0;i<height;i++) {
         GLubyte *src = (GLubyte *) gl_pixel_addr_in_image( ctx, pixels, width,
                                     height, format, srctype, GL_FALSE, i, 0 );
         if (!src) {
            free(buffer);
            return NULL;
         }
         if (interleave) {
            GLint j, k;
            for (j=0;j<width;j++) {
               for (k=0;k<components;k++) {
                  dst[k*width+j] = src[j*components+k];
               }
            }
         }
         else {
            MEMCPY( dst, src, width_in_bytes );
         }
         dst += width_in_bytes;
      }

      if (ctx->Unpack.LsbFirst) {
         gl_flip_bytes( buffer, height * width_in_bytes );
      }

      image = (struct gl_image *) malloc( sizeof(struct gl_image) );
      if (image) {
         image->Width = width;
         image->Height = height;
         image->Components = components;
         image->Type = GL_UNSIGNED_BYTE;
         image->Interleaved = interleave;
         image->Data = buffer;
      }
      else {
         free( buffer );
         return NULL;
      }
      return image;
   }
   else if (desttype==GL_FLOAT) {
      struct gl_image *image;
      GLfloat *buffer, *dst;
      GLenum format;
      GLint elems_per_row;
      GLint i, j;

      elems_per_row = width * components;
      buffer = (GLfloat *) malloc( height * elems_per_row * sizeof(GLfloat) );
      if (!buffer) {
         return NULL;
      }

      switch (components) {
         case 1:  format = GL_LUMINANCE;  break;
         case 2:  format = GL_LUMINANCE_ALPHA;  break;
         case 3:  format = GL_RGB;  break;
         case 4:  format = GL_RGBA;  break;
         default:  abort();
      }

      dst = buffer;
      for (i=0;i<height;i++) {
         GLvoid *src = gl_pixel_addr_in_image( ctx, pixels, width, height,
                                               format, srctype, GL_FALSE,
                                               i, 0 );
         if (!src) {
            free(buffer);
            return NULL;
         }

         switch (srctype) {
            case GL_UNSIGNED_BYTE:
               for (j=0;j<elems_per_row;j++) {
                  *dst++ = (GLfloat) ((GLubyte*)src)[j];
               }
               break;
            case GL_BYTE:
               for (j=0;j<elems_per_row;j++) {
                  *dst++ = (GLfloat) ((GLbyte*)src)[j];
               }
               break;
            case GL_UNSIGNED_SHORT:
               if (ctx->Unpack.SwapBytes) {
                  for (j=0;j<elems_per_row;j++) {
                     GLushort value = ((GLushort*)src)[j];
                     value = ((value >> 8) & 0xff) | ((value&0xff) << 8);
                     *dst++ = (GLfloat) value;
                  }
               }
               else {
                  for (j=0;j<elems_per_row;j++) {
                     *dst++ = (GLfloat) ((GLushort*)src)[j];
                  }
               }
               break;
            case GL_SHORT:
               if (ctx->Unpack.SwapBytes) {
                  for (j=0;j<elems_per_row;j++) {
                     GLshort value = ((GLshort*)src)[j];
                     value = ((value >> 8) & 0xff) | ((value&0xff) << 8);
                     *dst++ = (GLfloat) value;
                  }
               }
               else {
                  for (j=0;j<elems_per_row;j++) {
                     *dst++ = (GLfloat) ((GLshort*)src)[j];
                  }
               }
               break;
            case GL_UNSIGNED_INT:
               if (ctx->Unpack.SwapBytes) {
                  GLuint value;
                  for (j=0;j<elems_per_row;j++) {
                     value = ((GLuint*)src)[j];
                     value = ((value & 0xff000000) >> 24)
                           | ((value & 0x00ff0000) >> 8)
                           | ((value & 0x0000ff00) << 8)
                           | ((value & 0x000000ff) << 24);
                     *dst++ = (GLfloat) value;
                  }
               }
               else {
                  for (j=0;j<elems_per_row;j++) {
                     *dst++ = (GLfloat) ((GLuint*)src)[j];
                  }
               }
               break;
            case GL_INT:
               if (ctx->Unpack.SwapBytes) {
                  GLint value;
                  for (j=0;j<elems_per_row;j++) {
                     value = ((GLint*)src)[j];
                     value = ((value & 0xff000000) >> 24)
                           | ((value & 0x00ff0000) >> 8)
                           | ((value & 0x0000ff00) << 8)
                           | ((value & 0x000000ff) << 24);
                     *dst++ = (GLfloat) value;
                  }
               }
               else {
                  for (j=0;j<elems_per_row;j++) {
                     *dst++ = (GLfloat) ((GLint*)src)[j];
                  }
               }
               break;
            case GL_FLOAT:
               if (ctx->Unpack.SwapBytes) {
                  GLint value;
                  for (j=0;j<elems_per_row;j++) {
                     value = ((GLuint*)src)[j];
                     value = ((value & 0xff000000) >> 24)
                           | ((value & 0x00ff0000) >> 8)
                           | ((value & 0x0000ff00) << 8)
                           | ((value & 0x000000ff) << 24);
                     *dst++ = *((GLfloat*) &value);
                  }
               }
               else {
                  MEMCPY( dst, src, elems_per_row*sizeof(GLfloat) );
                  dst += elems_per_row;
               }
               break;
            default:
               abort();
         } /*switch*/
      } /*for*/

      image = (struct gl_image *) malloc( sizeof(struct gl_image) );
      if (image) {
         image->Width = width;
         image->Height = height;
         image->Components = components;
         image->Type = GL_FLOAT;
         image->Interleaved = GL_FALSE;
         image->Data = buffer;
      }
      else {
         free( buffer );
         return NULL;
      }
      return image;
   }
   else {
      abort();
   }
   return NULL;  /* never get here */
}



void gl_free_image( struct gl_image *image )
{
   if (image->Data) {
      free(image->Data);
   }
   free(image);
}
