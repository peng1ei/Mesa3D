/* $Id: osmesa.c,v 1.7 1996/10/01 03:30:48 brianp Exp $ */

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
 * $Log: osmesa.c,v $
 * Revision 1.7  1996/10/01 03:30:48  brianp
 * use new FixedToDepth() macro
 *
 * Revision 1.6  1996/10/01 01:43:21  brianp
 * added extra braces to the INNER_LOOP triangle macros
 *
 * Revision 1.5  1996/09/27 01:32:37  brianp
 * removed unused variables
 *
 * Revision 1.4  1996/09/19 03:17:28  brianp
 * now just one parameter to gl_create_framebuffer()
 *
 * Revision 1.3  1996/09/15 14:28:16  brianp
 * now use GLframebuffer and GLvisual
 *
 * Revision 1.2  1996/09/14 20:20:11  brianp
 * misc bug fixes from Randy Frank
 *
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */



/*
 * Off-Screen Mesa rendering / Rendering into client memory space
 */


#include <stdlib.h>
#include <string.h>
#include "GL/osmesa.h"
#include "context.h"
#include "depth.h"
#include "macros.h"
#include "matrix.h"
#include "types.h"
#include "vb.h"


struct osmesa_context {
   GLcontext *gl_ctx;		/* The core GL/Mesa context */
   GLvisual *gl_visual;		/* Describes the buffers */
   GLframebuffer *gl_buffer;	/* Depth, stencil, accum, etc buffers */
   GLenum format;		/* either GL_RGBA or GL_COLOR_INDEX */
   void *buffer;		/* the image buffer */
   GLint width, height;		/* size of image buffer */
   GLuint pixel;		/* current color index or RGBA pixel value */
   GLuint clearpixel;		/* pixel for clearing the color buffer */
   GLint rowlength;		/* number of pixels per row */
   GLint rshift, gshift;	/* bit shifts for RGBA formats */
   GLint bshift, ashift;
   void *rowaddr[MAX_HEIGHT];	/* address of first pixel in each image row */
   GLboolean yup;		/* TRUE  -> Y increases upward */
				/* FALSE -> Y increases downward */
};



#ifdef THREADS
   /* A context handle for each thread */
   /* TODO: an array/table of contexts indexed by thread IDs */
#else
   /* One current context for address space, all threads */
   static OSMesaContext Current = NULL;
#endif



/* A forward declaration: */
static void osmesa_setup_DD_pointers( GLcontext *ctx );




/*
 * Create an Off-Screen Mesa rendering context.  The only attribute needed is
 * an RGBA vs Color-Index mode flag.
 *
 * Input:  format - either GL_RGBA or GL_COLOR_INDEX
 *         sharelist - specifies another OSMesaContext with which to share
 *                     display lists.  NULL indicates no sharing.
 * Return:  an OSMesaContext or 0 if error
 */
