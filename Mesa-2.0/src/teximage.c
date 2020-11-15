/* $Id: teximage.c,v 1.5 1996/09/27 01:29:57 brianp Exp $ */

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
 * $Log: teximage.c,v $
 * Revision 1.5  1996/09/27 01:29:57  brianp
 * removed unused variables, fixed cut&paste bug in color scaling
 *
 * Revision 1.4  1996/09/26 22:35:10  brianp
 * fixed a few compiler warnings from IRIX 6 -n32 and -64 compiler
 *
 * Revision 1.3  1996/09/15 14:18:55  brianp
 * now use GLframebuffer and GLvisual
 *
 * Revision 1.2  1996/09/15 01:48:58  brianp
 * removed #define NULL 0
 *
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "context.h"
#include "image.h"
#include "macros.h"
#include "pixel.h"
#include "span.h"
#include "teximage.h"
#include "types.h"



/*
 * NOTES:
 *
 * The internal texture storage convension is an array of N GLubytes
 * where N = width * height * components.  There is no padding.
 */




/*
 * Compute log base 2 of n.
 * If n isn't an exact power of two return -1.
 * If n<0 return -1.
 */
static int logbase2( int n )
{
   GLint m = n;
   GLint i = 1;
   GLint log2 = 0;

   if (n<0) {
      return -1;
   }

   while ( m > i ) {
      i *= 2;
      log2++;
   }
   if (m != n) {
      return -1;
   }
   else {
      return log2;
   }
}



/*
 * Given an internal texture format enum or 1, 2, 3, 4 return the
 * corresponding _base_ internal format:  GL_ALPHA, GL_LUMINANCE,
 * GL_LUMANCE_ALPHA, GL_INTENSITY, GL_RGB, or GL_RGBA.  Return -1 if
 * invalid enum.
 */
GLint decode_internal_format( GLint format )
{
   switch (format) {
      case GL_ALPHA:
      case GL_ALPHA4:
      case GL_ALPHA8:
      case GL_ALPHA12:
      case GL_ALPHA16:
         return GL_ALPHA;
      case 1:
      case GL_LUMINANCE:
      case GL_LUMINANCE4:
      case GL_LUMINANCE8:
      case GL_LUMINANCE12:
      case GL_LUMINANCE16:
         return GL_LUMINANCE;
      case 2:
      case GL_LUMINANCE_ALPHA:
      case GL_LUMINANCE4_ALPHA4:
      case GL_LUMINANCE6_ALPHA2:
      case GL_LUMINANCE8_ALPHA8:
      case GL_LUMINANCE12_ALPHA4:
      case GL_LUMINANCE12_ALPHA12:
      case GL_LUMINANCE16_ALPHA16:
         return GL_LUMINANCE_ALPHA;
      case GL_INTENSITY:
      case GL_INTENSITY4:
      case GL_INTENSITY8:
      case GL_INTENSITY12:
      case GL_INTENSITY16:
         return GL_INTENSITY;
      case 3:
      case GL_RGB:
      case GL_R3_G3_B2:
      case GL_RGB4:
      case GL_RGB5:
      case GL_RGB8:
      case GL_RGB10:
      case GL_RGB12:
      case GL_RGB16:
         return GL_RGB;
      case 4:
      case GL_RGBA:
      case GL_RGBA2:
      case GL_RGBA4:
      case GL_RGB5_A1:
      case GL_RGBA8:
      case GL_RGB10_A2:
      case GL_RGBA12:
      case GL_RGBA16:
         return GL_RGBA;
      default:
         return -1;  /* error */
   }
}



/*
 * Given an internal texture format enum or 1, 2, 3, 4 return the
 * corresponding _base_ internal format:  GL_ALPHA, GL_LUMINANCE,
 * GL_LUMANCE_ALPHA, GL_INTENSITY, GL_RGB, or GL_RGBA.  Return the
 * number of components for the format.  Return -1 if invalid enum.
 */
GLint components_in_intformat( GLint format )
{
   switch (format) {
      case GL_ALPHA:
      case GL_ALPHA4:
      case GL_ALPHA8:
      case GL_ALPHA12:
      case GL_ALPHA16:
         return 1;
      case 1:
      case GL_LUMINANCE:
      case GL_LUMINANCE4:
      case GL_LUMINANCE8:
      case GL_LUMINANCE12:
      case GL_LUMINANCE16:
         return 1;
      case 2:
      case GL_LUMINANCE_ALPHA:
      case GL_LUMINANCE4_ALPHA4:
      case GL_LUMINANCE6_ALPHA2:
      case GL_LUMINANCE8_ALPHA8:
      case GL_LUMINANCE12_ALPHA4:
      case GL_LUMINANCE12_ALPHA12:
      case GL_LUMINANCE16_ALPHA16:
         return 2;
      case GL_INTENSITY:
      case GL_INTENSITY4:
      case GL_INTENSITY8:
      case GL_INTENSITY12:
      case GL_INTENSITY16:
         return 1;
      case 3:
      case GL_RGB:
      case GL_R3_G3_B2:
      case GL_RGB4:
      case GL_RGB5:
      case GL_RGB8:
      case GL_RGB10:
      case GL_RGB12:
      case GL_RGB16:
         return 3;
      case 4:
      case GL_RGBA:
      case GL_RGBA2:
      case GL_RGBA4:
      case GL_RGB5_A1:
      case GL_RGBA8:
      case GL_RGB10_A2:
      case GL_RGBA12:
      case GL_RGBA16:
         return 4;
      default:
         return -1;  /* error */
   }
}



struct gl_texture_image *gl_alloc_texture_image( void )
{
   return calloc( 1, sizeof(struct gl_texture_image) );
}



void gl_free_texture_image( struct gl_texture_image *teximage )
{
   if (teximage->DeleteFlag) {
      if (teximage->Data) {
         free( teximage->Data );
      }
      free( teximage );
   }
}



