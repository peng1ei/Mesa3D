/* $Id: matrix.c,v 1.4 1996/09/27 01:29:05 brianp Exp $ */

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
 * $Log: matrix.c,v $
 * Revision 1.4  1996/09/27 01:29:05  brianp
 * added missing default cases to switches
 *
 * Revision 1.3  1996/09/15 14:18:37  brianp
 * now use GLframebuffer and GLvisual
 *
 * Revision 1.2  1996/09/14 06:46:04  brianp
 * better matmul() from Jacques Leroy
 *
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


/*
 * Matrix operations
 *
 *
 * NOTES:
 * 1. 4x4 transformation matrices are stored in memory in column major order.
 * 2. Points/vertices are to be thought of as column vectors.
 * 3. Transformation of a point p by a matrix M is: p' = M * p
 *
 */



#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "accum.h"
#include "alphabuf.h"
#include "context.h"
#include "depth.h"
#include "dlist.h"
#include "macros.h"
#include "matrix.h"
#include "stencil.h"
#include "types.h"



static GLfloat Identity[16] = {
   1.0, 0.0, 0.0, 0.0,
   0.0, 1.0, 0.0, 0.0,
   0.0, 0.0, 1.0, 0.0,
   0.0, 0.0, 0.0, 1.0
};




#ifdef DEBUG
static void print_matrix( const GLfloat m[16] )
{
   int i;

   for (i=0;i<4;i++) {
      printf("%f %f %f %f\n", m[i], m[4+i], m[8+i], m[12+i] );
   }
}
#endif



/*
 * Perform a 4x4 matrix multiplication  (product = a x b).
 * Input:  a, b - matrices to multiply
 * Output:  product - product of a and b
 * WARNING: (product != b) assumed
 * NOTE:    (product == a) allowed    
 */
static void matmul( GLfloat *product, const GLfloat *a, const GLfloat *b )
{
   /* This matmul was contributed by Thomas Malik */
   GLint i;

#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  product[(col<<2)+row]

   /* i-te Zeile */
   for (i = 0; i < 4; i++) {
      GLfloat ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
      P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
      P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
      P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
      P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
   }

#undef A
#undef B
#undef P
}



/*
 * Find the inverse of the 4 by 4 matrix b using gausian elimination
 * and return it in a.
 *
 * This function was contributed by Thomas Malik (malik@rhrk.uni-kl.de).
 * Thanks Thomas!
 */
static void invert_matrix(const GLfloat *b,GLfloat * a)
{
#define MAT(m,r,c) ((m)[(c)*4+(r)])

  GLfloat val, val2;
  GLint   i, j, k, ind;
  GLfloat tmp[16];

  MEMCPY(a,Identity,sizeof(float)*16);
  MEMCPY(tmp, b,sizeof(float)*16);

  for (i = 0; i != 4; i++) {

    val = MAT(tmp,i,i);			/* find pivot */
    ind = i;
    for (j = i + 1; j != 4; j++) {
      if (fabs(MAT(tmp,j,i)) > fabs(val)) {
	ind = j;
	val = MAT(tmp,j,i);
      }
    }

    if (ind != i) {			/* swap columns */
      for (j = 0; j != 4; j++) {
	val2 = MAT(a,i,j);
	MAT(a,i,j) = MAT(a,ind,j);
	MAT(a,ind,j) = val2;
	val2 = MAT(tmp,i,j);
	MAT(tmp,i,j) = MAT(tmp,ind,j);
	MAT(tmp,ind,j) = val2;
      }
    }

    if (val == 0.0F) {	
       /* The matrix is singular (has no inverse).  This isn't really
	* an error since singular matrices can be used for projecting
	* shadows, etc.  We let the inverse be the identity matrix.
	*/
       /*fprintf(stderr,"Singular matrix, no inverse!\n");*/
       MEMCPY( a, Identity, 16*sizeof(GLfloat) );
       return;
    }

    for (j = 0; j != 4; j++) {
      MAT(tmp,i,j) /= val;
      MAT(a,i,j) /= val;
    }

    for (j = 0; j != 4; j++) {		/* eliminate column */
      if (j == i)
	continue;
      val = MAT(tmp,j,i);
      for (k = 0; k != 4; k++) {
	MAT(tmp,j,k) -= MAT(tmp,i,k) * val;
	MAT(a,j,k) -= MAT(a,i,k) * val;
      }
    }
  }
#undef MAT
}