OSMesaContext OSMesaCreateContext( GLenum format, OSMesaContext sharelist )
{
   OSMesaContext osmesa;
   GLfloat rscale, gscale, bscale, ascale;
   GLint rshift, gshift, bshift, ashift;
   GLint index_bits;
   GLboolean rgbmode;
   GLuint i4 = 1;
   GLubyte *i1 = (GLubyte *) &i4;
   GLint little_endian = *i1;

   if (format==OSMESA_COLOR_INDEX) {
      rscale = gscale = bscale = ascale = 0.0;
      index_bits = 8;
      rshift = gshift = bshift = ashift = 0;
      rgbmode = GL_FALSE;
   }
   else if (format==OSMESA_RGBA) {
      rscale = gscale = bscale = ascale = 255.0;
      index_bits = 0;
      if (little_endian) {
         rshift = 0;
         gshift = 8;
         bshift = 16;
         ashift = 24;
      }
      else {
         rshift = 24;
         gshift = 16;
         bshift = 8;
         ashift = 0;
      }
      rgbmode = GL_TRUE;
   }
   else if (format==OSMESA_BGRA) {
      rscale = gscale = bscale = ascale = 255.0;
      index_bits = 0;
      if (little_endian) {
         ashift = 0;
         rshift = 8;
         gshift = 16;
         bshift = 24;
      }
      else {
         bshift = 24;
         gshift = 16;
         rshift = 8;
         ashift = 0;
      }
      rgbmode = GL_TRUE;
   }
   else if (format==OSMESA_ARGB) {
      rscale = gscale = bscale = ascale = 255.0;
      index_bits = 0;
      if (little_endian) {
         bshift = 0;
         gshift = 8;
         rshift = 16;
         ashift = 24;
      }
      else {
         ashift = 24;
         rshift = 16;
         gshift = 8;
         bshift = 0;
      }
      rgbmode = GL_TRUE;
   }
   else {
      return NULL;
   }


   osmesa = (OSMesaContext) calloc( 1, sizeof(struct osmesa_context) );
   if (osmesa) {
      osmesa->gl_visual = gl_create_visual( rgbmode,
                                            GL_FALSE,	/* software alpha */
                                            GL_FALSE,	/* db_flag */
                                            16,		/* depth_bits */
                                            8,		/* stencil_bits */
                                            8,		/* accum_bits */
                                            index_bits,
                                            rscale, gscale, bscale, ascale );
      if (!osmesa->gl_visual) {
         return NULL;
      }

      osmesa->gl_ctx = gl_create_context( osmesa->gl_visual,
                                          sharelist ? sharelist->gl_ctx : NULL,
                                          (void *) osmesa );
      if (!osmesa->gl_ctx) {
         gl_destroy_visual( osmesa->gl_visual );
         free(osmesa);
         return NULL;
      }
      osmesa->gl_buffer = gl_create_framebuffer( osmesa->gl_visual );
      if (!osmesa->gl_buffer) {
         gl_destroy_visual( osmesa->gl_visual );
         gl_destroy_context( osmesa->gl_ctx );
         free(osmesa);
         return NULL;
      }
      osmesa->format = format;
      osmesa->buffer = NULL;
      osmesa->width = 0;
      osmesa->height = 0;
      osmesa->pixel = 0;
      osmesa->clearpixel = 0;
      osmesa->rowlength = 0;
      osmesa->yup = GL_TRUE;
      osmesa->rshift = rshift;
      osmesa->gshift = gshift;
      osmesa->bshift = bshift;
      osmesa->ashift = ashift;
   }
   return osmesa;
}



/*
 * Destroy an Off-Screen Mesa rendering context.
 *
 * Input:  ctx - the context to destroy
 */
void OSMesaDestroyContext( OSMesaContext ctx )
{
   if (ctx) {
      gl_destroy_visual( ctx->gl_visual );
      gl_destroy_framebuffer( ctx->gl_buffer );
      gl_destroy_context( ctx->gl_ctx );
      free( ctx );
   }
}



/*
 * Recompute the values of the context's rowaddr array.
 */
static void compute_row_addresses( OSMesaContext ctx )
{
   GLint i;

   if (ctx->yup) {
      /* Y=0 is bottom line of window */
      if (ctx->format==OSMESA_COLOR_INDEX) {
         /* 1-byte CI mode */
         GLubyte *origin = (GLubyte *) ctx->buffer;
         for (i=0;i<MAX_HEIGHT;i++) {
            ctx->rowaddr[i] = origin + i * ctx->rowlength;
         }
      }
      else {
         /* 4-byte RGBA mode */
         GLuint *origin = (GLuint *) ctx->buffer;
         for (i=0;i<MAX_HEIGHT;i++) {
            ctx->rowaddr[i] = origin + i * ctx->rowlength;
         }
      }
   }
   else {
      /* Y=0 is top line of window */
      if (ctx->format==OSMESA_COLOR_INDEX) {
         /* 1-byte CI mode */
         GLubyte *origin = (GLubyte *) ctx->buffer;
         for (i=0;i<MAX_HEIGHT;i++) {
            ctx->rowaddr[i] = origin + (ctx->height-i-1) * ctx->rowlength;
         }
      }
      else {
         /* 4-byte RGBA mode */
         GLuint *origin = (GLuint *) ctx->buffer;
         for (i=0;i<MAX_HEIGHT;i++) {
            ctx->rowaddr[i] = origin + (ctx->height-i-1) * ctx->rowlength;
         }
      }
   }
}


