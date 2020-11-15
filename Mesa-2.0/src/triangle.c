/* $Id: triangle.c,v 1.5 1996/10/01 03:31:17 brianp Exp $ */

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
 * $Log: triangle.c,v $
 * Revision 1.5  1996/10/01 03:31:17  brianp
 * use new FixedToDepth() macro
 *
 * Revision 1.4  1996/09/27 01:30:37  brianp
 * removed unneeded INTERP_ALPHA from flat_rgba_triangle()
 *
 * Revision 1.3  1996/09/15 14:19:16  brianp
 * now use GLframebuffer and GLvisual
 *
 * Revision 1.2  1996/09/15 01:48:58  brianp
 * removed #define NULL 0
 *
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


/*
 * Triangle rasterizers
 */


#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "depth.h"
#include "feedback.h"
#include "macros.h"
#include "span.h"
#include "triangle.h"
#include "types.h"
#include "vb.h"



/*
 * Put triangle in feedback buffer.
 */
static void feedback_triangle( GLcontext *ctx,
                               GLuint v0, GLuint v1, GLuint v2, GLuint pv )
{
   struct vertex_buffer *VB = ctx->VB;
   GLfloat x, y, z, w;
   GLfloat tc[4];
   GLfloat color[4];
   GLuint i, v;

   FEEDBACK_TOKEN( ctx, (GLfloat) GL_POLYGON_TOKEN );
   FEEDBACK_TOKEN( ctx, (GLfloat) 3 );        /* three vertices */

   for (i=0;i<3;i++) {
      if (i==0)       v = v0;
      else if (i==1)  v = v1;
      else            v = v2;

      x = VB->Win[v][0];
      y = VB->Win[v][1];
      z = VB->Win[v][2] / DEPTH_SCALE;
      w = VB->Clip[v][3];

      /* convert color from integer back to a float in [0,1] */
      color[0] = (GLfloat) VB->Color[v][0] * ctx->Visual->InvRedScale;
      color[1] = (GLfloat) VB->Color[v][1] * ctx->Visual->InvGreenScale;
      color[2] = (GLfloat) VB->Color[v][2] * ctx->Visual->InvBlueScale;
      color[3] = (GLfloat) VB->Color[v][3] * ctx->Visual->InvAlphaScale;

      tc[0] = VB->TexCoord[v][0];
      tc[1] = VB->TexCoord[v][1];
      tc[2] = 0.0F;          /* TODO: R, Q components */
      tc[3] = 1.0F;

      gl_feedback_vertex( ctx, x, y, z, w, color, (GLfloat) VB->Index[v], tc );
   }
}



/*
 * Put triangle in selection buffer.
 */
static void select_triangle( GLcontext *ctx,
                             GLuint v0, GLuint v1, GLuint v2, GLuint pv )
{
   struct vertex_buffer *VB = ctx->VB;

   gl_update_hitflag( ctx, VB->Win[v0][2] / DEPTH_SCALE );
   gl_update_hitflag( ctx, VB->Win[v1][2] / DEPTH_SCALE );
   gl_update_hitflag( ctx, VB->Win[v2][2] / DEPTH_SCALE );
}



/*
 * Render a flat-shaded color index triangle.
 */
static void flat_ci_triangle( GLcontext *ctx,
                              GLuint v0, GLuint v1, GLuint v2, GLuint pv )
{
#define INTERP_Z 1

#define SETUP_CODE				\
   GLuint index = VB->Index[pv];		\
   if (!VB->MonoColor) {			\
      /* set the color index */			\
      (*ctx->Driver.Index)( ctx, index );	\
   }

#define INNER_LOOP( LEFT, RIGHT, Y )				\
	{							\
	   GLint i, n = RIGHT-LEFT;				\
	   GLdepth zspan[MAX_WIDTH];				\
	   if (n>0) {						\
	      for (i=0;i<n;i++) {				\
		 zspan[i] = FixedToDepth(ffz);			\
		 ffz += fdzdx;					\
	      }							\
	      gl_write_monoindex_span( ctx, n, LEFT, Y,		\
	                            zspan, index, GL_POLYGON );	\
	   }							\
	}

#include "tritemp.h"	      
}