/*
 * Compute the inverse of the current ModelViewMatrix.
 */
void gl_compute_modelview_inverse( GLcontext *ctx )
{
   invert_matrix( ctx->ModelViewMatrix, ctx->ModelViewInv );
   ctx->ModelViewInvValid = GL_TRUE;
}




/*
 * Determine if the given matrix is the identity matrix.
 */
static GLboolean is_identity( const GLfloat m[16] )
{
   if (   m[0]==1.0F && m[4]==0.0F && m[ 8]==0.0F && m[12]==0.0F
       && m[1]==0.0F && m[5]==1.0F && m[ 9]==0.0F && m[13]==0.0F
       && m[2]==0.0F && m[6]==0.0F && m[10]==1.0F && m[14]==0.0F
       && m[3]==0.0F && m[7]==0.0F && m[11]==0.0F && m[15]==1.0F) {
      return GL_TRUE;
   }
   else {
      return GL_FALSE;
   }
}




void gl_Frustum( GLcontext *ctx,
                 GLdouble left, GLdouble right,
	 	 GLdouble bottom, GLdouble top,
		 GLdouble nearval, GLdouble farval )
{
   GLfloat x, y, a, b, c, d;
   GLfloat m[16];

   if (nearval<=0.0 || farval<=0.0) {
      gl_error( ctx,  GL_INVALID_VALUE, "glFrustum(near or far)" );
   }

   x = (2.0*nearval) / (right-left);
   y = (2.0*nearval) / (top-bottom);
   a = (right+left) / (right-left);
   b = (top+bottom) / (top-bottom);
   c = -(farval+nearval) / ( farval-nearval);
   d = -(2.0*farval*nearval) / (farval-nearval);  /* error? */

#define M(row,col)  m[col*4+row]
   M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = a;      M(0,3) = 0.0F;
   M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0.0F;
   M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = c;      M(2,3) = d;
   M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = -1.0F;  M(3,3) = 0.0F;
#undef M

   gl_MultMatrixf( ctx, m );
}



void gl_MatrixMode( GLcontext *ctx, GLenum mode )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glMatrixMode" );
      return;
   }
   switch (mode) {
      case GL_MODELVIEW:
      case GL_PROJECTION:
      case GL_TEXTURE:
         ctx->Transform.MatrixMode = mode;
         break;
      default:
         gl_error( ctx,  GL_INVALID_ENUM, "glMatrixMode" );
   }
}



void gl_PushMatrix( GLcontext *ctx )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glPushMatrix" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         if (ctx->ModelViewStackDepth>=MAX_MODELVIEW_STACK_DEPTH-1) {
            gl_error( ctx,  GL_STACK_OVERFLOW, "glPushMatrix");
            return;
         }
         MEMCPY( ctx->ModelViewStack[ctx->ModelViewStackDepth],
                 ctx->ModelViewMatrix,
                 16*sizeof(GLfloat) );
         ctx->ModelViewStackDepth++;
         break;
      case GL_PROJECTION:
         if (ctx->ProjectionStackDepth>=MAX_PROJECTION_STACK_DEPTH) {
            gl_error( ctx,  GL_STACK_OVERFLOW, "glPushMatrix");
            return;
         }
         MEMCPY( ctx->ProjectionStack[ctx->ProjectionStackDepth],
                 ctx->ProjectionMatrix,
                 16*sizeof(GLfloat) );
         ctx->ProjectionStackDepth++;
         break;
      case GL_TEXTURE:
         if (ctx->TextureStackDepth>=MAX_TEXTURE_STACK_DEPTH) {
            gl_error( ctx,  GL_STACK_OVERFLOW, "glPushMatrix");
            return;
         }
         MEMCPY( ctx->TextureStack[ctx->TextureStackDepth],
                 ctx->TextureMatrix,
                 16*sizeof(GLfloat) );
         ctx->TextureStackDepth++;
         break;
      default:
         abort();
   }
}