/*
 * Bind an OSMesaContext to an image buffer.  The image buffer is just a
 * block of memory which the client provides.  Its size must be at least
 * as large as width*height*sizeof(type).  Its address should be a multiple
 * of 4 if using RGBA mode.
 *
 * Image data is stored in the order of glDrawPixels:  row-major order
 * with the lower-left image pixel stored in the first array position
 * (ie. bottom-to-top).
 *
 * Since the only type initially supported is GL_UNSIGNED_BYTE, if the
 * context is in RGBA mode, each pixel will be stored as a 4-byte RGBA
 * value.  If the context is in color indexed mode, each pixel will be
 * stored as a 1-byte value.
 *
 * If the context's viewport hasn't been initialized yet, it will now be
 * initialized to (0,0,width,height).
 *
 * Input:  ctx - the rendering context
 *         buffer - the image buffer memory
 *         type - data type for pixel components, only GL_UNSIGNED_BYTE
 *                supported now
 *         width, height - size of image buffer in pixels, at least 1
 * Return:  GL_TRUE if success, GL_FALSE if error because of invalid ctx,
 *          invalid buffer address, type!=GL_UNSIGNED_BYTE, width<1, height<1,
 *          width>internal limit or height>internal limit.
 */
GLboolean OSMesaMakeCurrent( OSMesaContext ctx, void *buffer, GLenum type,
                             GLsizei width, GLsizei height )
{
   if (!ctx || !buffer || type!=GL_UNSIGNED_BYTE
       || width<1 || height<1 || width>MAX_WIDTH || height>MAX_HEIGHT) {
      return GL_FALSE;
   }

   gl_make_current( ctx->gl_ctx, ctx->gl_buffer );

   ctx->buffer = buffer;
   ctx->width = width;
   ctx->height = height;
   if (ctx->rowlength==0) {
      ctx->rowlength = width;
   }

   osmesa_setup_DD_pointers( ctx->gl_ctx );

#ifdef THREADS
   /* Set current context for the calling thread */
   /* TODO */
#else
   /* Set current context for the address space, all threads */
   Current = ctx;
#endif

   compute_row_addresses( ctx );

   /* init viewport */
   if (ctx->gl_ctx->Viewport.Width==0) {
      /* initialize viewport and scissor box to buffer size */
      gl_Viewport( ctx->gl_ctx, 0, 0, width, height );
      ctx->gl_ctx->Scissor.Width = width;
      ctx->gl_ctx->Scissor.Height = height;
   }

   return GL_TRUE;
}




OSMesaContext OSMesaGetCurrentContext( void )
{
#ifdef THREADS
   /* Return current handle for the calling thread */
#else
   /* Return current handle for the address space, all threads */
   return Current;
#endif
}



void OSMesaPixelStore( GLint pname, GLint value )
{
   OSMesaContext ctx = OSMesaGetCurrentContext();

   switch (pname) {
      case OSMESA_ROW_LENGTH:
         if (value<0) {
            gl_error( ctx->gl_ctx, GL_INVALID_VALUE,
                      "OSMesaPixelStore(value)" );
            return;
         }
         ctx->rowlength = value;
         break;
      case OSMESA_Y_UP:
         ctx->yup = value ? GL_TRUE : GL_FALSE;
         break;
      default:
         gl_error( ctx->gl_ctx, GL_INVALID_ENUM, "OSMesaPixelStore(pname)" );
         return;
   }

   compute_row_addresses( ctx );
}


void OSMesaGetIntegerv( GLint pname, GLint *value )
{
   OSMesaContext ctx = OSMesaGetCurrentContext();

   switch (pname) {
      case OSMESA_WIDTH:
         *value = ctx->width;
         return;
      case OSMESA_HEIGHT:
         *value = ctx->height;
         return;
      case OSMESA_FORMAT:
         *value = ctx->format;
         return;
      case OSMESA_TYPE:
         *value = GL_UNSIGNED_BYTE;
         return;
      case OSMESA_ROW_LENGTH:
         *value = ctx->rowlength;
         return;
      case OSMESA_Y_UP:
         *value = ctx->yup;
         return;
      default:
         gl_error( ctx->gl_ctx, GL_INVALID_ENUM, "OSMesaGetIntergerv(pname)" );
         return;
   }
}



/**********************************************************************/
/*** Device Driver Functions                                        ***/
/**********************************************************************/


/*
 * Useful macros:
 */
#define PACK_RGBA(R,G,B,A)  (  ((R) << osmesa->rshift) \
                             | ((G) << osmesa->gshift) \
                             | ((B) << osmesa->bshift) \
                             | ((A) << osmesa->ashift) )

#define PACK_RGBA2(R,G,B,A)  (  ((R) << rshift) \
                              | ((G) << gshift) \
                              | ((B) << bshift) \
                              | ((A) << ashift) )