/*
 * Render a smooth-shaded color index triangle.
 */
static void smooth_ci_triangle( GLcontext *ctx,
                                GLuint v0, GLuint v1, GLuint v2, GLuint pv )
{
#define INTERP_Z 1
#define INTERP_INDEX 1

#define INNER_LOOP( LEFT, RIGHT, Y )				\
	{							\
	   GLint i, n = RIGHT-LEFT;				\
	   GLdepth zspan[MAX_WIDTH];				\
           GLuint index[MAX_WIDTH];				\
	   if (n>0) {						\
	      for (i=0;i<n;i++) {				\
		 zspan[i] = FixedToDepth(ffz);			\
                 index[i] = FixedToInt(ffi);			\
		 ffz += fdzdx;					\
		 ffi += fdidx;					\
	      }							\
	      gl_write_index_span( ctx, n, LEFT, Y, zspan,	\
	                           index, GL_POLYGON );		\
	   }							\
	}

#include "tritemp.h"
}



/*
 * Render a flat-shaded RGBA triangle.
 */
static void flat_rgba_triangle( GLcontext *ctx,
                                GLuint v0, GLuint v1, GLuint v2, GLuint pv )
{
#define INTERP_Z 1

#define SETUP_CODE				\
   if (!VB->MonoColor) {			\
      /* set the color */			\
      GLubyte r = VB->Color[pv][0];		\
      GLubyte g = VB->Color[pv][1];		\
      GLubyte b = VB->Color[pv][2];		\
      GLubyte a = VB->Color[pv][3];		\
      (*ctx->Driver.Color)( ctx, r, g, b, a );	\
   }

#define INNER_LOOP( LEFT, RIGHT, Y )				\
	{							\
	   GLint i, n = RIGHT-LEFT;				\
	   GLdepth zspan[MAX_WIDTH];				\
	   if (n>0) {						\
	      for (i=0;i<n;i++) {				\
		 zspan[i] = FixedToDepth(ffz);			\
		 ffz += fdzdx;					\
	      }							\
              gl_write_monocolor_span( ctx, n, LEFT, Y, zspan,	\
                             VB->Color[pv][0], VB->Color[pv][1],\
                             VB->Color[pv][2], VB->Color[pv][3],\
			     GL_POLYGON );			\
	   }							\
	}

#include "tritemp.h"
}



/*
 * Render a smooth-shaded RGBA triangle.
 */
static void smooth_rgba_triangle( GLcontext *ctx,
                                  GLuint v0, GLuint v1, GLuint v2, GLuint pv )
{
#define INTERP_Z 1
#define INTERP_RGB 1
#define INTERP_ALPHA 1

#define INNER_LOOP( LEFT, RIGHT, Y )				\
	{							\
	   GLint i, n = RIGHT-LEFT;				\
	   GLdepth zspan[MAX_WIDTH];				\
	   GLubyte red[MAX_WIDTH], green[MAX_WIDTH];		\
	   GLubyte blue[MAX_WIDTH], alpha[MAX_WIDTH];		\
	   if (n>0) {						\
	      for (i=0;i<n;i++) {				\
		 zspan[i] = FixedToDepth(ffz);			\
		 red[i]   = FixedToInt(ffr);			\
		 green[i] = FixedToInt(ffg);			\
		 blue[i]  = FixedToInt(ffb);			\
		 alpha[i] = FixedToInt(ffa);			\
		 ffz += fdzdx;					\
		 ffr += fdrdx;					\
		 ffg += fdgdx;					\
		 ffb += fdbdx;					\
		 ffa += fdadx;					\
	      }							\
	      gl_write_color_span( ctx, n, LEFT, Y, zspan,	\
	                           red, green, blue, alpha,	\
				   GL_POLYGON );		\
	   }							\
	}

#include "tritemp.h"
}