void gl_PopMatrix( GLcontext *ctx )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glPopMatrix" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         if (ctx->ModelViewStackDepth==0) {
            gl_error( ctx,  GL_STACK_UNDERFLOW, "glPopMatrix");
            return;
         }
         ctx->ModelViewStackDepth--;
         MEMCPY( ctx->ModelViewMatrix,
                 ctx->ModelViewStack[ctx->ModelViewStackDepth],
                 16*sizeof(GLfloat) );
         ctx->ModelViewInvValid = GL_FALSE;
         break;
      case GL_PROJECTION:
         if (ctx->ProjectionStackDepth==0) {
            gl_error( ctx,  GL_STACK_UNDERFLOW, "glPopMatrix");
            return;
         }
         ctx->ProjectionStackDepth--;
         MEMCPY( ctx->ProjectionMatrix,
                 ctx->ProjectionStack[ctx->ProjectionStackDepth],
                 16*sizeof(GLfloat) );
         break;
      case GL_TEXTURE:
         if (ctx->TextureStackDepth==0) {
            gl_error( ctx,  GL_STACK_UNDERFLOW, "glPopMatrix");
            return;
         }
         ctx->TextureStackDepth--;
         MEMCPY( ctx->TextureMatrix,
                 ctx->TextureStack[ctx->TextureStackDepth],
                 16*sizeof(GLfloat) );
         ctx->IdentityTexMat = is_identity( ctx->TextureMatrix );
         break;
      default:
         abort();
   }
}



void gl_LoadMatrixf( GLcontext *ctx, const GLfloat *m )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glLoadMatrix" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         MEMCPY( ctx->ModelViewMatrix, m, 16*sizeof(GLfloat) );
	 ctx->ModelViewInvValid = GL_FALSE;
	 break;
      case GL_PROJECTION:
	 MEMCPY( ctx->ProjectionMatrix, m, 16*sizeof(GLfloat) );
	 break;
      case GL_TEXTURE:
	 MEMCPY( ctx->TextureMatrix, m, 16*sizeof(GLfloat) );
         ctx->IdentityTexMat = is_identity( ctx->TextureMatrix );
	 break;
      default:
         abort();
   }
}



void gl_MultMatrixf( GLcontext *ctx, const GLfloat *m )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glMultMatrix" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         matmul( ctx->ModelViewMatrix, ctx->ModelViewMatrix, m );
	 ctx->ModelViewInvValid = GL_FALSE;
	 break;
      case GL_PROJECTION:
	 matmul( ctx->ProjectionMatrix, ctx->ProjectionMatrix, m );
	 break;
      case GL_TEXTURE:
	 matmul( ctx->TextureMatrix, ctx->TextureMatrix, m );
         ctx->IdentityTexMat = is_identity( ctx->TextureMatrix );
	 break;
      default:
         abort();
   }
}



/*
 * Generate a 4x4 transformation matrix from glRotate parameters.
 */