/*
 * Convert the texture image given to glTexImage1D or glTexImage2D into
 * the internal texture format which is an array of GLubytes.
 * Error checking is performed on all parameters.
 * Return:  pointer to a gl_texture_image struct or NULL if error.
 */
struct gl_texture_image *gl_unpack_texture( GLcontext *ctx,
                                            GLint dimensions,
                                            GLenum target,
                                            GLint level,
                                            GLint internalformat,
                                            GLsizei width, GLsizei height,
                                            GLint border,
                                            GLenum format, GLenum type,
                                            const GLvoid *pixels )
{
   GLboolean rflag, gflag, bflag, aflag, lflag;
   GLuint elements;
   GLuint i, row;
   GLubyte *texture, *texptr;
   GLboolean scale_or_bias;
   struct gl_texture_image *teximage;
   GLint iformat;
   GLint components;

   /* Error checking */
   if (level<0 || level>=MAX_TEXTURE_LEVELS) {
      return NULL;
   }
   iformat = decode_internal_format( internalformat );
   components = components_in_intformat( internalformat );
   if (iformat==-1) {
      return NULL;
   }
   if (border!=0 && border!=1) {
      return NULL;
   }
   switch (type) {
      case GL_UNSIGNED_BYTE:
      case GL_BYTE:
      case GL_UNSIGNED_SHORT:
      case GL_SHORT:
      case GL_FLOAT:
         /* OK */
         break;
      default:
         return NULL;
   }

   if (dimensions==1) {
      if (target!=GL_TEXTURE_1D && target!=GL_PROXY_TEXTURE_1D) {
         return NULL;
      }
      if (width<2*border || width>2+MAX_TEXTURE_SIZE) {
         return NULL;
      }
      if (logbase2( width-2*border )<0) {
         return NULL;
      }
      width -= 2*border;
      assert( height==1 );
   }
   else if (dimensions==2) {
      if (target!=GL_TEXTURE_2D && target!=GL_PROXY_TEXTURE_2D) {
         return NULL;
      }
      if (height<2*border || height>2+MAX_TEXTURE_SIZE) {
         return NULL;
      }
      if (logbase2( height-2*border )<0) {
         return NULL;
      }
      width -= 2*border;
      height -= 2*border;
   }
   else {
      abort();
   }

   scale_or_bias = ctx->Pixel.RedScale  !=1.0F || ctx->Pixel.RedBias  !=0.0F
                || ctx->Pixel.GreenScale!=1.0F || ctx->Pixel.GreenBias!=0.0F
		|| ctx->Pixel.BlueScale !=1.0F || ctx->Pixel.BlueBias !=0.0F
		|| ctx->Pixel.AlphaScale!=1.0F || ctx->Pixel.AlphaBias!=0.0F;

   switch (format) {
      case GL_COLOR_INDEX:
         elements = 1;
	 rflag = gflag = bflag = aflag = lflag = GL_FALSE;
	 break;
      case GL_RED:
	 elements = 1;
	 rflag = GL_TRUE;
	 gflag = bflag = aflag = lflag = GL_FALSE;
	 break;
      case GL_GREEN:
	 elements = 1;
	 gflag = GL_TRUE;
	 rflag = bflag = aflag = lflag = GL_FALSE;
	 break;
      case GL_BLUE:
	 elements = 1;
	 bflag = GL_TRUE;
	 rflag = gflag = aflag = lflag = GL_FALSE;
	 break;
      case GL_ALPHA:
	 elements = 1;
	 aflag = GL_TRUE;
	 rflag = gflag = bflag = lflag = GL_FALSE;
	 break;
      case GL_RGB:
         elements = 3;
	 rflag = gflag = bflag = GL_TRUE;
	 aflag = lflag = GL_FALSE;
	 break;
      case GL_RGBA:
         elements = 4;
	 rflag = gflag = bflag = aflag = GL_TRUE;
	 lflag = GL_FALSE;
	 break;
      case GL_LUMINANCE:
         elements = 1;
	 rflag = gflag = bflag = aflag = GL_FALSE;
	 lflag = GL_TRUE;
	 break;
      case GL_LUMINANCE_ALPHA:
         elements = 2;
	 lflag = aflag = GL_TRUE;
	 rflag = gflag = bflag = GL_FALSE;
         break;
      default:
	 return NULL;
   }

   if (target==GL_PROXY_TEXTURE_1D || target==GL_PROXY_TEXTURE_2D) {
      texture = NULL;
   }
   else {
      /* Allocate texture memory */
      texture = (GLubyte *) malloc( width * height * components );
      if (!texture) {
         gl_error( ctx, GL_OUT_OF_MEMORY, "glTexImage1/2D" );
         return NULL;
      }
      texptr = texture;

      /* TODO: obey glPixelStore parameters! */

      /* Build texture map image row by row */
      for (row=0;row<height;row++) {
         if (type==GL_UNSIGNED_BYTE && format==GL_RGB && iformat==GL_RGB
             && !scale_or_bias) {
            /*
             * A frequent and simple case
             */
            GLubyte *src = (GLubyte *) pixels + row * width * 3;
            MEMCPY( texptr, src, 3*width );
            texptr += 3*width;
         }
         else if (type==GL_UNSIGNED_BYTE && format==GL_RGBA && iformat==GL_RGBA
             && !scale_or_bias) {
            /*
             * Another frequent and simple case
             */
            GLubyte *src = (GLubyte *) pixels + row * width * 4;
            MEMCPY( texptr, src, 4*width );
            texptr += 4*width;
         }
         else {
            /*
             * General solution
             */
            GLfloat red[MAX_TEXTURE_SIZE], green[MAX_TEXTURE_SIZE];
            GLfloat blue[MAX_TEXTURE_SIZE], alpha[MAX_TEXTURE_SIZE];

            switch (type) {
               case GL_UNSIGNED_BYTE:
                  {
                     GLubyte *src = (GLubyte*) pixels + row * width * elements;
                     for (i=0;i<width;i++) {
                        if (lflag) {
                           red[i] = green[i] = blue[i] = UBYTE_TO_FLOAT(*src++);
                        }
                        else {
                           red[i]   = rflag ? UBYTE_TO_FLOAT(*src++) : 0.0F;
                           green[i] = gflag ? UBYTE_TO_FLOAT(*src++) : 0.0F;
                           blue[i]  = bflag ? UBYTE_TO_FLOAT(*src++) : 0.0F;
                        }
                        alpha[i] = aflag ? UBYTE_TO_FLOAT(*src++) : 1.0F;
                     }
                  }
                  break;
               case GL_BYTE:
                  {
                     GLbyte *src = (GLbyte *) pixels + row * width * elements;
                     for (i=0;i<width;i++) {
                        if (lflag) {
                           red[i] = green[i] = blue[i] = BYTE_TO_FLOAT(*src++);
                        }
                        else {
                           red[i]   = rflag ? BYTE_TO_FLOAT(*src++) : 0.0F;
                           green[i] = gflag ? BYTE_TO_FLOAT(*src++) : 0.0F;
                           blue[i]  = bflag ? BYTE_TO_FLOAT(*src++) : 0.0F;
                        }
                        alpha[i] = aflag ? BYTE_TO_FLOAT(*src++) : 1.0F;
                     }
                  }
                  break;
               case GL_UNSIGNED_SHORT:
                  {
                     GLushort *src = (GLushort *) pixels + row *width*elements;
                     for (i=0;i<width;i++) {
                        if (lflag) {
                           red[i] = green[i] = blue[i]=USHORT_TO_FLOAT(*src++);
                        }
                        else {
                           red[i]   = rflag ? USHORT_TO_FLOAT(*src++) : 0.0F;
                           green[i] = gflag ? USHORT_TO_FLOAT(*src++) : 0.0F;
                           blue[i]  = bflag ? USHORT_TO_FLOAT(*src++) : 0.0F;
                        }
                        alpha[i] = aflag ? USHORT_TO_FLOAT(*src++) : 1.0F;
                     }
                  }
                  break;
               case GL_SHORT:
                  {
                     GLshort *src = (GLshort *) pixels + row * width *elements;
                     for (i=0;i<width;i++) {
                        if (lflag) {
                           red[i] = green[i] = blue[i] =SHORT_TO_FLOAT(*src++);
                        }
                        else {
                           red[i]   = rflag ? SHORT_TO_FLOAT(*src++) : 0.0F;
                           green[i] = gflag ? SHORT_TO_FLOAT(*src++) : 0.0F;
                           blue[i]  = bflag ? SHORT_TO_FLOAT(*src++) : 0.0F;
                        }
                        alpha[i] = aflag ? SHORT_TO_FLOAT(*src++) : 1.0F;
                     }
                  }
                  break;
               /*TODO: implement rest of data types */
               case GL_FLOAT:
                  {
                     GLfloat *src = (GLfloat *) pixels + row * width *elements;
                     for (i=0;i<width;i++) {
                        if (lflag) {
                           red[i] = green[i] = blue[i] = *src++;
                        }
                        else {
                           red[i]   = rflag ? *src++ : 0.0F;
                           green[i] = gflag ? *src++ : 0.0F;
                           blue[i]  = bflag ? *src++ : 0.0F;
                        }
                        alpha[i] = aflag ? *src++ : 1.0F;
                     }
                  }
                  break;
               default:
                  gl_error( ctx, GL_INVALID_ENUM, "glTexImage1/2D(type)" );
            } /* switch */

            /* apply scale and/or bias */
            if (scale_or_bias) {
               for (i=0;i<width;i++) {
                  register GLfloat r, g, b, a;
                  r = red[i]   * ctx->Pixel.RedScale   + ctx->Pixel.RedBias;
                  g = green[i] * ctx->Pixel.GreenScale + ctx->Pixel.GreenBias;
                  b = blue[i]  * ctx->Pixel.BlueScale  + ctx->Pixel.BlueBias;
                  a = alpha[i] * ctx->Pixel.AlphaScale + ctx->Pixel.AlphaBias;
                  red[i]   = CLAMP( r, 0.0F, 1.0F );
                  green[i] = CLAMP( g, 0.0F, 1.0F );
                  blue[i]  = CLAMP( b, 0.0F, 1.0F );
                  alpha[i] = CLAMP( a, 0.0F, 1.0F );
               }
            }

            /* save 8-bit components */
            switch (iformat) {
               case GL_ALPHA:
                  for (i=0;i<width;i++) {
                     *texptr++ = (GLubyte) (GLint) (alpha[i] * 255.0F);
                  }
                  break;
               case GL_LUMINANCE:
                  for (i=0;i<width;i++) {
                     *texptr++ = (GLubyte) (GLint) (red[i] * 255.0F);
                  }
                  break;
               case GL_LUMINANCE_ALPHA:
                  for (i=0;i<width;i++) {
                     *texptr++ = (GLubyte) (GLint) (red[i] * 255.0F);
                     *texptr++ = (GLubyte) (GLint) (alpha[i] * 255.0F);
                  }
                  break;
               case GL_INTENSITY:
                  for (i=0;i<width;i++) {
                     *texptr++ = (GLubyte) (GLint) (red[i] * 255.0F);
                  }
                  break;
               case GL_RGB:
                  for (i=0;i<width;i++) {
                     *texptr++ = (GLubyte) (GLint) (red[i] * 255.0F);
                     *texptr++ = (GLubyte) (GLint) (green[i] * 255.0F);
                     *texptr++ = (GLubyte) (GLint) (blue[i] * 255.0F);
                  }
                  break;
               case GL_RGBA:
                  for (i=0;i<width;i++) {
                     *texptr++ = (GLubyte) (GLint) (red[i] * 255.0F);
                     *texptr++ = (GLubyte) (GLint) (green[i] * 255.0F);
                     *texptr++ = (GLubyte) (GLint) (blue[i] * 255.0F);
                     *texptr++ = (GLubyte) (GLint) (alpha[i] * 255.0F);
                  }
                  break;
               default:
                  abort();
            } /* switch iformat */

         } /* if general solution */

      } /* for row */

   }

   /* If we get here we must have a valid, error-free texture image */
   teximage = gl_alloc_texture_image();
   if (teximage) {
      int log2_width = logbase2(width);
      int log2_height = logbase2(height);
      teximage->Format = (GLenum) iformat;
      teximage->Border = border;
      teximage->Width = width;
      teximage->Height = height;
      teximage->WidthLog2 = log2_width;
      teximage->HeightLog2 = log2_height;
      teximage->MaxLog2 = MAX2( log2_width, log2_height );
      teximage->Data = texture;
      teximage->DeleteFlag = GL_TRUE;  /* gl_save_TexImage*D may change this */
   }
   return teximage;
}