/*
 * Render an RGB, GL_DECAL, textured triangle.
 * Interpolate S,T only w/out mipmapping or perspective correction.
 */
static void simple_textured_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                      GLuint v2, GLuint pv )
{
#define INTERP_ST 1
#define S_SCALE twidth
#define T_SCALE theight
#define SETUP_CODE							\
   GLfloat twidth = (GLfloat) ctx->Texture.Current2D->Image[0]->Width;	\
   GLfloat theight = (GLfloat) ctx->Texture.Current2D->Image[0]->Height;\
   GLint twidth_log2 = ctx->Texture.Current2D->Image[0]->WidthLog2;	\
   GLubyte *texture = ctx->Texture.Current2D->Image[0]->Data;		\
   GLint smask = ctx->Texture.Current2D->Image[0]->Width - 1;		\
   GLint tmask = ctx->Texture.Current2D->Image[0]->Height - 1;

#define INNER_LOOP( LEFT, RIGHT, Y )				\
	{							\
	   GLint i, n = RIGHT-LEFT;				\
	   GLubyte red[MAX_WIDTH], green[MAX_WIDTH];		\
	   GLubyte blue[MAX_WIDTH], alpha[MAX_WIDTH];		\
	   if (n>0) {						\
	      for (i=0;i<n;i++) {				\
                 GLint s = FixedToInt(ffs) & smask;		\
                 GLint t = FixedToInt(fft) & tmask;		\
                 GLint pos = (t << twidth_log2) + s;		\
                 pos = pos + pos  + pos;  /* multiply by 3 */	\
                 red[i]   = texture[pos];			\
                 green[i] = texture[pos+1];			\
                 blue[i]  = texture[pos+2];			\
                 alpha[i] = 255;				\
		 ffs += fdsdx;					\
		 fft += fdtdx;					\
	      }							\
              (*ctx->Driver.WriteColorSpan)( ctx, n, LEFT, Y,	\
                             red, green, blue, alpha, NULL );	\
	   }							\
	}

#include "tritemp.h"
}



/*
 * Render an RGB, GL_DECAL, textured triangle.
 * Interpolate S,T, GL_LESS depth test, w/out mipmapping or
 * perspective correction.
 */
static void simple_z_textured_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                      GLuint v2, GLuint pv )
{
#define INTERP_Z 1
#define INTERP_ST 1
#define S_SCALE twidth
#define T_SCALE theight
#define SETUP_CODE							\
   GLfloat twidth = (GLfloat) ctx->Texture.Current2D->Image[0]->Width;	\
   GLfloat theight = (GLfloat) ctx->Texture.Current2D->Image[0]->Height;\
   GLint twidth_log2 = ctx->Texture.Current2D->Image[0]->WidthLog2;	\
   GLubyte *texture = ctx->Texture.Current2D->Image[0]->Data;		\
   GLint smask = ctx->Texture.Current2D->Image[0]->Width - 1;		\
   GLint tmask = ctx->Texture.Current2D->Image[0]->Height - 1;

#define INNER_LOOP( LEFT, RIGHT, Y )				\
	{							\
	   GLint i, n = RIGHT-LEFT;				\
	   GLubyte red[MAX_WIDTH], green[MAX_WIDTH];		\
	   GLubyte blue[MAX_WIDTH], alpha[MAX_WIDTH];		\
           GLubyte mask[MAX_WIDTH];				\
	   if (n>0) {						\
	      for (i=0;i<n;i++) {				\
                 GLdepth z = FixedToDepth(ffz);			\
                 if (z < zRow[i]) {				\
                    GLint s = FixedToInt(ffs) & smask;		\
                    GLint t = FixedToInt(fft) & tmask;		\
                    GLint pos = (t << twidth_log2) + s;		\
                    pos = pos + pos  + pos;  /* multiply by 3 */\
                    red[i]   = texture[pos];			\
                    green[i] = texture[pos+1];			\
                    blue[i]  = texture[pos+2];			\
                    alpha[i] = 255;				\
		    zRow[i] = z;				\
                    mask[i] = 1;				\
                 }						\
                 else {						\
                    mask[i] = 0;				\
                 }						\
		 ffz += fdzdx;					\
		 ffs += fdsdx;					\
		 fft += fdtdx;					\
	      }							\
              (*ctx->Driver.WriteColorSpan)( ctx, n, LEFT, Y,	\
                             red, green, blue, alpha, mask );	\
	   }							\
	}

#include "tritemp.h"
}