void gl_rotation_matrix( GLfloat angle, GLfloat x, GLfloat y, GLfloat z,
                         GLfloat m[] )
{
   /* This function contributed by Erich Boleyn (erich@uruk.org) */
   GLfloat mag, s, c;
   GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

   s = sin( angle * (M_PI / 180.0) );
   c = cos( angle * (M_PI / 180.0) );

   mag = sqrt( x*x + y*y + z*z );

   if (mag == 0.0)
     return;

   x /= mag;
   y /= mag;
   z /= mag;

#define M(row,col)  m[col*4+row]

   /*
    *     Arbitrary axis rotation matrix.
    *
    *  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
    *  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
    *  (which is about the X-axis), and the two composite transforms
    *  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
    *  from the arbitrary axis to the X-axis then back.  They are
    *  all elementary rotations.
    *
    *  Rz' is a rotation about the Z-axis, to bring the axis vector
    *  into the x-z plane.  Then Ry' is applied, rotating about the
    *  Y-axis to bring the axis vector parallel with the X-axis.  The
    *  rotation about the X-axis is then performed.  Ry and Rz are
    *  simply the respective inverse transforms to bring the arbitrary
    *  axis back to it's original orientation.  The first transforms
    *  Rz' and Ry' are considered inverses, since the data from the
    *  arbitrary axis gives you info on how to get to it, not how
    *  to get away from it, and an inverse must be applied.
    *
    *  The basic calculation used is to recognize that the arbitrary
    *  axis vector (x, y, z), since it is of unit length, actually
    *  represents the sines and cosines of the angles to rotate the
    *  X-axis to the same orientation, with theta being the angle about
    *  Z and phi the angle about Y (in the order described above)
    *  as follows:
    *
    *  cos ( theta ) = x / sqrt ( 1 - z^2 )
    *  sin ( theta ) = y / sqrt ( 1 - z^2 )
    *
    *  cos ( phi ) = sqrt ( 1 - z^2 )
    *  sin ( phi ) = z
    *
    *  Note that cos ( phi ) can further be inserted to the above
    *  formulas:
    *
    *  cos ( theta ) = x / cos ( phi )
    *  sin ( theta ) = y / sin ( phi )
    *
    *  ...etc.  Because of those relations and the standard trigonometric
    *  relations, it is pssible to reduce the transforms down to what
    *  is used below.  It may be that any primary axis chosen will give the
    *  same results (modulo a sign convention) using thie method.
    *
    *  Particularly nice is to notice that all divisions that might
    *  have caused trouble when parallel to certain planes or
    *  axis go away with care paid to reducing the expressions.
    *  After checking, it does perform correctly under all cases, since
    *  in all the cases of division where the denominator would have
    *  been zero, the numerator would have been zero as well, giving
    *  the expected result.
    */

   xx = x * x;
   yy = y * y;
   zz = z * z;
   xy = x * y;
   yz = y * z;
   zx = z * x;
   xs = x * s;
   ys = y * s;
   zs = z * s;
   one_c = 1.0F - c;

   M(0,0) = (one_c * xx) + c;
   M(0,1) = (one_c * xy) - zs;
   M(0,2) = (one_c * zx) + ys;
   M(0,3) = 0.0F;

   M(1,0) = (one_c * xy) + zs;
   M(1,1) = (one_c * yy) + c;
   M(1,2) = (one_c * yz) - xs;
   M(1,3) = 0.0F;

   M(2,0) = (one_c * zx) - ys;
   M(2,1) = (one_c * yz) + xs;
   M(2,2) = (one_c * zz) + c;
   M(2,3) = 0.0F;

   M(3,0) = 0.0F;
   M(3,1) = 0.0F;
   M(3,2) = 0.0F;
   M(3,3) = 1.0F;

#undef M
}



void gl_Rotatef( GLcontext *ctx,
                 GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
   GLfloat m[16];
   gl_rotation_matrix( angle, x, y, z, m );
   gl_MultMatrixf( ctx, m );
}



/*
 * Execute a glScale call
 */
