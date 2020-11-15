/* $Id: project.c,v 1.1 1996/09/27 01:19:39 brianp Exp $ */

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
 * $Log: project.c,v $
 * Revision 1.1  1996/09/27 01:19:39  brianp
 * Initial revision
 *
 */


#include <stdio.h>
#include <string.h>
#include <math.h>
#include "gluP.h"


/*
 * This code was contributed by Marc Buffat (buffat@mecaflu.ec-lyon.fr).
 * Thanks Marc!!!
 */



/* implementation de gluProject et gluUnproject */
/* M. Buffat 17/2/95 */



/*
 * Transform a point (column vector) by a 4x4 matrix.  I.e.  out = m * in
 * Input:  m - the 4x4 matrix
 *         in - the 4x1 vector
 * Output:  out - the resulting 4x1 vector.
 */
static void transform_point( GLdouble out[4], const GLdouble m[16],
			     const GLdouble in[4] )
{
#define M(row,col)  m[col*4+row]
   out[0] = M(0,0) * in[0] + M(0,1) * in[1] + M(0,2) * in[2] + M(0,3) * in[3];
   out[1] = M(1,0) * in[0] + M(1,1) * in[1] + M(1,2) * in[2] + M(1,3) * in[3];
   out[2] = M(2,0) * in[0] + M(2,1) * in[1] + M(2,2) * in[2] + M(2,3) * in[3];
   out[3] = M(3,0) * in[0] + M(3,1) * in[1] + M(3,2) * in[2] + M(3,3) * in[3];
#undef M
}




/*
 * Perform a 4x4 matrix multiplication  (product = a x b).
 * Input:  a, b - matrices to multiply
 * Output:  product - product of a and b
 */
static void matmul( GLdouble *product, const GLdouble *a, const GLdouble *b )
{
   /* This matmul was contributed by Thomas Malik */
   GLdouble temp[16];
   GLint i;

#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define T(row,col)  temp[(col<<2)+row]

   /* i-te Zeile */
   for (i = 0; i < 4; i++)
     {
	T(i, 0) = A(i, 0) * B(0, 0) + A(i, 1) * B(1, 0) + A(i, 2) * B(2, 0) + A(i, 3) * B(3, 0);
	T(i, 1) = A(i, 0) * B(0, 1) + A(i, 1) * B(1, 1) + A(i, 2) * B(2, 1) + A(i, 3) * B(3, 1);
	T(i, 2) = A(i, 0) * B(0, 2) + A(i, 1) * B(1, 2) + A(i, 2) * B(2, 2) + A(i, 3) * B(3, 2);
	T(i, 3) = A(i, 0) * B(0, 3) + A(i, 1) * B(1, 3) + A(i, 2) * B(2, 3) + A(i, 3) * B(3, 3);
     }

#undef A
#undef B
#undef T
   MEMCPY( product, temp, 16*sizeof(GLdouble) );
}




/*
 * Find the inverse of the 4 by 4 matrix b using gausian elimination
 * and return it in a.
 *
 * This function was contributed by Thomas Malik (malik@rhrk.uni-kl.de).
 * Thanks Thomas!
 */
static void invert_matrix(const GLdouble *b,GLdouble * a)
{
  static GLdouble identity[16] =
    {
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0
    };

#define MAT(m,r,c) ((m)[(c)*4+(r)])

  GLdouble val, val2;
  GLint   i, j, k, ind;
  GLdouble tmp[16];

  MEMCPY(a,identity,sizeof(double)*16);
  MEMCPY(tmp, b,sizeof(double)*16);

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
      fprintf(stderr,"Singular matrix, no inverse!\n");
      MEMCPY( a, identity, 16*sizeof(GLdouble) );  /* return the identity */
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




/* projection du point (objx,objy,obz) sur l'ecran (winx,winy,winz) */
GLint gluProject(GLdouble objx,GLdouble objy,GLdouble objz,
                 const GLdouble model[16],const GLdouble proj[16],
                 const GLint viewport[4],
                 GLdouble *winx,GLdouble *winy,GLdouble *winz)
{
    /* matrice de transformation */
    GLdouble in[4],out[4];

    /* initilise la matrice et le vecteur a transformer */
    in[0]=objx; in[1]=objy; in[2]=objz; in[3]=1.0;
    transform_point(out,model,in);
    transform_point(in,proj,out);

    /* d'ou le resultat normalise entre -1 et 1*/
    in[0]/=in[3];in[1]/=in[3];in[2]/=in[3];

    /* en coordonnees ecran */
    *winx = viewport[0]+(1+in[0])*viewport[2]/2;
    *winy = viewport[1]+(1+in[1])*viewport[3]/2;
    /* entre 0 et 1 suivant z */
    *winz = (1+in[2])/2;
    return GL_TRUE;
}



/* transformation du point ecran (winx,winy,winz) en point objet */
GLint gluUnProject(GLdouble winx,GLdouble winy,GLdouble winz,
                   const GLdouble model[16],const GLdouble proj[16],
                   const GLint viewport[4],
                   GLdouble *objx,GLdouble *objy,GLdouble *objz)
{
    /* matrice de transformation */
    GLdouble m[16], A[16];
    GLdouble in[4],out[4];

    /* transformation coordonnees normalisees entre -1 et 1 */
    in[0]=(winx-viewport[0])*2/viewport[2] - 1.0;
    in[1]=(winy-viewport[1])*2/viewport[3] - 1.0;
    in[2]=2*winz - 1.0;
    in[3]=1.0;

    /* calcul transformation inverse */
    matmul(A,proj,model); invert_matrix(A,m);

    /* d'ou les coordonnees objets */
    transform_point(out,m,in);
    *objx=out[0]/out[3];
    *objy=out[1]/out[3];
    *objz=out[2]/out[3];
    return GL_TRUE;
}