#define UNPACK_RED(P)      (((P) >> osmesa->rshift) & 0xff)
#define UNPACK_GREEN(P)    (((P) >> osmesa->gshift) & 0xff)
#define UNPACK_BLUE(P)     (((P) >> osmesa->bshift) & 0xff)
#define UNPACK_ALPHA(P)    (((P) >> osmesa->ashift) & 0xff)

#define PIXELADDR1(X,Y)  ((GLubyte *) osmesa->rowaddr[Y] + (X))
#define PIXELADDR4(X,Y)  ((GLuint *)  osmesa->rowaddr[Y] + (X))




static GLboolean set_buffer( GLcontext *ctx, GLenum mode )
{
   if (mode==GL_FRONT) {
      return GL_TRUE;
   }
   else {
      return GL_FALSE;
   }
}


static void clear_index( GLcontext *ctx, GLuint index )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   osmesa->clearpixel = index;
}



static void clear_color( GLcontext *ctx,
                         GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   osmesa->clearpixel = PACK_RGBA( r, g, b, a );
}



static void clear( GLcontext *ctx,
                   GLboolean all, GLint x, GLint y, GLint width, GLint height )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   if (osmesa->format==OSMESA_COLOR_INDEX) {
      if (all) {
         /* Clear whole CI buffer */
         MEMSET(osmesa->buffer, osmesa->clearpixel, osmesa->rowlength*osmesa->height);
      }
      else {
         /* Clear part of CI buffer */
         GLuint i, j;
         for (i=0;i<height;i++) {
            GLubyte *ptr1 = PIXELADDR1( x, (y+i) );
            for (j=0;j<width;j++) {
               *ptr1++ = osmesa->clearpixel;
            }
         }
      }
   }
   else {
      if (all) {
         /* Clear whole RGBA buffer */
         GLuint i, n, *ptr4;
         n = osmesa->rowlength * osmesa->height;
         ptr4 = (GLuint *) osmesa->buffer;
         for (i=0;i<n;i++) {
            *ptr4++ = osmesa->clearpixel;
         }
      }
      else {
         /* Clear part of RGBA buffer */
         GLuint i, j;
         for (i=0;i<height;i++) {
            GLuint *ptr4 = PIXELADDR4( x, (y+i) );
            for (j=0;j<width;j++) {
               *ptr4++ = osmesa->clearpixel;
            }
         }
      }
   }
}



static void set_index( GLcontext *ctx, GLuint index )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   osmesa->pixel = index;
}



static void set_color( GLcontext *ctx,
                       GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   osmesa->pixel = PACK_RGBA( r, g, b, a );
}



static void buffer_size( GLcontext *ctx, GLuint *width, GLuint *height )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   *width = osmesa->width;
   *height = osmesa->height;
}