/*
 * Called when there's an error in glTexImage1D with proxy target.
 */
static void proxy_1D_error( GLcontext *ctx, GLint level )
{
   MEMSET( ctx->Texture.Proxy1D->Image[level], 0,
           sizeof(struct gl_texture_image) );
}



/*
 * Called when there's an error in glTexImage1D with proxy target.
 */
static void proxy_2D_error( GLcontext *ctx, GLint level )
{
   MEMSET( ctx->Texture.Proxy2D->Image[level], 0,
           sizeof(struct gl_texture_image) );
}



/*
 * Called from the API.  Note that width includes the border.
 */
void gl_TexImage1D( GLcontext *ctx,
                    GLenum target, GLint level, GLint internalformat,
		    GLsizei width, GLint border, GLenum format,
		    GLenum type, struct gl_texture_image *teximage )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glTexImage1D" );
      return;
   }

   if (teximage) {
      /* if teximage is not NULL then it must be valid */
      if (target==GL_TEXTURE_1D) {
         /* free current texture image, if any */
         if (ctx->Texture.Current1D->Image[level]) {
            gl_free_texture_image( ctx->Texture.Current1D->Image[level] );
         }
         /* install new texture image */
         ctx->Texture.Current1D->Image[level] = teximage;
         ctx->NewState |= NEW_TEXTURING;
      }
      else {
         /* Proxy texture: update proxy state */
         MEMCPY( ctx->Texture.Proxy1D->Image[level], teximage,
                 sizeof(struct gl_texture_image) );
      }
   }
   else {
      /* An error must have occured during texture unpacking.  Record the
       * error now.
       */
      GLint iformat;
      if (target!=GL_TEXTURE_1D && target!=GL_PROXY_TEXTURE_1D) {
         gl_error( ctx, GL_INVALID_ENUM, "glTexImage1D" );
         return;
      }
      if (level<0 || level>=MAX_TEXTURE_LEVELS) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexImage1D(level)" );
         return;
      }
      iformat = decode_internal_format( internalformat );
      if (iformat<0) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexImage1D(internalformat)" );
         if (target==GL_PROXY_TEXTURE_1D) {
            proxy_1D_error( ctx, level );
         }
         return;
      }
      if (width<2*border || width>2+MAX_TEXTURE_SIZE) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexImage1D(width)" );
         if (target==GL_PROXY_TEXTURE_1D) {
            proxy_1D_error( ctx, level );
         }
         return;
      }
      if (border!=0 && border!=1) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexImage1D(border)" );
         if (target==GL_PROXY_TEXTURE_1D) {
            proxy_1D_error( ctx, level );
         }
         return;
      }
      if (logbase2( width-2*border )<0) {
         gl_error( ctx, GL_INVALID_VALUE,
                   "glTexImage1D(width != 2^k + 2*border))");
         if (target==GL_PROXY_TEXTURE_1D) {
            proxy_1D_error( ctx, level );
         }
         return;
      }
      switch (format) {
         case GL_COLOR_INDEX:
         case GL_RED:
         case GL_GREEN:
         case GL_BLUE:
         case GL_ALPHA:
         case GL_RGB:
         case GL_RGBA:
         case GL_LUMINANCE:
         case GL_LUMINANCE_ALPHA:
            /* OK */
            break;
         default:
            gl_error( ctx, GL_INVALID_ENUM, "glTexImage1D(format)" );
            if (target==GL_PROXY_TEXTURE_1D) {
               proxy_1D_error( ctx, level );
            }
            return;
      }
      switch (type) {
         case GL_UNSIGNED_BYTE:
         case GL_BYTE:
         case GL_UNSIGNED_SHORT:
         case GL_SHORT:
         case GL_FLOAT:
            /* OK */
            break;
         default:
            gl_error( ctx, GL_INVALID_ENUM, "glTexImage1D(type)" );
            if (target==GL_PROXY_TEXTURE_1D) {
               proxy_1D_error( ctx, level );
            }
            return;
      }
   }
}




