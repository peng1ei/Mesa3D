/* $Id: dd.h,v 1.1 1996/09/13 01:38:16 brianp Exp $ */

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
 * $Log: dd.h,v $
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


#ifndef DD_INCLUDED
#define DD_INCLUDED


/* THIS FILE ONLY INCLUDED BY types.h !!!!! */


/*
 *                      Device Driver (DD) interface
 *
 *
 * All device driver functions are accessed via pointers in the global
 * DD struct.  The reason we use function pointers is to:
 *   1. allow switching between a number of different device drivers at
 *      runtime.
 *   2. use optimized functions dependant on frame buffer configuration
 *
 * The function pointers in the DD struct are divided into two groups:
 * mandatory and optional.  Mandatory functions have to be implemented by
 * every device driver.  Optional functions may or may not be implemented
 * by the device driver.  Optional functions provide ways to take advantage
 * of special hardware or optimized algorithms.
 *
 * When should the device driver set the DD pointers?
 *   1. When a "MakeCurrent" function is called such as GLXMakeCurrent(),
 *      XMesaMakeCurrent(), WMesaMakeCurrent(), etc.  In this case, _all_
 *      the mandatory DD pointers must be updated.  But do this after
 *      the call to gl_set_context().
 *   2. Whenever the Driver.update_state() function is called.  In this case
 *      only some DD pointers may need to be updated.
 *
 * In either case, the device driver should re-examine the GL context state
 * and update the DD pointers as necessary.
 *
 *
 * The first argument to every device driver function is a GLcontext *.
 * This is needed so the callee knows which window/context to work with.
 *
 *
 *
 * Here's a quick description of most device driver function's purpose:
 *
 * Mandatory functions:
 * --------------------
 * UpdateState - called by Mesa whenever it thinks the device driver
 *                should update its DD pointers.
 * ClearIndex - implements glClearIndex()
 * ClearColor - implements glClearColor()
 * ClearColorBuffer - implements glClear(), with some special arguments
 * Index - implements glIndex()
 * Color - implements glColor()
 *
 * SetBuffer - selects the front or back buffer for reading and writing.
 *             The default value is the buffer selected for writing pixels.
 *             When pixels have to be read from the color buffer, the core
 *             library will  call this function to temporarily select the
 *             "read" buffer, then restore it to the "draw" buffer.
 *
 * GetBufferSize - return width and height of image buffer
 *
 * WriteColorSpan - write a horizontal run of RGBA pixels
 * WriteMonocolorSpan - write a horizontal run of mono-RGBA pixels
 * WriteColorPixels - write a random array of RGBA pixels
 * WriteMonocolorPixels - write a random array of mono-RGBA pixels
 *
 * WriteIndexSpan - write a horizontal run of CI pixels
 * WriteMonoindexSpan - write a horizontal run of mono-CI pixels
 * WriteIndexPixels - write a random array of CI pixels
 * WriteMonoindexPixels - write a random array of mono-CI pixels
 *
 * ReadIndexSpan - read a horizontal run of color index pixels
 * ReadColorSpan - read a horizontal run of RGBA pixels
 * ReadIndexPixels - read a random array of CI pixels
 * ReadColorPixels - read a random array of RGBA pixels
 *
 *
 * Optional functions:
 * -------------------
 * Finish - implements glFinish()
 * Flush - implements glFlush()
 *
 * IndexMask - implements glIndexMask() if possible, else return GL_FALSE
 * ColorMask - implements glColorMask() if possible, else return GL_FALSE
 * LogicOp - implements glLogicOp() if possible, else return GL_FALSE
 * Dither - enable/disable dithering
 *
 *
 * Depth (Z) buffer functions may be implemented by drivers for systems
 * with hardware Z buffers.  The functions are:
 *
 * AllocDepthBuffer - called when the depth buffer must be allocated or
 *     possibly resized.
 * ClearDepthBuffer - clear the depth buffer to depth specified by
 *     CC.Depth.Clear value.
 * DepthTestSpan/Pixels - apply the depth buffer test to an span/array of
 *     pixels and return an updated pixel mask.  This function is not used
 *     when accelerated point, line, polygon functions are used.
 * ReadDepthSpanFloat - return depth values in [0,1] for glReadPixels
 * ReadDepthSpanInt - return depth values as integers for glReadPixels
 *
 *
 * Accelerated point, line, polygon drawing:
 *
 * PointsFunc - points to accelerated points drawing function, or NULL
 * LineFunc - points to accelerated line drawing function, or NULL
 * PolygonFunc - points to accelrated polygon drawing function, or NULL
 *
 *
 * Miscellaneous
 *
 * DrawPixels - implements glDrawPixels, returns GL_TRUE iff successful.
 *     the job.
 * Bitmap - implements glBitmap, returns GL_TRUE iff successful.
 * Begin, End - called by glBegin/glEnd so the device driver can do whatever
 *     it may need to do (window system synchronization, for example)
 *
 *
 * Notes:
 * ------
 *   RGBA = red/green/blue/alpha
 *   CI = color index (color mapped mode)
 *   mono = all pixels have the same color or index
 *
 *   The write_ functions all take an array of mask flags which indicate
 *   whether or not the pixel should be written.  One special case exists
 *   in the write_color_span function: if the mask array is NULL, then
 *   draw all pixels.  This is an optimization used for glDrawPixels().
 *
 * IN ALL CASES:
 *      X coordinates start at 0 at the left and increase to the right
 *      Y coordinates start at 0 at the bottom and increase upward
 *
 */





/*
 * Device Driver function table.  See dd.h for more information
 */
struct dd_function_table {

   /***
    *** Mandatory functions:  these functions must be implemented by
    *** every device driver.
    ***/