/*
 * Render a smooth-shaded, textured, RGBA triangle.
 * Interpolate S,T with perspective correction, w/out mipmapping.
 */
static void general_textured_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                       GLuint v2, GLuint pv )
{
#define INTERP_Z 1
#define INTERP_RGB 1
#define INTERP_ALPHA 1
#define INTERP_STW 1
#define S_SCALE 100.0F
#define T_SCALE 100.0F
#define SETUP_CODE						\
   GLboolean flat_shade = (ctx->Light.ShadeModel==GL_FLAT);	\
   GLint r, g, b, a;						\
   if (flat_shade) {						\
      r = VB->Color[pv][0];					\
      g = VB->Color[pv][1];					\
      b = VB->Color[pv][2];					\
      a = VB->Color[pv][3];					\
   }
#define INNER_LOOP( LEFT, RIGHT, Y )				\
	{							\
	   GLint i, n = RIGHT-LEFT;				\
	   GLdepth zspan[MAX_WIDTH];				\
	   GLubyte red[MAX_WIDTH], green[MAX_WIDTH];		\
	   GLubyte blue[MAX_WIDTH], alpha[MAX_WIDTH];		\
           GLfloat s[MAX_WIDTH], t[MAX_WIDTH];			\
	   if (n>0) {						\
              if (flat_shade) {					\
                 for (i=0;i<n;i++) {				\
		    zspan[i] = FixedToDepth(ffz);		\
		    red[i]   = r;				\
		    green[i] = g;				\
		    blue[i]  = b;				\
		    alpha[i] = a;				\
		    s[i] = ss/ww;				\
		    t[i] = tt/ww;				\
		    ffz += fdzdx;				\
		    ss += dsdx;					\
		    tt += dtdx;					\
		    ww += dwdx;					\
		 }						\
              }							\
              else {						\
                 for (i=0;i<n;i++) {				\
		    zspan[i] = FixedToDepth(ffz);		\
		    red[i]   = FixedToInt(ffr);			\
		    green[i] = FixedToInt(ffg);			\
		    blue[i]  = FixedToInt(ffb);			\
		    alpha[i] = FixedToInt(ffa);			\
		    s[i] = ss/ww;				\
		    t[i] = tt/ww;				\
		    ffz += fdzdx;				\
		    ffr += fdrdx;				\
		    ffg += fdgdx;				\
		    ffb += fdbdx;				\
		    ffa += fdadx;				\
		    ss += dsdx;					\
		    tt += dtdx;					\
		    ww += dwdx;					\
		 }						\
              }							\
	      gl_write_texture_span( ctx, n, LEFT, Y, zspan,	\
                                     s, t, NULL, 		\
	                             red, green, blue, alpha,	\
				     GL_POLYGON );		\
	   }							\
	}

#include "tritemp.h"
}



/*
 * Compute the lambda (texture level value) for a fragment.
 */