static void write_color_span( GLcontext *ctx,
                              GLuint n, GLint x, GLint y,
                              const GLubyte red[], const GLubyte green[],
			      const GLubyte blue[], const GLubyte alpha[],
			      const GLubyte mask[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLuint *ptr4 = PIXELADDR4( x, y );
   GLuint i;
   GLint rshift = osmesa->rshift;
   GLint gshift = osmesa->gshift;
   GLint bshift = osmesa->bshift;
   GLint ashift = osmesa->ashift;
   if (mask) {
      for (i=0;i<n;i++,ptr4++) {
         if (mask[i]) {
            *ptr4 = PACK_RGBA2( red[i], green[i], blue[i], alpha[i] );
         }
      }
   }
   else {
      for (i=0;i<n;i++,ptr4++) {
         *ptr4 = PACK_RGBA2( red[i], green[i], blue[i], alpha[i] );
      }
   }
}



static void write_monocolor_span( GLcontext *ctx,
                                  GLuint n, GLint x, GLint y,
				  const GLubyte mask[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLuint *ptr4 = PIXELADDR4(x,y);
   GLuint i;
   for (i=0;i<n;i++,ptr4++) {
      if (mask[i]) {
         *ptr4 = osmesa->pixel;
      }
   }
}



static void write_color_pixels( GLcontext *ctx,
                                GLuint n, const GLint x[], const GLint y[],
                                const GLubyte red[], const GLubyte green[],
			        const GLubyte blue[], const GLubyte alpha[],
			        const GLubyte mask[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLuint i;
   GLint rshift = osmesa->rshift;
   GLint gshift = osmesa->gshift;
   GLint bshift = osmesa->bshift;
   GLint ashift = osmesa->ashift;
   for (i=0;i<n;i++) {
      if (mask[i]) {
         GLuint *ptr4 = PIXELADDR4(x[i],y[i]);
         *ptr4 = PACK_RGBA2( red[i], green[i], blue[i], alpha[i] );
      }
   }
}



static void write_monocolor_pixels( GLcontext *ctx,
                                    GLuint n, const GLint x[], const GLint y[],
				    const GLubyte mask[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLuint i;
   for (i=0;i<n;i++) {
      if (mask[i]) {
         GLuint *ptr4 = PIXELADDR4(x[i],y[i]);
         *ptr4 = osmesa->pixel;
      }
   }
}



static void write_index_span( GLcontext *ctx,
                              GLuint n, GLint x, GLint y, const GLuint index[],
			      const GLubyte mask[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLubyte *ptr1 = PIXELADDR1(x,y);
   GLuint i;
   for (i=0;i<n;i++,ptr1++) {
      if (mask[i]) {
         *ptr1 = (GLubyte) index[i];
      }
   }
}



static void write_monoindex_span( GLcontext *ctx,
                                  GLuint n, GLint x, GLint y,
				  const GLubyte mask[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLubyte *ptr1 = PIXELADDR1(x,y);
   GLuint i;
   for (i=0;i<n;i++,ptr1++) {
      if (mask[i]) {
         *ptr1 = (GLubyte) osmesa->pixel;
      }
   }
}



static void write_index_pixels( GLcontext *ctx,
                                GLuint n, const GLint x[], const GLint y[],
			        const GLuint index[], const GLubyte mask[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLuint i;
   for (i=0;i<n;i++) {
      if (mask[i]) {
         GLubyte *ptr1 = PIXELADDR1(x[i],y[i]);
         *ptr1 = (GLubyte) index[i];
      }
   }
}



static void write_monoindex_pixels( GLcontext *ctx,
                                    GLuint n, const GLint x[], const GLint y[],
				    const GLubyte mask[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLuint i;
   for (i=0;i<n;i++) {
      if (mask[i]) {
         GLubyte *ptr1 = PIXELADDR1(x[i],y[i]);
         *ptr1 = (GLubyte) osmesa->pixel;
      }
   }
}



static void read_index_span( GLcontext *ctx,
                             GLuint n, GLint x, GLint y, GLuint index[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLuint i;
   GLubyte *ptr1 = PIXELADDR1(x,y);
   for (i=0;i<n;i++,ptr1++) {
      index[i] = (GLuint) *ptr1;
   }
}


static void read_color_span( GLcontext *ctx,
                             GLuint n, GLint x, GLint y,
                             GLubyte red[], GLubyte green[],
			     GLubyte blue[], GLubyte alpha[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLuint i;
   GLuint *ptr4 = PIXELADDR4(x,y);
   for (i=0;i<n;i++) {
      GLuint pixel = *ptr4++;
      red[i]   = UNPACK_RED(pixel);
      green[i] = UNPACK_GREEN(pixel);
      blue[i]  = UNPACK_BLUE(pixel);
      alpha[i] = UNPACK_ALPHA(pixel);
   }
}


static void read_index_pixels( GLcontext *ctx,
                               GLuint n, const GLint x[], const GLint y[],
			       GLuint index[], const GLubyte mask[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLuint i;
   for (i=0;i<n;i++) {
      if (mask[i] ) {
         GLubyte *ptr1 = PIXELADDR1(x[i],y[i]);
         index[i] = (GLuint) *ptr1;
      }
   }
}


static void read_color_pixels( GLcontext *ctx,
                               GLuint n, const GLint x[], const GLint y[],
			       GLubyte red[], GLubyte green[],
			       GLubyte blue[], GLubyte alpha[],
                               const GLubyte mask[] )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLuint i;
   for (i=0;i<n;i++) {
      if (mask[i]) {
         GLuint *ptr4 = PIXELADDR4(x[i],y[i]);
         GLuint pixel = *ptr4;
         red[i]   = UNPACK_RED(pixel);
         green[i] = UNPACK_GREEN(pixel);
         blue[i]  = UNPACK_BLUE(pixel);
         alpha[i] = UNPACK_ALPHA(pixel);
      }
   }
}




/**********************************************************************/
/*****                 Optimized triangle rendering               *****/
/**********************************************************************/


/*
 * Smooth-shaded, z-less triangle, RGBA color.
 */
static void smooth_color_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                     GLuint v2, GLuint pv )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
   GLint rshift = osmesa->rshift;
   GLint gshift = osmesa->gshift;
   GLint bshift = osmesa->bshift;
   GLint ashift = osmesa->ashift;
#define INTERP_Z 1
#define INTERP_RGB 1
#define INTERP_ALPHA 1
#define INNER_LOOP( LEFT, RIGHT, Y )				\
{								\
   GLint i, len = RIGHT-LEFT;					\
   GLuint *img = PIXELADDR4(LEFT,Y);   				\
   for (i=0;i<len;i++,img++) {					\
      GLdepth z = FixedToDepth(ffz);				\
      if (z < zRow[i]) {					\
         *img = PACK_RGBA2( FixedToInt(ffr), FixedToInt(ffg),	\
		            FixedToInt(ffb), FixedToInt(ffa) );	\
         zRow[i] = z;						\
      }								\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;  ffa += fdadx;\
      ffz += fdzdx;						\
   }								\
}
#include "tritemp.h"
}




/*
 * Flat-shaded, z-less triangle, RGBA color.
 */
static void flat_color_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                   GLuint v2, GLuint pv )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define SETUP_CODE			\
   GLubyte r = VB->Color[pv][0];	\
   GLubyte g = VB->Color[pv][1];	\
   GLubyte b = VB->Color[pv][2];	\
   GLubyte a = VB->Color[pv][3];	\
   GLuint pixel = PACK_RGBA(r,g,b,a);

#define INNER_LOOP( LEFT, RIGHT, Y )	\
{					\
   GLint i, len = RIGHT-LEFT;		\
   GLuint *img = PIXELADDR4(LEFT,Y);   	\
   for (i=0;i<len;i++,img++) {		\
      GLdepth z = FixedToDepth(ffz);	\
      if (z < zRow[i]) {		\
         *img = pixel;			\
         zRow[i] = z;			\
      }					\
      ffz += fdzdx;			\
   }					\
}
#include "tritemp.h"
}



/*
 * Return pointer to an accelerated triangle function if possible.
 */
static triangle_func choose_triangle_function( GLcontext *ctx )
{
   OSMesaContext osmesa = (OSMesaContext) ctx->DriverCtx;

   if (ctx->Polygon.SmoothFlag)     return NULL;
   if (ctx->Polygon.StippleFlag)    return NULL;
   if (ctx->Texture.Enabled)        return NULL;

   if (ctx->RasterMask==DEPTH_BIT
       && ctx->Depth.Func==GL_LESS
       && ctx->Depth.Mask==GL_TRUE
       && osmesa->format!=OSMESA_COLOR_INDEX) {
      if (ctx->Light.ShadeModel==GL_SMOOTH) {
         return smooth_color_z_triangle;
      }
      else {
         return flat_color_z_triangle;
      }
   }
   return NULL;
}



static void osmesa_setup_DD_pointers( GLcontext *ctx )
{
   ctx->Driver.UpdateState = osmesa_setup_DD_pointers;

   ctx->Driver.SetBuffer = set_buffer;
   ctx->Driver.Color = set_color;
   ctx->Driver.Index = set_index;
   ctx->Driver.ClearIndex = clear_index;
   ctx->Driver.ClearColor = clear_color;
   ctx->Driver.Clear = clear;

   ctx->Driver.GetBufferSize = buffer_size;

   ctx->Driver.PointsFunc = NULL;
   ctx->Driver.LineFunc = NULL;
   ctx->Driver.TriangleFunc = choose_triangle_function( ctx );

   ctx->Driver.WriteColorSpan = write_color_span;
   ctx->Driver.WriteColorPixels = write_color_pixels;
   ctx->Driver.WriteIndexSpan = write_index_span;
   ctx->Driver.WriteMonocolorSpan = write_monocolor_span;
   ctx->Driver.WriteMonoindexSpan = write_monoindex_span;
   ctx->Driver.WriteIndexPixels = write_index_pixels;
   ctx->Driver.WriteMonocolorPixels = write_monocolor_pixels;
   ctx->Driver.WriteMonoindexPixels = write_monoindex_pixels;

   ctx->Driver.ReadColorSpan = read_color_span;
   ctx->Driver.ReadIndexSpan = read_index_span;
   ctx->Driver.ReadColorPixels = read_color_pixels;
   ctx->Driver.ReadIndexPixels = read_index_pixels;
}