   void (*UpdateState)( GLcontext *ctx );

   void (*ClearIndex)( GLcontext *ctx, GLuint index );

   void (*ClearColor)( GLcontext *ctx, GLubyte red, GLubyte green,
                                        GLubyte blue, GLubyte alpha );

   void (*Clear)( GLcontext *ctx,
                  GLboolean all, GLint x, GLint y, GLint width, GLint height );

   void (*Index)( GLcontext *ctx, GLuint index );

   void (*Color)( GLcontext *ctx,
                  GLubyte red, GLubyte green, GLubyte glue, GLubyte alpha );

   GLboolean (*SetBuffer)( GLcontext *ctx, GLenum mode );

   void (*GetBufferSize)( GLcontext *ctx,
                          GLuint *width, GLuint *height );

   /*
    * Functions for writing pixels to the frame buffer:
    */
   void (*WriteColorSpan)( GLcontext *ctx,
                           GLuint n, GLint x, GLint y,
			   const GLubyte red[], const GLubyte green[],
			   const GLubyte blue[], const GLubyte alpha[],
			   const GLubyte mask[] );

   void (*WriteMonocolorSpan)( GLcontext *ctx,
                               GLuint n, GLint x, GLint y,
			       const GLubyte mask[] );

   void (*WriteColorPixels)( GLcontext *ctx,
                             GLuint n, const GLint x[], const GLint y[],
			     const GLubyte red[], const GLubyte green[],
			     const GLubyte blue[], const GLubyte alpha[],
			     const GLubyte mask[] );

   void (*WriteMonocolorPixels)( GLcontext *ctx,
                                 GLuint n, const GLint x[], const GLint y[],
				 const GLubyte mask[] );

   void (*WriteIndexSpan)( GLcontext *ctx,
                           GLuint n, GLint x, GLint y, const GLuint index[],
                           const GLubyte mask[] );

   void (*WriteMonoindexSpan)( GLcontext *ctx,
                               GLuint n, GLint x, GLint y,
			       const GLubyte mask[] );

   void (*WriteIndexPixels)( GLcontext *ctx,
                             GLuint n, const GLint x[], const GLint y[],
                             const GLuint index[], const GLubyte mask[] );

   void (*WriteMonoindexPixels)( GLcontext *ctx,
                                 GLuint n, const GLint x[], const GLint y[],
				 const GLubyte mask[] );

   /*
    * Functions to read pixels from frame buffer:
    */
   void (*ReadIndexSpan)( GLcontext *ctx,
                          GLuint n, GLint x, GLint y, GLuint index[] );

   void (*ReadColorSpan)( GLcontext *ctx,
                          GLuint n, GLint x, GLint y,
			  GLubyte red[], GLubyte green[],
			  GLubyte blue[], GLubyte alpha[] );

   void (*ReadIndexPixels)( GLcontext *ctx,
                            GLuint n, const GLint x[], const GLint y[],
			    GLuint indx[], const GLubyte mask[] );

   void (*ReadColorPixels)( GLcontext *ctx,
                            GLuint n, const GLint x[], const GLint y[],
			    GLubyte red[], GLubyte green[],
			    GLubyte blue[], GLubyte alpha[],
                            const GLubyte mask[] );



   /***
    *** Optional functions:  these functions may or may not be implemented
    *** by the device driver.  If the device driver doesn't implement them
    *** it should never touch these pointers since Mesa will either set them
    *** to NULL or point them at a fall-back function.
    ***/

   void (*Finish)( GLcontext *ctx );

   void (*Flush)( GLcontext *ctx );

   GLboolean (*IndexMask)( GLcontext *ctx, GLuint mask );

   GLboolean (*ColorMask)( GLcontext *ctx,
                           GLboolean rmask, GLboolean gmask,
                           GLboolean bmask, GLboolean amask );

   GLboolean (*LogicOp)( GLcontext *ctx, GLenum op );

   void (*Dither)( GLcontext *ctx, GLboolean enable );

   /*
    * For supporting hardware Z buffers:
    */
   void (*AllocDepthBuffer)( GLcontext *ctx );
   void (*ClearDepthBuffer)( GLcontext *ctx );

   GLuint (*DepthTestSpan)( GLcontext *ctx,
                            GLuint n, GLint x, GLint y, const GLdepth z[],
                            GLubyte mask[] );

   void (*DepthTestPixels)( GLcontext *ctx,
                            GLuint n, const GLint x[], const GLint y[],
                            const GLdepth z[], GLubyte mask[] );

   void (*ReadDepthSpanFloat)( GLcontext *ctx,
                               GLuint n, GLint x, GLint y, GLfloat depth[]);

   void (*ReadDepthSpanInt)( GLcontext *ctx,
                             GLuint n, GLint x, GLint y, GLdepth depth[] );

   /*
    * Accelerated point, line, polygon, glDrawPixels and glBitmap functions:
    */
   points_func PointsFunc;
   line_func LineFunc;
   triangle_func TriangleFunc;

   GLboolean (*DrawPixels)( GLcontext *ctx,
                            GLint x, GLint y, GLsizei width, GLsizei height,
                            GLenum format, GLenum type, GLboolean packed,
                            const GLvoid *pixels );

   GLboolean (*Bitmap)( GLcontext *ctx, GLsizei width, GLsizei height,
                        GLfloat xorig, GLfloat yorig,
                        GLfloat xmove, GLfloat ymove,
                        const struct gl_image *bitmap );

   void (*Begin)( GLcontext *ctx, GLenum mode );
   void (*End)( GLcontext *ctx );
};



extern void gl_init_dd_function_table( GLcontext *ctx,
                                       struct dd_function_table *Driver );


#endif