/*
 * Called by the API or display list executor.
 * Note that width and height include the border.
 */
void gl_TexImage2D( GLcontext *ctx,
                    GLenum target, GLint level, GLint internalformat,
                    GLsizei width, GLsizei height, GLint border,
                    GLenum format, GLenum type,
                    struct gl_texture_image *teximage )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glTexImage2D" );
      return;
   }

   if (teximage) {
      /* if teximage is not NULL then it must be valid */
      if (target==GL_TEXTURE_2D) {
         /* free current texture image, if any */
         if (ctx->Texture.Current2D->Image[level]) {
            gl_free_texture_image( ctx->Texture.Current2D->Image[level] );
         }
         /* install new texture image */
         ctx->Texture.Current2D->Image[level] = teximage;
         ctx->NewState |= NEW_TEXTURING;
      }
      else {
         /* Proxy texture: update proxy state */
         MEMCPY( ctx->Texture.Proxy1D->Image[level], teximage,
                 sizeof(struct gl_texture_image) );
      }
   }
   else {
      /* An error must have occured during texture unpacking.  Record the
       * error now.
       */
      GLint iformat;
      if (target!=GL_TEXTURE_2D && target!=GL_PROXY_TEXTURE_2D) {
         gl_error( ctx, GL_INVALID_ENUM, "glTexImage2D(target)" );
         return;
      }
      if (level<0 || level>=MAX_TEXTURE_LEVELS) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexImage2D(level)" );
         return;
      }
      iformat = decode_internal_format( internalformat );
      if (iformat<0) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexImage2D(internalformat)" );
         if (target==GL_PROXY_TEXTURE_2D) {
            proxy_2D_error( ctx, level );
         }
         return;
      }
      if (width<2*border || width>2+MAX_TEXTURE_SIZE) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexImage2D(width)" );
         if (target==GL_PROXY_TEXTURE_2D) {
            proxy_2D_error( ctx, level );
         }
         return;
      }
      if (height<2*border || height>2+MAX_TEXTURE_SIZE) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexImage2D(height)" );
         if (target==GL_PROXY_TEXTURE_2D) {
            proxy_2D_error( ctx, level );
         }
         return;
      }
      if (border!=0 && border!=1) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexImage2D(border)" );
         if (target==GL_PROXY_TEXTURE_2D) {
            proxy_2D_error( ctx, level );
         }
         return;
      }
      if (logbase2( width-2*border )<0) {
         gl_error( ctx,GL_INVALID_VALUE,
                   "glTexImage2D(width != 2^k + 2*border))");
         if (target==GL_PROXY_TEXTURE_2D) {
            proxy_2D_error( ctx, level );
         }
         return;
      }
      if (logbase2( height-2*border )<0) {
         gl_error( ctx,GL_INVALID_VALUE,
                   "glTexImage2D(height != 2^k + 2*border))");
         if (target==GL_PROXY_TEXTURE_2D) {
            proxy_2D_error( ctx, level );
         }
         return;
      }
      switch (format) {
         case GL_COLOR_INDEX:
         case GL_RED:
         case GL_GREEN:
         case GL_BLUE:
         case GL_ALPHA:
         case GL_RGB:
         case GL_RGBA:
         case GL_LUMINANCE:
         case GL_LUMINANCE_ALPHA:
            /* OK */
            break;
         default:
            gl_error( ctx, GL_INVALID_ENUM, "glTexImage2D(format)" );
            if (target==GL_PROXY_TEXTURE_2D) {
               proxy_2D_error( ctx, level );
            }
            return;
      }
      switch (type) {
         case GL_UNSIGNED_BYTE:
         case GL_BYTE:
         case GL_UNSIGNED_SHORT:
         case GL_SHORT:
         case GL_FLOAT:
            /* OK */
            break;
         default:
            gl_error( ctx, GL_INVALID_ENUM, "glTexImage2D(type)" );
            if (target==GL_PROXY_TEXTURE_2D) {
               proxy_2D_error( ctx, level );
            }
            return;
      }
   }
}