static GLfloat compute_lambda( GLfloat s, GLfloat t,
                               GLfloat dsdx, GLfloat dsdy,
                               GLfloat dtdx, GLfloat dtdy,
                               GLfloat w,
                               GLfloat width, GLfloat height )
{
   /* TODO: this function can probably be optimized a bit */
   GLfloat invw = 1.0F / w;
   GLfloat s0, t0, s1, t1, s2, t2;
   GLfloat dudx, dudy, dvdx, dvdy;
   GLfloat r1, r2, rho;

   s0 = s * invw;
   t0 = t * invw;
   s1 = (s+dsdx) * invw;
   t1 = (t+dtdx) * invw;
   s2 = (s+dsdy) * invw;
   t2 = (t+dtdy) * invw;

   dudx = (s1-s0) * width;
   dudy = (s2-s0) * width;
   dvdx = (t1-t0) * height;
   dvdy = (t2-t0) * height;

   /* r1 = sqrt( dudx * dudx + dvdx * dvdx ); */
   /* r2 = sqrt( dudy * dudy + dvdy * dvdy ); */
   if (dudx<0.0F)  dudx = -dudx;
   if (dudy<0.0F)  dudy = -dudy;
   if (dvdx<0.0F)  dvdx = -dvdx;
   if (dvdy<0.0F)  dvdy = -dvdy;
   r1 = MAX2( dudx, dudy );
   r2 = MAX2( dvdx, dvdy );
   rho = MAX2(r1,r2);

   if (rho<=0.0F) {
      return 0.0F;
   }
   else {
      /* return log base 2 of rho */
      return log(rho) * 1.442695;       /* 1.442695 = 1/log(2) */
   }
}



/*
 * Render a smooth-shaded, textured, RGBA triangle.
 * Interpolate S,T with perspective correction and mipmapping.
 */
static void mipmap_textured_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                      GLuint v2, GLuint pv )
{
#define INTERP_Z 1
#define INTERP_RGB 1
#define INTERP_ALPHA 1
#define INTERP_STW 1

#define SETUP_CODE							\
   GLfloat twidth = (GLfloat) ctx->Texture.Current2D->Image[0]->Width;	\
   GLfloat theight = (GLfloat) ctx->Texture.Current2D->Image[0]->Height;\
   GLboolean flat_shade = (ctx->Light.ShadeModel==GL_FLAT);		\
   GLint r, g, b, a;							\
   if (flat_shade) {							\
      r = VB->Color[pv][0];						\
      g = VB->Color[pv][1];						\
      b = VB->Color[pv][2];						\
      a = VB->Color[pv][3];						\
   }

#define INNER_LOOP( LEFT, RIGHT, Y )				\
	{							\
	   GLint i, n = RIGHT-LEFT;				\
	   GLdepth zspan[MAX_WIDTH];				\
	   GLubyte red[MAX_WIDTH], green[MAX_WIDTH];		\
	   GLubyte blue[MAX_WIDTH], alpha[MAX_WIDTH];		\
           GLfloat s[MAX_WIDTH], t[MAX_WIDTH];			\
           GLfloat lambda[MAX_WIDTH];				\
	   if (n>0) {						\
	      if (flat_shade) {					\
		 for (i=0;i<n;i++) {				\
		    zspan[i] = FixedToDepth(ffz);		\
		    red[i]   = r;				\
		    green[i] = g;				\
		    blue[i]  = b;				\
		    alpha[i] = a;				\
		    s[i] = ss/ww;				\
		    t[i] = tt/ww;				\
		    lambda[i] = compute_lambda( s[i], t[i],	\
						dsdx, dsdy,	\
						dtdx, dtdy, ww,	\
						twidth, theight );	\
		    ffz += fdzdx;				\
		    ss += dsdx;					\
		    tt += dtdx;					\
		    ww += dwdx;					\
		 }						\
              }							\
              else {						\
		 for (i=0;i<n;i++) {				\
		    zspan[i] = FixedToDepth(ffz);		\
		    red[i]   = FixedToInt(ffr);			\
		    green[i] = FixedToInt(ffg);			\
		    blue[i]  = FixedToInt(ffb);			\
		    alpha[i] = FixedToInt(ffa);			\
		    s[i] = ss/ww;				\
		    t[i] = tt/ww;				\
		    lambda[i] = compute_lambda( s[i], t[i],	\
						dsdx, dsdy,	\
						dtdx, dtdy, ww,	\
						twidth, theight );	\
		    ffz += fdzdx;				\
		    ffr += fdrdx;				\
		    ffg += fdgdx;				\
		    ffb += fdbdx;				\
		    ffa += fdadx;				\
		    ss += dsdx;					\
		    tt += dtdx;					\
		    ww += dwdx;					\
		 }						\
              }							\
	      gl_write_texture_span( ctx, n, LEFT, Y, zspan,	\
                                     s, t, lambda, 		\
	                             red, green, blue, alpha,	\
				     GL_POLYGON );		\
	   }							\
	}

#include "tritemp.h"
}