void gl_Scalef( GLcontext *ctx, GLfloat x, GLfloat y, GLfloat z )
{
   GLfloat *m;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glScale" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         m = ctx->ModelViewMatrix;
	 ctx->ModelViewInvValid = GL_FALSE;
	 break;
      case GL_PROJECTION:
         m = ctx->ProjectionMatrix;
	 break;
      case GL_TEXTURE:
         m = ctx->TextureMatrix;
	 break;
      default:
         abort();
   }
   m[0] *= x;   m[4] *= y;   m[8]  *= z;
   m[1] *= x;   m[5] *= y;   m[9]  *= z;
   m[2] *= x;   m[6] *= y;   m[10] *= z;
   m[3] *= x;   m[7] *= y;   m[11] *= z;

   if (ctx->Transform.MatrixMode==GL_TEXTURE) {
      ctx->IdentityTexMat = is_identity( ctx->TextureMatrix );
   }
}



/*
 * Execute a glTranslate call
 */
void gl_Translatef( GLcontext *ctx, GLfloat x, GLfloat y, GLfloat z )
{
   GLfloat *m;
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glTranslate" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         m = ctx->ModelViewMatrix;
	 ctx->ModelViewInvValid = GL_FALSE;
	 break;
      case GL_PROJECTION:
         m = ctx->ProjectionMatrix;
	 break;
      case GL_TEXTURE:
         m = ctx->TextureMatrix;
	 break;
      default:
         abort();
   }

   m[12] = m[0] * x + m[4] * y + m[8]  * z + m[12];
   m[13] = m[1] * x + m[5] * y + m[9]  * z + m[13];
   m[14] = m[2] * x + m[6] * y + m[10] * z + m[14];
   m[15] = m[3] * x + m[7] * y + m[11] * z + m[15];

   if (ctx->Transform.MatrixMode==GL_TEXTURE) {
      ctx->IdentityTexMat = is_identity( ctx->TextureMatrix );
   }
}




/*
 * Define a new viewport and reallocate auxillary buffers if the size of
 * the window (color buffer) has changed.
 */
void gl_Viewport( GLcontext *ctx,
                  GLint x, GLint y, GLsizei width, GLsizei height )
{
   GLint newsize;
   GLuint buf_width, buf_height;

   if (width<0 || height<0) {
      gl_error( ctx,  GL_INVALID_VALUE, "glViewport" );
      return;
   }
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glViewport" );
      return;
   }

   /* clamp width, and height to implementation dependent range */
   width  = CLAMP( width,  1, MAX_WIDTH );
   height = CLAMP( height, 1, MAX_HEIGHT );

   /* ask device driver for size of output buffer */
   (*ctx->Driver.GetBufferSize)( ctx, &buf_width, &buf_height );

   /* see if size of device driver's color buffer (window) has changed */
   newsize = ctx->Buffer->Width!=buf_width || ctx->Buffer->Height!=buf_height;

   /* save buffer size */
   ctx->Buffer->Width = buf_width;
   ctx->Buffer->Height = buf_height;

   /* Save viewport */
   ctx->Viewport.X = x;
   ctx->Viewport.Width = width;
   ctx->Viewport.Y = y;
   ctx->Viewport.Height = height;

   /* compute scale and bias values */
   ctx->Viewport.Sx = (GLfloat) width / 2.0F;
   ctx->Viewport.Tx = ctx->Viewport.Sx + x;
   ctx->Viewport.Sy = (GLfloat) height / 2.0F;
   ctx->Viewport.Ty = ctx->Viewport.Sy + y;

   /* Reallocate other buffers if needed. */
   if (newsize && ctx->Visual->DepthBits>0) {
      /* reallocate depth buffer */
      (*ctx->Driver.AllocDepthBuffer)( ctx );
   }
   if (newsize && ctx->Visual->StencilBits>0) {
      /* reallocate stencil buffer */
      gl_alloc_stencil_buffer( ctx );
   }
   if (newsize && ctx->Visual->AccumBits>0) {
      /* reallocate accum buffer */
      gl_alloc_accum_buffer( ctx );
   }
   if (newsize
       && (ctx->Visual->FrontAlphaEnabled || ctx->Visual->BackAlphaEnabled)) {
      gl_alloc_alpha_buffers( ctx );
   }

   ctx->NewState |= NEW_ALL;   /* just to be safe */
}