void gl_GetTexImage( GLcontext *ctx, GLenum target, GLint level, GLenum format,
                     GLenum type, GLvoid *pixels )
{
   /* TODO */
}





/*
 * GL_EXT_copy_texture
 */



/*
 * Unpack the image data given to glTexSubImage[12]D.
 */
struct gl_image *
gl_unpack_texsubimage( GLcontext *ctx, GLint width, GLint height,
                       GLenum format, GLenum type, const GLvoid *pixels )
{
   GLint components;
   GLenum desttype;

   if (width<0 || height<0 || !pixels) {
      return NULL;
   }

   components = components_in_intformat( format );
   if (components<0 || format==GL_STENCIL_INDEX || format==GL_DEPTH_COMPONENT){
      return NULL;
   }

   if (gl_sizeof_type(type)<=0) {
      return NULL;
   }

   if (type==GL_UNSIGNED_BYTE) {
      desttype = GL_UNSIGNED_BYTE;
   }
   else {
      desttype = GL_FLOAT;
   }
   
   return gl_unpack_image( ctx, width, height, components, type,
                           desttype, pixels, GL_FALSE );
}






void gl_TexSubImage1D( GLcontext *ctx,
                       GLenum target, GLint level, GLint xoffset,
                       GLsizei width, GLenum format, GLenum type,
                       struct gl_image *image )
{
   struct gl_texture_image *teximage;

   if (target!=GL_TEXTURE_1D) {
      gl_error( ctx, GL_INVALID_ENUM, "glTexSubImage1D(target)" );
      return;
   }
   if (level<0 || level>=MAX_TEXTURE_LEVELS) {
      gl_error( ctx, GL_INVALID_ENUM, "glTexSubImage1D(level)" );
      return;
   }

   teximage = ctx->Texture.Current1D->Image[level];
   if (!teximage) {
      gl_error( ctx, GL_INVALID_OPERATION, "glTexSubImage1D" );
      return;
   }

   if (xoffset < -teximage->Border) {
      gl_error( ctx, GL_INVALID_VALUE, "glTexSubImage1D(xoffset)" );
      return;
   }
   if (xoffset + width > teximage->Width+teximage->Border) {
      gl_error( ctx, GL_INVALID_VALUE, "glTexSubImage1D(xoffset+width)" );
      return;
   }