/*
 * Determine which triangle rendering function to use given the current
 * rendering context.
 */
void gl_set_triangle_function( GLcontext *ctx )
{
   GLboolean rgbmode = ctx->Visual->RGBAflag;

   if (ctx->RenderMode==GL_RENDER) {
      if (ctx->Driver.TriangleFunc) {
         /* Device driver will draw triangles. */
         ctx->TriangleFunc = ctx->Driver.TriangleFunc;
      }
      else if (ctx->Texture.Enabled) {
         if (   (ctx->Texture.Enabled & 2)
             && ctx->Texture.Current2D->Complete
             && ctx->Texture.Current2D->MinFilter==GL_NEAREST
             && ctx->Texture.Current2D->MagFilter==GL_NEAREST
             && ctx->Texture.Current2D->WrapS==GL_REPEAT
             && ctx->Texture.Current2D->WrapT==GL_REPEAT
             && ctx->Texture.Current2D->Image[0]->Format==GL_RGB
             && (ctx->Texture.EnvMode==GL_DECAL
                 || ctx->Texture.EnvMode==GL_REPLACE)
             && ctx->Hint.PerspectiveCorrection==GL_FASTEST
             && (ctx->RasterMask==DEPTH_BIT || ctx->RasterMask==0)
             && ctx->Depth.Func==GL_LESS
             && ctx->Depth.Mask==GL_TRUE
             && ctx->Polygon.StippleFlag==GL_FALSE
             && ctx->Visual->EightBitColor) {
            if (ctx->RasterMask==DEPTH_BIT) {
               ctx->TriangleFunc = simple_z_textured_triangle;
            }
            else {
               ctx->TriangleFunc = simple_textured_triangle;
            }
         }
         else {
            GLboolean mipmap = GL_TRUE;
            if (ctx->Texture.Enabled & 2) {
               if (   ctx->Texture.Current2D->MinFilter==GL_NEAREST
                      || ctx->Texture.Current2D->MinFilter==GL_LINEAR) {
                  mipmap = GL_FALSE;
               }
            }
            else {
               if (   ctx->Texture.Current1D->MinFilter==GL_NEAREST 
                      || ctx->Texture.Current1D->MinFilter==GL_LINEAR) {
                  mipmap = GL_FALSE;
               }
            }
            ctx->TriangleFunc = mipmap ? mipmap_textured_triangle
                                       : general_textured_triangle;
         }
      }
      else {
	 if (ctx->Light.ShadeModel==GL_SMOOTH) {
	    /* smooth shaded, no texturing, stippled or some raster ops */
	    ctx->TriangleFunc = rgbmode ? smooth_rgba_triangle
                                        : smooth_ci_triangle;
	 }
	 else {
	    /* flat shaded, no texturing, stippled or some raster ops */
	    ctx->TriangleFunc = rgbmode ? flat_rgba_triangle
                                        : flat_ci_triangle;
	 }
      }
   }
   else if (ctx->RenderMode==GL_FEEDBACK) {
      ctx->TriangleFunc = feedback_triangle;
   }
   else {
      /* GL_SELECT mode */
      ctx->TriangleFunc = select_triangle;
   }
}