   if (image) {
      /* unpacking must have been error-free */
      GLint texcomponents, i, k;
      GLubyte *dst, *src;

      /* TODO: this is temporary.  If Type==GL_FLOAT or scale&bias needed */
      /* then do more work. */
	  if (image->Type==GL_FLOAT) {
		  gl_warning( ctx, "unimplemented texture type in glTexSubImage1D" );
		  return;
	  }

      texcomponents = components_in_intformat(teximage->Format);

      dst = teximage->Data + texcomponents * xoffset;

      if (texcomponents == image->Components) {
         MEMCPY( dst, image->Data, width * texcomponents );
      }
      else {
         /* TODO: this is a hack */
         gl_warning( ctx, "component mismatch in glTexSubImage1D" );
         for (i=0;i<width;i++) {
            for (k=0;k<texcomponents;k++) {
               dst[k] = src[k];
            }
            dst += texcomponents;
            src += image->Components;
         }
      }
   }
   else {
      /* if no image, an error must have occured, do more testing now */
      GLint components, size;

      if (width<0) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexSubImage1D(width)" );
         return;
      }
      components = components_in_intformat( format );
      if (components<0 || format==GL_STENCIL_INDEX
          || format==GL_DEPTH_COMPONENT){
         gl_error( ctx, GL_INVALID_ENUM, "glTexSubImage1D(format)" );
         return;
      }
      size = gl_sizeof_type( type );
      if (size<=0) {
         gl_error( ctx, GL_INVALID_ENUM, "glTexSubImage1D(type)" );
         return;
      }
      /* if we get here, probably ran out of memory during unpacking */
      gl_error( ctx, GL_OUT_OF_MEMORY, "glTexSubImage1D" );
   }
}



void gl_TexSubImage2D( GLcontext *ctx,
                       GLenum target, GLint level,
                       GLint xoffset, GLint yoffset,
                       GLsizei width, GLsizei height,
                       GLenum format, GLenum type,
                       struct gl_image *image )
{
   struct gl_texture_image *teximage;

   if (target!=GL_TEXTURE_2D) {
      gl_error( ctx, GL_INVALID_ENUM, "glTexSubImage2D(target)" );
      return;
   }
   if (level<0 || level>=MAX_TEXTURE_LEVELS) {
      gl_error( ctx, GL_INVALID_ENUM, "glTexSubImage2D(level)" );
      return;
   }

   teximage = ctx->Texture.Current2D->Image[level];
   if (!teximage) {
      gl_error( ctx, GL_INVALID_OPERATION, "glTexSubImage2D" );
      return;
   }

   if (xoffset < -teximage->Border) {
      gl_error( ctx, GL_INVALID_VALUE, "glTexSubImage2D(xoffset)" );
      return;
   }
   if (yoffset < -teximage->Border) {
      gl_error( ctx, GL_INVALID_VALUE, "glTexSubImage2D(yoffset)" );
      return;
   }
   if (xoffset + width > teximage->Width+teximage->Border) {
      gl_error( ctx, GL_INVALID_VALUE, "glTexSubImage2D(xoffset+width)" );
      return;
   }
   if (yoffset + height > teximage->Height+teximage->Border) {
      gl_error( ctx, GL_INVALID_VALUE, "glTexSubImage2D(yoffset+height)" );
      return;
   }

   if (image) {
      /* unpacking must have been error-free */
      GLint texcomponents, i, j, k;
      GLubyte *dst, *src;

      /* TODO: this is temporary.  If Type==GL_FLOAT or scale&bias needed */
      /* then do more work. */
	  if (image->Type==GL_FLOAT) {
		  gl_warning( ctx, "unimplemented texture type in glTexSubImage2D" );
		  return;
	  }

      texcomponents = components_in_intformat(teximage->Format);

      if (texcomponents == image->Components) {
         dst = teximage->Data 
               + (yoffset * teximage->Width + xoffset) * texcomponents;
         src = image->Data;
         for (j=0;j<height;j++) {
            MEMCPY( dst, src, width * texcomponents );
            dst += teximage->Width * texcomponents;
            src += width * texcomponents;
         }
      }
      else {
         /* TODO: this is a hack */
         gl_warning( ctx, "component mismatch in glTexSubImage2D" );

         for (j=0;j<height;j++) {
            dst = teximage->Data 
               + ((yoffset+j) * teximage->Width + xoffset) * texcomponents;
            src = (GLubyte *) image->Data + j * width * image->Components;
            for (i=0;i<width;i++) {
               for (k=0;k<texcomponents;k++) {
                  dst[k] = src[k];
               }
               dst += texcomponents;
               src += image->Components;
            }
         }
      }
   }
   else {
      /* if no image, an error must have occured, do more testing now */
      GLint components, size;

      if (width<0) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexSubImage2D(width)" );
         return;
      }
      if (height<0) {
         gl_error( ctx, GL_INVALID_VALUE, "glTexSubImage2D(height)" );
         return;
      }
      components = components_in_intformat( format );
      if (components<0 || format==GL_STENCIL_INDEX
          || format==GL_DEPTH_COMPONENT){
         gl_error( ctx, GL_INVALID_ENUM, "glTexSubImage2D(format)" );
         return;
      }
      size = gl_sizeof_type( type );
      if (size<=0) {
         gl_error( ctx, GL_INVALID_ENUM, "glTexSubImage2D(type)" );
         return;
      }
      /* if we get here, probably ran out of memory during unpacking */
      gl_error( ctx, GL_OUT_OF_MEMORY, "glTexSubImage2D" );
   }
}





/*
 * Read pixels from the color buffer to make a new texture image.
 * This does the real work of glCopyTexImage[12]D().
 * Return pointer to new gl_texture_image struct or NULL if we run
 * out of memory.
 */
static struct gl_texture_image *
read_texture_image( GLcontext *ctx, GLint x, GLint y,
                    GLsizei width, GLsizei height, GLint border,
                    GLint format )
{
   struct gl_texture_image *teximage;
   GLubyte *texture, *texptr;
   GLint components;
   GLint i, j;

   components = components_in_intformat( format );

   /* allocate texel space */
   texture = (GLubyte *) malloc( width * height * components );
   if (!texture) {
      gl_error( ctx, GL_OUT_OF_MEMORY, "glCopyTexImage2D" );
      return NULL;
   }
   texptr = texture;

   /* Select buffer to read from */
   (void) (*ctx->Driver.SetBuffer)( ctx, ctx->Pixel.ReadBuffer );

   for (j=0;j<height;j++) {
      GLubyte red[MAX_WIDTH], green[MAX_WIDTH];
      GLubyte blue[MAX_WIDTH], alpha[MAX_WIDTH];
      gl_read_color_span( ctx, width, x, y+j, red, green, blue, alpha );

      if (!ctx->Visual->EightBitColor) {
         /* scale red, green, blue, alpha values to range [0,255] */
         GLfloat rscale = 255.0f * ctx->Visual->InvRedScale;
         GLfloat gscale = 255.0f * ctx->Visual->InvGreenScale;
         GLfloat bscale = 255.0f * ctx->Visual->InvBlueScale;
         GLfloat ascale = 255.0f * ctx->Visual->InvAlphaScale;
         for (i=0;i<width;i++) {
            red[i]   = (GLubyte) (GLint) (red[i]   * rscale);
            green[i] = (GLubyte) (GLint) (green[i] * gscale);
            blue[i]  = (GLubyte) (GLint) (blue[i]  * bscale);
            alpha[i] = (GLubyte) (GLint) (alpha[i] * ascale);
         }
      }

      switch (format) {
         case GL_ALPHA:
            for (i=0;i<width;i++) {
               *texptr++ = alpha[i];
            }
            break;
         case GL_LUMINANCE:
            for (i=0;i<width;i++) {
               *texptr++ = red[i];
            }
            break;
         case GL_LUMINANCE_ALPHA:
            for (i=0;i<width;i++) {
               *texptr++ = red[i];
               *texptr++ = alpha[i];
            }
            break;
         case GL_INTENSITY:
            for (i=0;i<width;i++) {
               *texptr++ = red[i];
            }
            break;
         case GL_RGB:
            for (i=0;i<width;i++) {
               *texptr++ = red[i];
               *texptr++ = green[i];
               *texptr++ = blue[i];
            }
            break;
         case GL_RGBA:
            for (i=0;i<width;i++) {
               *texptr++ = red[i];
               *texptr++ = green[i];
               *texptr++ = blue[i];
               *texptr++ = alpha[i];
            }
            break;
      } /*switch*/

   } /*for*/         

   /* Restore drawing buffer */
   (void) (*ctx->Driver.SetBuffer)( ctx, ctx->Color.DrawBuffer );

   teximage = gl_alloc_texture_image();
   if (teximage) {
      int log2_width = logbase2(width);
      int log2_height = logbase2(height);
      teximage->Format = (GLenum) format;
      teximage->Border = border;
      teximage->Width = width;
      teximage->Height = height;
      teximage->WidthLog2 = log2_width;
      teximage->HeightLog2 = log2_height;
      teximage->MaxLog2 = MAX2( log2_width, log2_height );
      teximage->Data = texture;
      teximage->DeleteFlag = GL_TRUE;
   }

   return teximage;
}





void gl_CopyTexImage1D( GLcontext *ctx,
                        GLenum target, GLint level,
                        GLenum internalformat,
                        GLint x, GLint y,
                        GLsizei width, GLint border )
{
   GLint format;
   struct gl_texture_image *teximage;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glCopyTexImage1D" );
      return;
   }
   if (target!=GL_TEXTURE_1D) {
      gl_error( ctx, GL_INVALID_ENUM, "glCopyTexImage1D(target)" );
      return;
   }
   if (level<0 || level>=MAX_TEXTURE_LEVELS) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexImage1D(level)" );
      return;
   }
   if (border!=0 && border!=1) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexImage1D(border)" );
      return;
   }
   if (width<2*border || width>2+MAX_TEXTURE_SIZE || width<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexImage1D(width)" );
      return;
   }
   format = decode_internal_format( internalformat );
   if (format<0 || (internalformat>=1 && internalformat<=4)) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexImage1D(format)" );
      return;
   }

   teximage = read_texture_image( ctx, x, y, width, 1, border, format );

   gl_TexImage1D( ctx, target, level, internalformat, width,
                  border, GL_RGBA, GL_UNSIGNED_BYTE, teximage );
}



void gl_CopyTexImage2D( GLcontext *ctx,
                        GLenum target, GLint level, GLenum internalformat,
                        GLint x, GLint y, GLsizei width, GLsizei height,
                        GLint border )
{
   GLint format;
   struct gl_texture_image *teximage;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glCopyTexImage2D" );
      return;
   }
   if (target!=GL_TEXTURE_2D) {
      gl_error( ctx, GL_INVALID_ENUM, "glCopyTexImage2D(target)" );
      return;
   }
   if (level<0 || level>=MAX_TEXTURE_LEVELS) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexImage2D(level)" );
      return;
   }
   if (border!=0 && border!=1) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexImage2D(border)" );
      return;
   }
   if (width<2*border || width>2+MAX_TEXTURE_SIZE || width<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexImage2D(width)" );
      return;
   }
   if (height<2*border || height>2+MAX_TEXTURE_SIZE || height<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexImage2D(height)" );
      return;
   }
   format = decode_internal_format( internalformat );
   if (format<0 || (internalformat>=1 && internalformat<=4)) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexImage2D(format)" );
      return;
   }

   teximage = read_texture_image( ctx, x, y, width, height, border, format );

   gl_TexImage2D( ctx, target, level, internalformat, width, height,
                  border, GL_RGBA, GL_UNSIGNED_BYTE, teximage );
}




/*
 * Do the work of glCopyTexSubImage[12]D.
 */
static void copy_tex_sub_image( GLcontext *ctx, struct gl_texture_image *dest,
                                GLint width, GLint height,
                                GLint srcx, GLint srcy,
                                GLint dstx, GLint dsty )
{
   GLint i, j;
   GLint format, components;

   format = dest->Format;
   components = components_in_intformat( format );

   for (j=0;j<height;j++) {
      GLubyte red[MAX_WIDTH], green[MAX_WIDTH];
      GLubyte blue[MAX_WIDTH], alpha[MAX_WIDTH];
      GLubyte *texptr;

      gl_read_color_span( ctx, width, srcx, srcy+j, red, green, blue, alpha );

      if (!ctx->Visual->EightBitColor) {
         /* scale red, green, blue, alpha values to range [0,255] */
         GLfloat rscale = 255.0f * ctx->Visual->InvRedScale;
         GLfloat gscale = 255.0f * ctx->Visual->InvGreenScale;
         GLfloat bscale = 255.0f * ctx->Visual->InvBlueScale;
         GLfloat ascale = 255.0f * ctx->Visual->InvAlphaScale;
         for (i=0;i<width;i++) {
            red[i]   = (GLubyte) (GLint) (red[i]   * rscale);
            green[i] = (GLubyte) (GLint) (green[i] * gscale);
            blue[i]  = (GLubyte) (GLint) (blue[i]  * bscale);
            alpha[i] = (GLubyte) (GLint) (alpha[i] * ascale);
         }
      }

      texptr = dest->Data + ((dsty+j) * width + dstx) * components;

      switch (format) {
         case GL_ALPHA:
            for (i=0;i<width;i++) {
               *texptr++ = alpha[i];
            }
            break;
         case GL_LUMINANCE:
            for (i=0;i<width;i++) {
               *texptr++ = red[i];
            }
            break;
         case GL_LUMINANCE_ALPHA:
            for (i=0;i<width;i++) {
               *texptr++ = red[i];
               *texptr++ = alpha[i];
            }
            break;
         case GL_INTENSITY:
            for (i=0;i<width;i++) {
               *texptr++ = red[i];
            }
            break;
         case GL_RGB:
            for (i=0;i<width;i++) {
               *texptr++ = red[i];
               *texptr++ = green[i];
               *texptr++ = blue[i];
            }
            break;
         case GL_RGBA:
            for (i=0;i<width;i++) {
               *texptr++ = red[i];
               *texptr++ = green[i];
               *texptr++ = blue[i];
               *texptr++ = alpha[i];
            }
            break;
      } /*switch*/
   } /*for*/         
}




void gl_CopyTexSubImage1D( GLcontext *ctx,
                              GLenum target, GLint level,
                              GLint xoffset, GLint x, GLint y, GLsizei width )
{
   struct gl_texture_image *teximage;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glCopyTexSubImage1D" );
      return;
   }
   if (target!=GL_TEXTURE_1D) {
      gl_error( ctx, GL_INVALID_ENUM, "glCopyTexSubImage1D(target)" );
      return;
   }
   if (level<0 || level>=MAX_TEXTURE_LEVELS) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexSubImage1D(level)" );
      return;
   }
   if (width<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexSubImage1D(width)" );
      return;
   }

   teximage = ctx->Texture.Current1D->Image[level];

   if (teximage) {
      if (xoffset < -teximage->Border) {
         gl_error( ctx, GL_INVALID_VALUE, "glCopyTexSubImage1D(xoffset)" );
         return;
      }
      /* NOTE: we're adding the border here, not subtracting! */
      if (xoffset+width > teximage->Width+teximage->Border) {
         gl_error( ctx, GL_INVALID_VALUE,
                   "glCopyTexSubImage1D(xoffset+width)" );
         return;
      }
      if (teximage->Data) {
         copy_tex_sub_image( ctx, teximage, width, 1, x, y, xoffset, 0 );
      }
   }
   else {
      gl_error( ctx, GL_INVALID_OPERATION, "glCopyTexSubImage1D" );
   }
}



void gl_CopyTexSubImage2D( GLcontext *ctx,
                              GLenum target, GLint level,
                              GLint xoffset, GLint yoffset,
                              GLint x, GLint y, GLsizei width, GLsizei height )
{
   struct gl_texture_image *teximage;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glCopyTexSubImage2D" );
      return;
   }
   if (target!=GL_TEXTURE_2D) {
      gl_error( ctx, GL_INVALID_ENUM, "glCopyTexSubImage2D(target)" );
      return;
   }
   if (level<0 || level>=MAX_TEXTURE_LEVELS) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexSubImage2D(level)" );
      return;
   }
   if (width<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexSubImage2D(width)" );
      return;
   }
   if (height<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyTexSubImage2D(height)" );
      return;
   }

   teximage = ctx->Texture.Current2D->Image[level];

   if (teximage) {
      if (xoffset < -teximage->Border) {
         gl_error( ctx, GL_INVALID_VALUE, "glCopyTexSubImage2D(xoffset)" );
         return;
      }
      if (yoffset < -teximage->Border) {
         gl_error( ctx, GL_INVALID_VALUE, "glCopyTexSubImage2D(yoffset)" );
         return;
      }
      /* NOTE: we're adding the border here, not subtracting! */
      if (xoffset+width > teximage->Width+teximage->Border) {
         gl_error( ctx, GL_INVALID_VALUE,
                   "glCopyTexSubImage2D(xoffset+width)" );
         return;
      }
      if (yoffset+height > teximage->Height+teximage->Border) {
         gl_error( ctx, GL_INVALID_VALUE,
                   "glCopyTexSubImage2D(yoffset+height)" );
         return;
      }

      if (teximage->Data) {
         copy_tex_sub_image( ctx, teximage, width, height,
                             x, y, xoffset, yoffset );
      }
   }
   else {
      gl_error( ctx, GL_INVALID_OPERATION, "glCopyTexSubImage2D" );
   }
}
