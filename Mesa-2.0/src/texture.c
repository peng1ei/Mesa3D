/* $Id: texture.c,v 1.4 1996/09/27 01:30:24 brianp Exp $ */

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
 * $Log: texture.c,v $
 * Revision 1.4  1996/09/27 01:30:24  brianp
 * added missing default cases to switches
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "context.h"
#include "dlist.h"
#include "macros.h"
#include "pb.h"
#include "teximage.h"
#include "texture.h"
#include "types.h"





/**********************************************************************/
/*                       Texture Environment                          */
/**********************************************************************/



void gl_TexEnvfv( GLcontext *ctx,
                  GLenum target, GLenum pname, const GLfloat *param )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glTexEnv" );
      return;
   }

   if (target!=GL_TEXTURE_ENV) {
      gl_error( ctx, GL_INVALID_ENUM, "glTexEnv(target)" );
      return;
   }

   if (pname==GL_TEXTURE_ENV_MODE) {
      GLenum mode = (GLenum) (GLint) *param;
      switch (mode) {
	 case GL_MODULATE:
	 case GL_BLEND:
	 case GL_DECAL:
	 case GL_REPLACE_EXT:
	    ctx->Texture.EnvMode = mode;
	    break;
	 default:
	    gl_error( ctx, GL_INVALID_ENUM, "glTexEnv(param)" );
	    return;
      }
   }
   else if (pname==GL_TEXTURE_ENV_COLOR) {
      ctx->Texture.EnvColor[0] = CLAMP( param[0], 0.0, 1.0 );
      ctx->Texture.EnvColor[1] = CLAMP( param[1], 0.0, 1.0 );
      ctx->Texture.EnvColor[2] = CLAMP( param[2], 0.0, 1.0 );
      ctx->Texture.EnvColor[3] = CLAMP( param[3], 0.0, 1.0 );
   }
   else {
      gl_error( ctx, GL_INVALID_ENUM, "glTexEnv(pname)" );
      return;
   }
}





void gl_GetTexEnvfv( GLcontext *ctx,
                     GLenum target, GLenum pname, GLfloat *params )
{
   if (target!=GL_TEXTURE_ENV) {
      gl_error( ctx, GL_INVALID_ENUM, "glGetTexEnvfv(target)" );
      return;
   }
   switch (pname) {
      case GL_TEXTURE_ENV_MODE:
         *params = (GLfloat) ctx->Texture.EnvMode;
	 break;
      case GL_TEXTURE_ENV_COLOR:
	 COPY_4V( params, ctx->Texture.EnvColor );
	 break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glGetTexEnvfv(pname)" );
   }
}


void gl_GetTexEnviv( GLcontext *ctx,
                     GLenum target, GLenum pname, GLint *params )
{
   if (target!=GL_TEXTURE_ENV) {
      gl_error( ctx, GL_INVALID_ENUM, "glGetTexEnvfv(target)" );
      return;
   }
   switch (pname) {
      case GL_TEXTURE_ENV_MODE:
         *params = (GLint) ctx->Texture.EnvMode;
	 break;
      case GL_TEXTURE_ENV_COLOR:
	 params[0] = FLOAT_TO_INT( ctx->Texture.EnvColor[0] );
	 params[1] = FLOAT_TO_INT( ctx->Texture.EnvColor[1] );
	 params[2] = FLOAT_TO_INT( ctx->Texture.EnvColor[2] );
	 params[3] = FLOAT_TO_INT( ctx->Texture.EnvColor[3] );
	 break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glGetTexEnvfv(pname)" );
   }
}




/**********************************************************************/
/*                       Texture Parameters                           */
/**********************************************************************/


void gl_TexParameterfv( GLcontext *ctx,
                        GLenum target, GLenum pname, const GLfloat *params )
{
   GLenum eparam = (GLenum) (GLint) params[0];

   if (target==GL_TEXTURE_1D) {
      switch (pname) {
	 case GL_TEXTURE_MIN_FILTER:
	    if (eparam==GL_NEAREST || eparam==GL_LINEAR
		|| eparam==GL_NEAREST_MIPMAP_NEAREST
		|| eparam==GL_LINEAR_MIPMAP_NEAREST
		|| eparam==GL_NEAREST_MIPMAP_LINEAR
		|| eparam==GL_LINEAR_MIPMAP_LINEAR) {
	       ctx->Texture.Current1D->MinFilter = eparam;
               ctx->NewState |= NEW_TEXTURING;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
	    }
	    break;
	 case GL_TEXTURE_MAG_FILTER:
	    if (eparam==GL_NEAREST || eparam==GL_LINEAR) {
	       ctx->Texture.Current1D->MagFilter = eparam;
               ctx->NewState |= NEW_TEXTURING;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
	    }
	    break;
	 case GL_TEXTURE_WRAP_S:
	    if (eparam==GL_CLAMP || eparam==GL_REPEAT) {
	       ctx->Texture.Current1D->WrapS = eparam;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
	    }
	    break;
	 case GL_TEXTURE_WRAP_T:
	    if (eparam==GL_CLAMP || eparam==GL_REPEAT) {
	       ctx->Texture.Current1D->WrapT = eparam;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
	    }
	    break;
         case GL_TEXTURE_BORDER_COLOR:
            {
               GLint *bc = ctx->Texture.Current2D->BorderColor;
               bc[0] = CLAMP((GLint)(params[0]*255.0), 0, 255);
               bc[1] = CLAMP((GLint)(params[1]*255.0), 0, 255);
               bc[2] = CLAMP((GLint)(params[2]*255.0), 0, 255);
               bc[3] = CLAMP((GLint)(params[3]*255.0), 0, 255);
            }
            break;
	 default:
	    gl_error( ctx, GL_INVALID_ENUM, "glTexParameter(pname)" );
      }
   }
   else if (target==GL_TEXTURE_2D) {
      switch (pname) {
	 case GL_TEXTURE_MIN_FILTER:
	    if (eparam==GL_NEAREST || eparam==GL_LINEAR
		|| eparam==GL_NEAREST_MIPMAP_NEAREST
		|| eparam==GL_LINEAR_MIPMAP_NEAREST
		|| eparam==GL_NEAREST_MIPMAP_LINEAR
		|| eparam==GL_LINEAR_MIPMAP_LINEAR) {
	       ctx->Texture.Current2D->MinFilter = eparam;
               ctx->NewState |= NEW_TEXTURING;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
	    }
	    break;
	 case GL_TEXTURE_MAG_FILTER:
	    if (eparam==GL_NEAREST || eparam==GL_LINEAR) {
	       ctx->Texture.Current2D->MagFilter = eparam;
               ctx->NewState |= NEW_TEXTURING;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
	    }
	    break;
	 case GL_TEXTURE_WRAP_S:
	    if (eparam==GL_CLAMP || eparam==GL_REPEAT) {
	       ctx->Texture.Current2D->WrapS = eparam;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
	    }
	    break;
	 case GL_TEXTURE_WRAP_T:
	    if (eparam==GL_CLAMP || eparam==GL_REPEAT) {
	       ctx->Texture.Current2D->WrapT = eparam;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
	    }
	    break;
         case GL_TEXTURE_BORDER_COLOR:
            {
               GLint *bc = ctx->Texture.Current2D->BorderColor;
               bc[0] = CLAMP((GLint)(params[0]*255.0), 0, 255);
               bc[1] = CLAMP((GLint)(params[1]*255.0), 0, 255);
               bc[2] = CLAMP((GLint)(params[2]*255.0), 0, 255);
               bc[3] = CLAMP((GLint)(params[3]*255.0), 0, 255);
            }
            break;
	 default:
	    gl_error( ctx, GL_INVALID_ENUM, "glTexParameter(pname)" );
      }
   }
   else {
      gl_error( ctx, GL_INVALID_ENUM, "glTexParameter(target)" );
   }
}



void gl_GetTexLevelParameterfv( GLcontext *ctx, GLenum target, GLint level,
                                GLenum pname, GLfloat *params )
{
   struct gl_texture_image *tex;

   if (level<0 || level>=MAX_TEXTURE_LEVELS) {
      gl_error( ctx, GL_INVALID_VALUE, "glGetTexLevelParameterfv" );
      return;
   }

   switch (target) {
      case GL_TEXTURE_1D:
         tex = ctx->Texture.Current1D->Image[level];
         switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = (GLfloat) (tex->Width + tex->Border);
	       break;
	    case GL_TEXTURE_COMPONENTS:
	       *params = (GLfloat) tex->Format;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = (GLfloat) tex->Border;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameterfv(pname)" );
	 }
	 break;
      case GL_TEXTURE_2D:
         tex = ctx->Texture.Current2D->Image[level];
	 switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = (GLfloat) (tex->Width + tex->Border);
	       break;
	    case GL_TEXTURE_HEIGHT:
	       *params = (GLfloat) (tex->Height + tex->Border);
	       break;
	    case GL_TEXTURE_COMPONENTS:
	       *params = (GLfloat) tex->Format;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = (GLfloat) tex->Border;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameterfv(pname)" );
	 }
	 break;
#ifdef GL_VERSION_1_1
      case GL_PROXY_TEXTURE_1D:
         tex = ctx->Texture.Proxy1D->Image[level];
         switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = (GLfloat) (tex->Width + tex->Border);
	       break;
	    case GL_TEXTURE_COMPONENTS:
	       *params = (GLfloat) tex->Format;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = (GLfloat) tex->Border;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameterfv(pname)" );
	 }
	 break;
      case GL_PROXY_TEXTURE_2D:
         tex = ctx->Texture.Proxy2D->Image[level];
	 switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = (GLfloat) (tex->Width + tex->Border);
	       break;
	    case GL_TEXTURE_HEIGHT:
	       *params = (GLfloat) (tex->Height + tex->Border);
	       break;
	    case GL_TEXTURE_COMPONENTS:
	       *params = (GLfloat) tex->Format;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = (GLfloat) tex->Border;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameterfv(pname)" );
	 }
	 break;
#endif
     default:
	 gl_error( ctx, GL_INVALID_ENUM, "glGetTexLevelParameterfv(target)" );
   }	 
}



void gl_GetTexLevelParameteriv( GLcontext *ctx, GLenum target, GLint level,
                                GLenum pname, GLint *params )
{
   struct gl_texture_image *tex;

   if (level<0 || level>=MAX_TEXTURE_LEVELS) {
      gl_error( ctx, GL_INVALID_VALUE, "glGetTexLevelParameteriv" );
      return;
   }

   switch (target) {
      case GL_TEXTURE_1D:
         tex = ctx->Texture.Current1D->Image[level];
         switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = tex->Width + tex->Border;
	       break;
	    case GL_TEXTURE_COMPONENTS:
	       *params = tex->Format;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = tex->Border;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameteriv(pname)" );
	 }
	 break;
      case GL_TEXTURE_2D:
         tex = ctx->Texture.Current2D->Image[level];
	 switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = tex->Width + tex->Border;
	       break;
	    case GL_TEXTURE_HEIGHT:
	       *params = tex->Height + tex->Border;
	       break;
	    case GL_TEXTURE_COMPONENTS:
	       *params = tex->Format;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = tex->Border;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameteriv(pname)" );
	 }
	 break;
#ifdef GL_VERSION_1_1
      case GL_PROXY_TEXTURE_1D:
         tex = ctx->Texture.Proxy1D->Image[level];
         switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = tex->Width + tex->Border;
	       break;
	    case GL_TEXTURE_COMPONENTS:
	       *params = tex->Format;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = tex->Border;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameteriv(pname)" );
	 }
	 break;
      case GL_PROXY_TEXTURE_2D:
         tex = ctx->Texture.Proxy2D->Image[level];
	 switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = tex->Width + tex->Border;
	       break;
	    case GL_TEXTURE_HEIGHT:
	       *params = tex->Height + tex->Border;
	       break;
	    case GL_TEXTURE_COMPONENTS:
	       *params = tex->Format;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = tex->Border;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameteriv(pname)" );
	 }
	 break;
#endif
     default:
	 gl_error( ctx, GL_INVALID_ENUM, "glGetTexLevelParameteriv(target)" );
   }	 
}




void gl_GetTexParameterfv( GLcontext *ctx,
                           GLenum target, GLenum pname, GLfloat *params )
{
   switch (target) {
      case GL_TEXTURE_1D:
         switch (pname) {
	    case GL_TEXTURE_MAG_FILTER:
	       *params = (GLfloat) ctx->Texture.Current1D->MagFilter;
	       break;
	    case GL_TEXTURE_MIN_FILTER:
	       *params = (GLfloat) ctx->Texture.Current1D->MinFilter;
	       break;
	    case GL_TEXTURE_WRAP_S:
	       *params = (GLfloat) ctx->Texture.Current1D->WrapS;
	       break;
	    case GL_TEXTURE_WRAP_T:
	       *params = (GLfloat) ctx->Texture.Current1D->WrapT;
	       break;
	    case GL_TEXTURE_BORDER_COLOR:
               params[0] = ctx->Texture.Current1D->BorderColor[0] / 255.0f;
               params[1] = ctx->Texture.Current1D->BorderColor[1] / 255.0f;
               params[2] = ctx->Texture.Current1D->BorderColor[2] / 255.0f;
               params[3] = ctx->Texture.Current1D->BorderColor[3] / 255.0f;
	       break;
	    case GL_TEXTURE_RESIDENT_EXT:
               *params = (GLfloat) GL_TRUE;
	       break;
	    case GL_TEXTURE_PRIORITY_EXT:
               *params = ctx->Texture.Current1D->Priority;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM, "glGetTexParameterfv(pname)" );
	 }
         break;
      case GL_TEXTURE_2D:
         switch (pname) {
	    case GL_TEXTURE_MAG_FILTER:
	       *params = (GLfloat) ctx->Texture.Current2D->MagFilter;
	       break;
	    case GL_TEXTURE_MIN_FILTER:
	       *params = (GLfloat) ctx->Texture.Current2D->MinFilter;
	       break;
	    case GL_TEXTURE_WRAP_S:
	       *params = (GLfloat) ctx->Texture.Current2D->WrapS;
	       break;
	    case GL_TEXTURE_WRAP_T:
	       *params = (GLfloat) ctx->Texture.Current2D->WrapT;
	       break;
	    case GL_TEXTURE_BORDER_COLOR:
               params[0] = ctx->Texture.Current2D->BorderColor[0] / 255.0f;
               params[1] = ctx->Texture.Current2D->BorderColor[1] / 255.0f;
               params[2] = ctx->Texture.Current2D->BorderColor[2] / 255.0f;
               params[3] = ctx->Texture.Current2D->BorderColor[3] / 255.0f;
               break;
	    case GL_TEXTURE_RESIDENT_EXT:
               *params = (GLfloat) GL_TRUE;
	       break;
	    case GL_TEXTURE_PRIORITY_EXT:
               *params = ctx->Texture.Current2D->Priority;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM, "glGetTexParameterfv(pname)" );
	 }
	 break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glGetTexParameterfv(target)" );
   }
}


void gl_GetTexParameteriv( GLcontext *ctx,
                           GLenum target, GLenum pname, GLint *params )
{
   switch (target) {
      case GL_TEXTURE_1D:
         switch (pname) {
	    case GL_TEXTURE_MAG_FILTER:
	       *params = (GLint) ctx->Texture.Current1D->MagFilter;
	       break;
	    case GL_TEXTURE_MIN_FILTER:
	       *params = (GLint) ctx->Texture.Current1D->MinFilter;
	       break;
	    case GL_TEXTURE_WRAP_S:
	       *params = (GLint) ctx->Texture.Current1D->WrapS;
	       break;
	    case GL_TEXTURE_WRAP_T:
	       *params = (GLint) ctx->Texture.Current1D->WrapT;
	       break;
	    case GL_TEXTURE_BORDER_COLOR:
               {
                  GLfloat color[4];
                  color[0] = ctx->Texture.Current1D->BorderColor[0]/255.0;
                  color[1] = ctx->Texture.Current1D->BorderColor[1]/255.0;
                  color[2] = ctx->Texture.Current1D->BorderColor[2]/255.0;
                  color[3] = ctx->Texture.Current1D->BorderColor[3]/255.0;
                  params[0] = FLOAT_TO_INT( color[0] );
                  params[1] = FLOAT_TO_INT( color[1] );
                  params[2] = FLOAT_TO_INT( color[2] );
                  params[3] = FLOAT_TO_INT( color[3] );
               }
	       break;
	    case GL_TEXTURE_RESIDENT_EXT:
               *params = (GLint) GL_TRUE;
	       break;
	    case GL_TEXTURE_PRIORITY_EXT:
               *params = (GLint) ctx->Texture.Current1D->Priority;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM, "glGetTexParameteriv(pname)" );
	 }
         break;
      case GL_TEXTURE_2D:
         switch (pname) {
	    case GL_TEXTURE_MAG_FILTER:
	       *params = (GLint) ctx->Texture.Current2D->MagFilter;
	       break;
	    case GL_TEXTURE_MIN_FILTER:
	       *params = (GLint) ctx->Texture.Current2D->MinFilter;
	       break;
	    case GL_TEXTURE_WRAP_S:
	       *params = (GLint) ctx->Texture.Current2D->WrapS;
	       break;
	    case GL_TEXTURE_WRAP_T:
	       *params = (GLint) ctx->Texture.Current2D->WrapT;
	       break;
	    case GL_TEXTURE_BORDER_COLOR:
               {
                  GLfloat color[4];
                  color[0] = ctx->Texture.Current2D->BorderColor[0]/255.0;
                  color[1] = ctx->Texture.Current2D->BorderColor[1]/255.0;
                  color[2] = ctx->Texture.Current2D->BorderColor[2]/255.0;
                  color[3] = ctx->Texture.Current2D->BorderColor[3]/255.0;
                  params[0] = FLOAT_TO_INT( color[0] );
                  params[1] = FLOAT_TO_INT( color[1] );
                  params[2] = FLOAT_TO_INT( color[2] );
                  params[3] = FLOAT_TO_INT( color[3] );
               }
	       break;
	    case GL_TEXTURE_RESIDENT_EXT:
               *params = (GLint) GL_TRUE;
	       break;
	    case GL_TEXTURE_PRIORITY_EXT:
               *params = (GLint) ctx->Texture.Current2D->Priority;
	       break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM, "glGetTexParameteriv(pname)" );
	 }
	 break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glGetTexParameteriv(target)" );
   }
}




/**********************************************************************/
/*                    Texture Coord Generation                        */
/**********************************************************************/


void gl_TexGenfv( GLcontext *ctx,
                  GLenum coord, GLenum pname, const GLfloat *params )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glTexGenfv" );
      return;
   }

   switch( coord ) {
      case GL_S:
         if (pname==GL_TEXTURE_GEN_MODE) {
	    GLenum mode = (GLenum) (GLint) *params;
	    if (mode==GL_OBJECT_LINEAR ||
		mode==GL_EYE_LINEAR ||
		mode==GL_SPHERE_MAP) {
	       ctx->Texture.GenModeS = mode;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(param)" );
	       return;
	    }
	 }
	 else if (pname==GL_OBJECT_PLANE) {
	    ctx->Texture.ObjectPlaneS[0] = params[0];
	    ctx->Texture.ObjectPlaneS[1] = params[1];
	    ctx->Texture.ObjectPlaneS[2] = params[2];
	    ctx->Texture.ObjectPlaneS[3] = params[3];
	 }
	 else if (pname==GL_EYE_PLANE) {
	    /* TODO:  xform plane by modelview??? */
	    ctx->Texture.EyePlaneS[0] = params[0];
	    ctx->Texture.EyePlaneS[1] = params[1];
	    ctx->Texture.EyePlaneS[2] = params[2];
	    ctx->Texture.EyePlaneS[3] = params[3];
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(pname)" );
	    return;
	 }
	 break;
      case GL_T:
         if (pname==GL_TEXTURE_GEN_MODE) {
	    GLenum mode = (GLenum) (GLint) *params;
	    if (mode==GL_OBJECT_LINEAR ||
		mode==GL_EYE_LINEAR ||
		mode==GL_SPHERE_MAP) {
	       ctx->Texture.GenModeT = mode;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(param)" );
	       return;
	    }
	 }
	 else if (pname==GL_OBJECT_PLANE) {
	    ctx->Texture.ObjectPlaneT[0] = params[0];
	    ctx->Texture.ObjectPlaneT[1] = params[1];
	    ctx->Texture.ObjectPlaneT[2] = params[2];
	    ctx->Texture.ObjectPlaneT[3] = params[3];
	 }
	 else if (pname==GL_EYE_PLANE) {
	    ctx->Texture.EyePlaneT[0] = params[0];
	    ctx->Texture.EyePlaneT[1] = params[1];
	    ctx->Texture.EyePlaneT[2] = params[2];
	    ctx->Texture.EyePlaneT[3] = params[3];
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(pname)" );
	    return;
	 }
	 break;
      case GL_R:
         if (pname==GL_TEXTURE_GEN_MODE) {
	    GLenum mode = (GLenum) (GLint) *params;
	    if (mode==GL_OBJECT_LINEAR ||
		mode==GL_EYE_LINEAR) {
	       ctx->Texture.GenModeR = mode;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(param)" );
	       return;
	    }
	 }
	 else if (pname==GL_OBJECT_PLANE) {
	    ctx->Texture.ObjectPlaneR[0] = params[0];
	    ctx->Texture.ObjectPlaneR[1] = params[1];
	    ctx->Texture.ObjectPlaneR[2] = params[2];
	    ctx->Texture.ObjectPlaneR[3] = params[3];
	 }
	 else if (pname==GL_EYE_PLANE) {
	    ctx->Texture.EyePlaneR[0] = params[0];
	    ctx->Texture.EyePlaneR[1] = params[1];
	    ctx->Texture.EyePlaneR[2] = params[2];
	    ctx->Texture.EyePlaneR[3] = params[3];
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(pname)" );
	    return;
	 }
	 break;
      case GL_Q:
         if (pname==GL_TEXTURE_GEN_MODE) {
	    GLenum mode = (GLenum) (GLint) *params;
	    if (mode==GL_OBJECT_LINEAR ||
		mode==GL_EYE_LINEAR) {
	       ctx->Texture.GenModeQ = mode;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(param)" );
	       return;
	    }
	 }
	 else if (pname==GL_OBJECT_PLANE) {
	    ctx->Texture.ObjectPlaneQ[0] = params[0];
	    ctx->Texture.ObjectPlaneQ[1] = params[1];
	    ctx->Texture.ObjectPlaneQ[2] = params[2];
	    ctx->Texture.ObjectPlaneQ[3] = params[3];
	 }
	 else if (pname==GL_EYE_PLANE) {
	    ctx->Texture.EyePlaneQ[0] = params[0];
	    ctx->Texture.EyePlaneQ[1] = params[1];
	    ctx->Texture.EyePlaneQ[2] = params[2];
	    ctx->Texture.EyePlaneQ[3] = params[3];
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(pname)" );
	    return;
	 }
	 break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(coord)" );
	 return;
   }

   ctx->NewState |= NEW_TEXTURING;
}



void gl_GetTexGendv( GLcontext *ctx,
                     GLenum coord, GLenum pname, GLdouble *params )
{
   /* TODO */
}

void gl_GetTexGenfv( GLcontext *ctx,
                     GLenum coord, GLenum pname, GLfloat *params )
{
   /* TODO */
}

void gl_GetTexGeniv( GLcontext *ctx,
                     GLenum coord, GLenum pname, GLint *params )
{
   /* TODO */
}



/*
 * Perform automatic texture coordinate generation.
 * Input:  obj - vertex in object coordinate system
 *         eye - vertex in eye coordinate system
 *         normal - normal vector in eye coordinate system
 * Output:  texcoord - the resuling texture coordinate, if TexGen enabled.
 */
void gl_do_texgen( GLcontext *ctx,
                   const GLfloat obj[4],
                   const GLfloat eye[4],
                   const GLfloat normal[3],
                   GLfloat texcoord[4] )
{
   GLfloat u[3], two_nn, m, fx, fy, fz;

   if (ctx->Texture.TexGenEnabled & S_BIT) {
      switch( ctx->Texture.GenModeS) {
	 case GL_OBJECT_LINEAR:
            texcoord[0] = DOT4( obj, ctx->Texture.ObjectPlaneS );
	    break;
	 case GL_EYE_LINEAR:
	    texcoord[0] = DOT4( eye, ctx->Texture.EyePlaneS );
	    break;
	 case GL_SPHERE_MAP:
            COPY_3V( u, eye );
            NORMALIZE_3V( u );
	    two_nn = 2.0*DOT3(normal,normal);
	    fx = u[0] - two_nn * u[0];
	    fy = u[1] - two_nn * u[1];
	    fz = u[2] - two_nn * u[2];
	    m = 2.0 * sqrt( fx*fx + fy*fy + (fz+1.0)*(fz+1.0) );
	    if (m==0.0) {
	       texcoord[0] = 0.0;
	    }
	    else {
	       texcoord[0] = fx / m + 0.5;
	    }
	    break;
         default:
            abort();
      }
   }

   if (ctx->Texture.TexGenEnabled & T_BIT) {
      switch( ctx->Texture.GenModeT) {
	 case GL_OBJECT_LINEAR:
	    texcoord[1] = DOT4( obj, ctx->Texture.ObjectPlaneT );
	    break;
	 case GL_EYE_LINEAR:
	    texcoord[1] = DOT4( eye, ctx->Texture.EyePlaneT );
	    break;
	 case GL_SPHERE_MAP:
	    /* TODO: safe to assume that m and fy valid from above??? */
	    if (m==0.0) {
	       texcoord[1] = 0.0;
	    }
	    else {
	       texcoord[1] = fy / m + 0.5;
	    }
	    break;
         default:
            abort();
      }
   }

   if (ctx->Texture.TexGenEnabled & R_BIT) {
      switch( ctx->Texture.GenModeR) {
	 case GL_OBJECT_LINEAR:
	    texcoord[2] = DOT4( obj, ctx->Texture.ObjectPlaneR );
	    break;
	 case GL_EYE_LINEAR:
	    texcoord[2] = DOT4( eye, ctx->Texture.EyePlaneR );
	    break;
         default:
            abort();
      }
   }

   if (ctx->Texture.TexGenEnabled & Q_BIT) {
      switch( ctx->Texture.GenModeQ) {
	 case GL_OBJECT_LINEAR:
	    texcoord[3] = DOT4( obj, ctx->Texture.ObjectPlaneQ );
	    break;
	 case GL_EYE_LINEAR:
	    texcoord[3] = DOT4( eye, ctx->Texture.EyePlaneQ );
	    break;
         default:
            abort();
      }
   }
}





/**********************************************************************/
/*                    1-D Texture Sampling Functions                  */
/**********************************************************************/


/*
 * Return the fractional part of x.
 */
#define frac(x) ((GLfloat)(x)-floor((GLfloat)x))




/*
 * Given 1-D texture image and an (i) texel column coordinate, return the
 * texel color.
 */
static void get_1d_texel( struct gl_texture_image *img, GLint i,
                          GLubyte *red, GLubyte *green, GLubyte *blue,
                          GLubyte *alpha )
{
   GLubyte *texel;

   /* DEBUG */
   GLint width = img->Width;
   if (i<0 || i>=width)  abort();

   switch (img->Format) {
      case GL_ALPHA:
      case GL_LUMINANCE:
      case GL_INTENSITY:
         *red   = img->Data[ i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + i * 2;
         *red   = texel[0];
         *alpha = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + i * 3;
         *red   = texel[0];
         *green = texel[1];
         *blue  = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + i * 4;
         *red   = texel[0];
         *green = texel[1];
         *blue  = texel[2];
         *alpha = texel[3];
         return;
      default:
         abort();
   }
}



/*
 * Return a texture sample for (s) using GL_NEAREST filter.
 */
static void sample_1d_nearest( GLcontext *ctx,
                               struct gl_texture_image *img,
                               GLfloat s,
                               GLubyte *red, GLubyte *green,
                               GLubyte *blue, GLubyte *alpha )
{
   GLint width = img->Width;  /* width is a power of two */
   GLint i;
   GLubyte *texel;

   /* Clamp/Repeat S and convert to integer texel coordinate */
   if (ctx->Texture.Current1D->WrapS==GL_REPEAT) {
      /* s limited to [0,1) */
      /* i limited to [0,width-1] */
      i = (GLint) (s * width);
      i &= (width-1);
   }
   else {
      /* s limited to [0,1] */
      /* i limited to [0,width-1] */
      if (s<0.0F) {
         i = 0;
      }
      else if (s>1.0F) {
         i = width-1;
      }
      else {
         i = (GLint) (s * width);
      }
   }

   /* Get the texel */
   switch (img->Format) {
      case GL_ALPHA:
      case GL_LUMINANCE:
      case GL_INTENSITY:
         *red   = img->Data[i];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + i * 2;
         *red   = texel[0];
         *alpha = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + i * 3;
         *red   = texel[0];
         *green = texel[1];
         *blue  = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + i * 4;
         *red   = texel[0];
         *green = texel[1];
         *blue  = texel[2];
         *alpha = texel[3];
         return;
      default:
         abort();
   }
}




/*
 * Return a texture sample for (s) using GL_LINEAR filter.
 */
static void sample_1d_linear( GLcontext *ctx,
                              struct gl_texture_image *img,
                              GLfloat s,
                              GLubyte *red, GLubyte *green,
                              GLubyte *blue, GLubyte *alpha )
{
   GLint width = img->Width;
   GLint i0, i1;
   GLfloat u;
   GLint i0border, i1border;

   u = s * width;
   if (ctx->Texture.Current1D->WrapS==GL_REPEAT) {
      i0 = ((GLint) floor( u - 0.5F)) & (width-1);
      i1 = (i0 + 1) & (width-1);
      i0border = i1border = 0;
   }
   else {
      i0 = (GLint) floor( u - 0.5F );
      i1 = i0 + 1;
      i0border = (i0<0) | (i0>=width);
      i1border = (i1<0) | (i1>=width);
   }

   {
      GLfloat a = frac(u - 0.5F);
      GLint w0 = (GLint) ((1.0F-a) * 256.0F);
      GLint w1 = (GLint) (      a  * 256.0F);

      GLubyte red0, green0, blue0, alpha0;
      GLubyte red1, green1, blue1, alpha1;

      if (i0border) {
         red0   = ctx->Texture.Current1D->BorderColor[0];
         green0 = ctx->Texture.Current1D->BorderColor[1];
         blue0  = ctx->Texture.Current1D->BorderColor[2];
         alpha0 = ctx->Texture.Current1D->BorderColor[3];
      }
      else {
         get_1d_texel( img, i0, &red0, &green0, &blue0, &alpha0 );
      }
      if (i1border) {
         red1   = ctx->Texture.Current1D->BorderColor[0];
         green1 = ctx->Texture.Current1D->BorderColor[1];
         blue1  = ctx->Texture.Current1D->BorderColor[2];
         alpha1 = ctx->Texture.Current1D->BorderColor[3];
      }
      else {
         get_1d_texel( img, i1, &red1, &green1, &blue1, &alpha1 );
      }

      *red   = (w0*red0   + w1*red1)   >> 8;
      *green = (w0*green0 + w1*green1) >> 8;
      *blue  = (w0*blue0  + w1*blue1)  >> 8;
      *alpha = (w0*alpha0 + w1*alpha1) >> 8;
   }
}


static void sample_1d_nearest_mipmap_nearest( GLcontext *ctx,
                                              GLfloat lambda, GLfloat s,
                                              GLubyte *red, GLubyte *green,
                                              GLubyte *blue, GLubyte *alpha )
{
   GLint level;
   if (lambda<=0.5F) {
      level = 0;
   }
   else {
      GLint widthlog2 = ctx->Texture.Current1D->Image[0]->WidthLog2;
      level = (GLint) (lambda + 0.499999F);
      if (level>widthlog2 ) {
         level = widthlog2;
      }
   }
   sample_1d_nearest( ctx, ctx->Texture.Current1D->Image[level],
                      s, red, green, blue, alpha );
}


static void sample_1d_linear_mipmap_nearest( GLcontext *ctx,
                                             GLfloat lambda, GLfloat s,
                                             GLubyte *red, GLubyte *green,
                                             GLubyte *blue, GLubyte *alpha )
{
   GLint level;
   if (lambda<=0.5F) {
      level = 0;
   }
   else {
      GLint widthlog2 = ctx->Texture.Current1D->Image[0]->WidthLog2;
      level = (GLint) (lambda + 0.499999F);
      if (level>widthlog2 ) {
         level = widthlog2;
      }
   }
   sample_1d_linear( ctx, ctx->Texture.Current1D->Image[level],
                     s, red, green, blue, alpha );
}



static void sample_1d_nearest_mipmap_linear( GLcontext *ctx,
                                             GLfloat lambda, GLfloat s,
                                             GLubyte *red, GLubyte *green,
                                             GLubyte *blue, GLubyte *alpha )
{
   GLint max = ctx->Texture.Current1D->Image[0]->MaxLog2;

   if (lambda>=max) {
      sample_1d_nearest( ctx, ctx->Texture.Current1D->Image[max],
                         s, red, green, blue, alpha );
   }
   else {
      GLubyte red0, green0, blue0, alpha0;
      GLubyte red1, green1, blue1, alpha1;
      GLfloat f = frac(lambda);
      GLint level = (GLint) (lambda + 1.0F);
      level = CLAMP( level, 1, max );
      sample_1d_nearest( ctx, ctx->Texture.Current1D->Image[level-1],
                         s, &red0, &green0, &blue0, &alpha0 );
      sample_1d_nearest( ctx, ctx->Texture.Current1D->Image[level],
                         s, &red1, &green1, &blue1, &alpha1 );
      *red   = (1.0F-f)*red1   + f*red0;
      *green = (1.0F-f)*green1 + f*green0;
      *blue  = (1.0F-f)*blue1  + f*blue0;
      *alpha = (1.0F-f)*alpha1 + f*alpha0;
   }
}



static void sample_1d_linear_mipmap_linear( GLcontext *ctx,
                                            GLfloat lambda, GLfloat s,
                                            GLubyte *red, GLubyte *green,
                                            GLubyte *blue, GLubyte *alpha )
{
   GLint max = ctx->Texture.Current1D->Image[0]->MaxLog2;

   if (lambda>=max) {
      sample_1d_linear( ctx, ctx->Texture.Current1D->Image[max],
                        s, red, green, blue, alpha );
   }
   else {
      GLubyte red0, green0, blue0, alpha0;
      GLubyte red1, green1, blue1, alpha1;
      GLfloat f = frac(lambda);
      GLint level = (GLint) (lambda + 1.0F);
      level = CLAMP( level, 1, max );
      sample_1d_linear( ctx, ctx->Texture.Current1D->Image[level-1],
                        s, &red0, &green0, &blue0, &alpha0 );
      sample_1d_linear( ctx, ctx->Texture.Current1D->Image[level],
                        s, &red1, &green1, &blue1, &alpha1 );
      *red   = (1.0F-f)*red1   + f*red0;
      *green = (1.0F-f)*green1 + f*green0;
      *blue  = (1.0F-f)*blue1  + f*blue0;
      *alpha = (1.0F-f)*alpha1 + f*alpha0;
   }
}





/*
 * Given an (s) texture coordinate and lambda (level of detail) value
 * return a texture sample (color).
 *
 */
static void sample_1d_texture( GLcontext *ctx,
                               GLfloat s, GLfloat lambda,
                               GLubyte *red, GLubyte *green, GLubyte *blue,
                               GLubyte *alpha, GLfloat c )
{
   GLint level;

   if (lambda>c) {
      /* minification */
      switch (ctx->Texture.Current1D->MinFilter) {
         case GL_NEAREST:
            level = 0;
            sample_1d_nearest( ctx, ctx->Texture.Current1D->Image[level],
                               s, red, green, blue, alpha );
            break;
         case GL_LINEAR:
            level = 0;
            sample_1d_linear( ctx, ctx->Texture.Current1D->Image[level],
                              s, red, green, blue, alpha );
            break;
         case GL_NEAREST_MIPMAP_NEAREST:
	    sample_1d_nearest_mipmap_nearest( ctx, lambda, s,
                                              red, green, blue, alpha );
            break;
         case GL_LINEAR_MIPMAP_NEAREST:
	    sample_1d_linear_mipmap_nearest( ctx, lambda, s,
                                             red, green, blue, alpha );
            break;
         case GL_NEAREST_MIPMAP_LINEAR:
	    sample_1d_nearest_mipmap_linear( ctx, lambda, s,
                                             red, green, blue, alpha );
            break;
         case GL_LINEAR_MIPMAP_LINEAR:
	    sample_1d_linear_mipmap_linear( ctx, lambda, s,
                                            red, green, blue, alpha );
            break;
         default:
            abort();
      }
   }
   else {
      /* magnification */
      switch (ctx->Texture.Current1D->MagFilter) {
         case GL_NEAREST:
            sample_1d_nearest( ctx, ctx->Texture.Current1D->Image[0],
                               s, red, green, blue, alpha );
            break;
         case GL_LINEAR:
            sample_1d_linear( ctx, ctx->Texture.Current1D->Image[0],
                              s, red, green, blue, alpha );
            break;
         default:
            abort();
      }
   }
}




/**********************************************************************/
/*                    2-D Texture Sampling Functions                  */
/**********************************************************************/


/*
 * Given a texture image and an (i,j) integer texel coordinate, return the
 * texel color.
 */
static void get_2d_texel( struct gl_texture_image *img, GLint i, GLint j,
                          GLubyte *red, GLubyte *green, GLubyte *blue,
                          GLubyte *alpha )
{
   GLint width = img->Width;    /* power of two */
   GLint height = img->Height;  /* power of two */
   GLubyte *texel;

   /* DEBUG */
   if (i<0 || i>=width)  abort();
   if (j<0 || j>=height)  abort();

   switch (img->Format) {
      case GL_ALPHA:
      case GL_LUMINANCE:
      case GL_INTENSITY:
         *red = img->Data[ width * j + i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + (width * j + i) * 2;
         *red   = texel[0];
         *alpha = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + (width * j + i) * 3;
         *red   = texel[0];
         *green = texel[1];
         *blue  = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + (width * j + i) * 4;
         *red   = texel[0];
         *green = texel[1];
         *blue  = texel[2];
         *alpha = texel[3];
         return;
      default:
         abort();
   }
}




/*
 * Return a texture sample for (s,t) using GL_NEAREST filter.
 */
static void sample_2d_nearest( GLcontext *ctx,
                               struct gl_texture_image *img,
                               GLfloat s, GLfloat t,
                               GLubyte *red, GLubyte *green,
                               GLubyte *blue, GLubyte *alpha )
{
   GLint width = img->Width;    /* power of two */
   GLint height = img->Height;  /* power of two */
   GLint i, j;
   GLubyte *texel;

   /* Clamp/Repeat S and convert to integer texel coordinate */
   if (ctx->Texture.Current2D->WrapS==GL_REPEAT) {
      /* s limited to [0,1) */
      /* i limited to [0,width-1] */
      i = (GLint) (s * width);
      i &= (width-1);
   }
   else {
      /* s limited to [0,1] */
      /* i limited to [0,width-1] */
      if (s<0.0F) {
         i = 0;
      }
      else if (s>1.0F) {
         i = width-1;
      }
      else {
         i = (GLint) (s * width);
      }
   }

   /* Clamp/Repeat T and convert to integer texel coordinate */
   if (ctx->Texture.Current2D->WrapT==GL_REPEAT) {
      /* t limited to [0,1) */
      /* j limited to [0,height-1] */
      j = (GLint) (t * height);
      j &= (height-1);
   }
   else {
      /* t limited to [0,1] */
      /* j limited to [0,height-1] */
      if (t<0.0F) {
         j = 0;
      }
      else if (t>1.0F) {
         j = height-1;
      }
      else {
         j = (GLint) (t * height);
      }
   }

   switch (img->Format) {
      case GL_ALPHA:
      case GL_LUMINANCE:
      case GL_INTENSITY:
         *red   = img->Data[ j * width + i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + ((j * width + i) << 1);
         *red   = texel[0];
         *alpha = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + (j * width + i) * 3;
         *red   = texel[0];
         *green = texel[1];
         *blue  = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + ((j * width + i) << 2);
         *red   = texel[0];
         *green = texel[1];
         *blue  = texel[2];
         *alpha = texel[3];
         return;
      default:
         abort();
   }
}



/*
 * Return a texture sample for (s,t) using GL_LINEAR filter.
 */
static void sample_2d_linear( GLcontext *ctx,
                              struct gl_texture_image *img,
                              GLfloat s, GLfloat t,
                              GLubyte *red, GLubyte *green,
                              GLubyte *blue, GLubyte *alpha )
{
   GLint width = img->Width;
   GLint height = img->Height;
   GLint i0, j0, i1, j1;
   GLint i0border, j0border, i1border, j1border;
   GLfloat u, v;

   u = s * width;
   if (ctx->Texture.Current2D->WrapS==GL_REPEAT) {
      i0 = ((GLint) floor( u - 0.5F)) & (width-1);
      i1 = (i0 + 1) & (width-1);
      i0border = i1border = 0;
   }
   else {
      i0 = (GLint) floor( u - 0.5F );
      i1 = i0 + 1;
      i0border = (i0<0) | (i0>=width);
      i1border = (i1<0) | (i1>=width);
   }

   v = t * height;
   if (ctx->Texture.Current2D->WrapT==GL_REPEAT) {
      j0 = ((GLint) floor( v - 0.5F)) & (height-1);
      j1 = (j0 + 1) & (height-1);
      j0border = j1border = 0;
   }
   else {
      j0 = (GLint) floor( v - 0.5F );
      j1 = j0 + 1;
      j0border = (j0<0) | (j0>=height);
      j1border = (j1<0) | (j1>=height);
   }

   {
      GLfloat a = frac( u - 0.5F );
      GLfloat b = frac( v - 0.5F );

      GLint w00 = (GLint) ((1.0F-a)*(1.0F-b) * 256.0F);
      GLint w10 = (GLint) (      a *(1.0F-b) * 256.0F);
      GLint w01 = (GLint) ((1.0F-a)*      b  * 256.0F);
      GLint w11 = (GLint) (      a *      b  * 256.0F);

      GLubyte red00, green00, blue00, alpha00;
      GLubyte red10, green10, blue10, alpha10;
      GLubyte red01, green01, blue01, alpha01;
      GLubyte red11, green11, blue11, alpha11;

      if (i0border | j0border) {
         red00   = ctx->Texture.Current2D->BorderColor[0];
         green00 = ctx->Texture.Current2D->BorderColor[1];
         blue00  = ctx->Texture.Current2D->BorderColor[2];
         alpha00 = ctx->Texture.Current2D->BorderColor[3];
      }
      else {
         get_2d_texel( img, i0, j0, &red00, &green00, &blue00, &alpha00 );
      }
      if (i1border | j0border) {
         red10   = ctx->Texture.Current2D->BorderColor[0];
         green10 = ctx->Texture.Current2D->BorderColor[1];
         blue10  = ctx->Texture.Current2D->BorderColor[2];
         alpha10 = ctx->Texture.Current2D->BorderColor[3];
      }
      else {
         get_2d_texel( img, i1, j0, &red10, &green10, &blue10, &alpha10 );
      }
      if (i0border | j1border) {
         red01   = ctx->Texture.Current2D->BorderColor[0];
         green01 = ctx->Texture.Current2D->BorderColor[1];
         blue01  = ctx->Texture.Current2D->BorderColor[2];
         alpha01 = ctx->Texture.Current2D->BorderColor[3];
      }
      else {
         get_2d_texel( img, i0, j1, &red01, &green01, &blue01, &alpha01 );
      }
      if (i1border | j1border) {
         red11   = ctx->Texture.Current2D->BorderColor[0];
         green11 = ctx->Texture.Current2D->BorderColor[1];
         blue11  = ctx->Texture.Current2D->BorderColor[2];
         alpha11 = ctx->Texture.Current2D->BorderColor[3];
      }
      else {
         get_2d_texel( img, i1, j1, &red11, &green11, &blue11, &alpha11 );
      }

      *red   = (w00*red00   + w10*red10   + w01*red01   + w11*red11  ) >> 8;
      *green = (w00*green00 + w10*green10 + w01*green01 + w11*green11) >> 8;
      *blue  = (w00*blue00  + w10*blue10  + w01*blue01  + w11*blue11 ) >> 8;
      *alpha = (w00*alpha00 + w10*alpha10 + w01*alpha01 + w11*alpha11) >> 8;
   }
}



static void sample_2d_nearest_mipmap_nearest( GLcontext *ctx,
                                              GLfloat lambda,
                                              GLfloat s, GLfloat t,
                                              GLubyte *red, GLubyte *green,
                                              GLubyte *blue, GLubyte *alpha )
{
   GLint level;
   if (lambda<=0.5F) {
      level = 0;
   }
   else {
      GLint widthlog2 = ctx->Texture.Current2D->Image[0]->WidthLog2;
      level = (GLint) (lambda + 0.499999F);
      if (level>widthlog2 ) {
         level = widthlog2;
      }
   }
   sample_2d_nearest( ctx, ctx->Texture.Current2D->Image[level],
                      s, t, red, green, blue, alpha );
}



static void sample_2d_linear_mipmap_nearest( GLcontext *ctx,
                                             GLfloat lambda,
                                             GLfloat s, GLfloat t,
                                             GLubyte *red, GLubyte *green,
                                             GLubyte *blue, GLubyte *alpha )
{
   GLint level;
   if (lambda<=0.5F) {
      level = 0;
   }
   else {
      GLint widthlog2 = ctx->Texture.Current2D->Image[0]->WidthLog2;
      level = (GLint) (lambda + 0.499999F);
      if (level>widthlog2 ) {
         level = widthlog2;
      }
   }
   sample_2d_linear( ctx, ctx->Texture.Current2D->Image[level],
                     s, t, red, green, blue, alpha );
}




static void sample_2d_nearest_mipmap_linear( GLcontext *ctx,
                                             GLfloat lambda,
                                             GLfloat s, GLfloat t,
                                             GLubyte *red, GLubyte *green,
                                             GLubyte *blue, GLubyte *alpha )
{
   GLint max = ctx->Texture.Current2D->Image[0]->MaxLog2;

   if (lambda>=max) {
      sample_2d_nearest( ctx, ctx->Texture.Current2D->Image[max],
                         s, t, red, green, blue, alpha );
   }
   else {
      GLubyte red0, green0, blue0, alpha0;
      GLubyte red1, green1, blue1, alpha1;
      GLfloat f = frac(lambda);
      GLint level = (GLint) (lambda + 1.0F);
      level = CLAMP( level, 1, max );
      sample_2d_nearest( ctx, ctx->Texture.Current2D->Image[level-1], s, t,
                         &red0, &green0, &blue0, &alpha0 );
      sample_2d_nearest( ctx, ctx->Texture.Current2D->Image[level], s, t,
                         &red1, &green1, &blue1, &alpha1 );
      *red   = (1.0F-f)*red1   + f*red0;
      *green = (1.0F-f)*green1 + f*green0;
      *blue  = (1.0F-f)*blue1  + f*blue0;
      *alpha = (1.0F-f)*alpha1 + f*alpha0;
   }
}



static void sample_2d_linear_mipmap_linear( GLcontext *ctx,
                                            GLfloat lambda,
                                            GLfloat s, GLfloat t,
                                            GLubyte *red, GLubyte *green,
                                            GLubyte *blue, GLubyte *alpha )
{
   GLint max = ctx->Texture.Current2D->Image[0]->MaxLog2;

   if (lambda>=max) {
      sample_2d_linear( ctx, ctx->Texture.Current2D->Image[max],
                         s, t, red, green, blue, alpha );
   }
   else {
      GLubyte red0, green0, blue0, alpha0;
      GLubyte red1, green1, blue1, alpha1;
      GLfloat f = frac(lambda);
      GLint level = (GLint) (lambda + 1.0F);
      level = CLAMP( level, 1, max );
      sample_2d_linear( ctx, ctx->Texture.Current2D->Image[level-1], s, t,
                         &red0, &green0, &blue0, &alpha0 );
      sample_2d_linear( ctx, ctx->Texture.Current2D->Image[level], s, t,
                         &red1, &green1, &blue1, &alpha1 );
      *red   = (1.0F-f)*red1   + f*red0;
      *green = (1.0F-f)*green1 + f*green0;
      *blue  = (1.0F-f)*blue1  + f*blue0;
      *alpha = (1.0F-f)*alpha1 + f*alpha0;
   }
}




/*
 * Given an (s,t) texture coordinate and lambda (level of detail) value
 * return a texture sample (color).
 *
 */
static void sample_2d_texture( GLcontext *ctx,
                               GLfloat s, GLfloat t, GLfloat lambda,
                               GLubyte *red, GLubyte *green, GLubyte *blue,
                               GLubyte *alpha, GLfloat c )
{
   if (lambda>c) {
      /* minification */
      switch (ctx->Texture.Current2D->MinFilter) {
         case GL_NEAREST:
            sample_2d_nearest( ctx, ctx->Texture.Current2D->Image[0],
                               s, t, red, green, blue, alpha );
            break;
         case GL_LINEAR:
            sample_2d_linear( ctx, ctx->Texture.Current2D->Image[0],
                              s, t, red, green, blue, alpha );
            break;
         case GL_NEAREST_MIPMAP_NEAREST:
            sample_2d_nearest_mipmap_nearest( ctx, lambda, s, t,
                                              red, green, blue, alpha );
            break;
         case GL_LINEAR_MIPMAP_NEAREST:
            sample_2d_linear_mipmap_nearest( ctx, lambda, s, t,
                                             red, green, blue, alpha );
            break;
         case GL_NEAREST_MIPMAP_LINEAR:
            sample_2d_nearest_mipmap_linear( ctx, lambda, s, t,
                                             red, green, blue, alpha );
            break;
         case GL_LINEAR_MIPMAP_LINEAR:
            sample_2d_linear_mipmap_linear( ctx, lambda, s, t,
                                            red, green, blue, alpha );
            break;
         default:
            abort();
      }
   }
   else {
      /* magnification */
      switch (ctx->Texture.Current2D->MagFilter) {
         case GL_NEAREST:
            sample_2d_nearest( ctx, ctx->Texture.Current2D->Image[0],
                               s, t, red, green, blue, alpha );
            break;
         case GL_LINEAR:
            sample_2d_linear( ctx, ctx->Texture.Current2D->Image[0],
                              s, t, red, green, blue, alpha );
            break;
         default:
            abort();
      }
   }
}

#undef SAMPLE_MIPMAP_LINEAR
#undef SAMPLE_MIPMAP_NEAREST




/**********************************************************************/
/*                      Texture Application                           */
/**********************************************************************/



/*
 * Combine incoming fragment color with texel color to produce output color.
 * Input:  n - number of fragments
 *         format - base internal texture format
 *         env_mode - texture environment mode
 *         Rt, Gt, Bt, At - array of texel colors
 * InOut:  red, green, blue, alpha - incoming fragment colors modified
 *                                   by texel colors according to the
 *                                   texture environment mode.
 */
static void apply_texture( GLcontext *ctx,
         GLuint n, GLint format, GLenum env_mode,
	 GLubyte red[], GLubyte green[], GLubyte blue[], GLubyte alpha[],
	 GLubyte Rt[], GLubyte Gt[], GLubyte Bt[], GLubyte At[] )
{
   GLuint i;
   GLint Rc, Gc, Bc, Ac;

   if (!ctx->Visual->EightBitColor) {
      /* This is a hack!  Rescale input colors from [0,scale] to [0,255]. */
      GLfloat rscale = 255.0 * ctx->Visual->InvRedScale;
      GLfloat gscale = 255.0 * ctx->Visual->InvGreenScale;
      GLfloat bscale = 255.0 * ctx->Visual->InvBlueScale;
      GLfloat ascale = 255.0 * ctx->Visual->InvAlphaScale;
      for (i=0;i<n;i++) {
	 red[i]   = (GLint) (red[i]   * rscale);
	 green[i] = (GLint) (green[i] * gscale);
	 blue[i]  = (GLint) (blue[i]  * bscale);
	 alpha[i] = (GLint) (alpha[i] * ascale);
      }
   }

/*
 * Use (A*(B+1)) >> 8 as a fast approximation of (A*B)/255 for A
 * and B in [0,255]
 */
#define PROD(A,B)   (((GLint)(A) * (GLint)(B)+1) >> 8)

   switch (env_mode) {
      case GL_REPLACE:
	 switch (format) {
	    case GL_ALPHA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf */
                  /* Av = At */
                  alpha[i] = At[i];
	       }
	       break;
	    case GL_LUMINANCE:
	       for (i=0;i<n;i++) {
		  /* Cv = Lt */
                  GLint Lt = Rt[i];
                  red[i] = green[i] = blue[i] = Lt;
                  /* Av = Af */
	       }
	       break;
	    case GL_LUMINANCE_ALPHA:
	       for (i=0;i<n;i++) {
                  GLint Lt = Rt[i];
		  /* Cv = Lt */
		  red[i] = green[i] = blue[i] = Lt;
		  /* Av = At */
		  alpha[i] = At[i];
	       }
	       break;
	    case GL_INTENSITY:
	       for (i=0;i<n;i++) {
		  /* Cv = It */
                  GLint It = Rt[i];
                  red[i] = green[i] = blue[i] = It;
                  /* Av = It */
                  alpha[i] = It;
	       }
	       break;
	    case GL_RGB:
	       for (i=0;i<n;i++) {
		  /* Cv = Ct */
		  red[i]   = Rt[i];
		  green[i] = Gt[i];
		  blue[i]  = Bt[i];
		  /* Av = Af */
	       }
	       break;
	    case GL_RGBA:
	       for (i=0;i<n;i++) {
		  /* Cv = Ct */
		  red[i]   = Rt[i];
		  green[i] = Gt[i];
		  blue[i]  = Bt[i];
		  /* Av = At */
		  alpha[i] = At[i];
	       }
	       break;
            default:
               abort();
	 }
	 break;

      case GL_MODULATE:
         switch (format) {
	    case GL_ALPHA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf */
		  /* Av = AfAt */
		  alpha[i] = PROD( alpha[i], At[i] );
	       }
	       break;
	    case GL_LUMINANCE:
	       for (i=0;i<n;i++) {
		  /* Cv = LtCf */
                  GLint Lt = Rt[i];
		  red[i]   = PROD( red[i],   Lt );
		  green[i] = PROD( green[i], Lt );
		  blue[i]  = PROD( blue[i],  Lt );
		  /* Av = Af */
	       }
	       break;
	    case GL_LUMINANCE_ALPHA:
	       for (i=0;i<n;i++) {
		  /* Cv = CfLt */
                  GLint Lt = Rt[i];
		  red[i]   = PROD( red[i],   Lt );
		  green[i] = PROD( green[i], Lt );
		  blue[i]  = PROD( blue[i],  Lt );
		  /* Av = AfAt */
		  alpha[i] = PROD( alpha[i], At[i] );
	       }
	       break;
	    case GL_INTENSITY:
	       for (i=0;i<n;i++) {
		  /* Cv = CfIt */
                  GLint It = Rt[i];
		  red[i]   = PROD( red[i],   It );
		  green[i] = PROD( green[i], It );
		  blue[i]  = PROD( blue[i],  It );
		  /* Av = AfIt */
		  alpha[i] = PROD( alpha[i], It );
	       }
	       break;
	    case GL_RGB:
	       for (i=0;i<n;i++) {
		  /* Cv = CfCt */
		  red[i]   = PROD( red[i],   Rt[i] );
		  green[i] = PROD( green[i], Gt[i] );
		  blue[i]  = PROD( blue[i],  Bt[i] );
		  /* Av = Af */
	       }
	       break;
	    case GL_RGBA:
	       for (i=0;i<n;i++) {
		  /* Cv = CfCt */
		  red[i]   = PROD( red[i],   Rt[i] );
		  green[i] = PROD( green[i], Gt[i] );
		  blue[i]  = PROD( blue[i],  Bt[i] );
		  /* Av = AfAt */
		  alpha[i] = PROD( alpha[i], At[i] );
	       }
	       break;
            default:
               abort();
	 }
	 break;

      case GL_DECAL:
         switch (format) {
            case GL_ALPHA:
            case GL_LUMINANCE:
            case GL_LUMINANCE_ALPHA:
            case GL_INTENSITY:
               /* undefined */
               break;
	    case GL_RGB:
	       for (i=0;i<n;i++) {
		  /* Cv = Ct */
		  red[i]   = Rt[i];
		  green[i] = Gt[i];
		  blue[i]  = Bt[i];
		  /* Av = Af */
	       }
	       break;
	    case GL_RGBA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-At) + CtAt */
		  GLint t = At[i], s = 255 - t;
		  red[i]   = PROD(red[i],  s) + PROD(Rt[i],t);
		  green[i] = PROD(green[i],s) + PROD(Gt[i],t);
		  blue[i]  = PROD(blue[i], s) + PROD(Bt[i],t);
		  /* Av = Af */
	       }
	       break;
            default:
               abort();
	 }
	 break;

      case GL_BLEND:
         Rc = (GLint) (ctx->Texture.EnvColor[0] * 255.0F);
         Gc = (GLint) (ctx->Texture.EnvColor[1] * 255.0F);
         Bc = (GLint) (ctx->Texture.EnvColor[2] * 255.0F);
         Ac = (GLint) (ctx->Texture.EnvColor[2] * 255.0F);
	 switch (format) {
	    case GL_ALPHA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf */
		  /* Av = AfAt */
                  alpha[i] = PROD(alpha[i], At[i]);
	       }
	       break;
            case GL_LUMINANCE:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-Lt) + CcLt */
		  GLint Lt = Rt[i], s = 255 - Lt;
		  red[i]   = PROD(red[i],  s) + PROD(Rc,  Lt);
		  green[i] = PROD(green[i],s) + PROD(Gc,Lt);
		  blue[i]  = PROD(blue[i], s) + PROD(Bc, Lt);
		  /* Av = Af */
	       }
	       break;
	    case GL_LUMINANCE_ALPHA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-Lt) + CcLt */
		  GLint Lt = Rt[i], s = 255 - Lt;
		  red[i]   = PROD(red[i],  s) + PROD(Rc,  Lt);
		  green[i] = PROD(green[i],s) + PROD(Gc,Lt);
		  blue[i]  = PROD(blue[i], s) + PROD(Bc, Lt);
		  /* Av = AfAt */
		  alpha[i] = PROD(alpha[i],At[i]);
	       }
	       break;
            case GL_INTENSITY:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-It) + CcLt */
		  GLint It = Rt[i], s = 255 - It;
		  red[i]   = PROD(red[i],  s) + PROD(Rc,It);
		  green[i] = PROD(green[i],s) + PROD(Gc,It);
		  blue[i]  = PROD(blue[i], s) + PROD(Bc,It);
                  /* Av = Af(1-It) + Ac*It */
                  alpha[i] = PROD(alpha[i],s) + PROD(Ac,It);
               }
               break;
	    case GL_RGB:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-Ct) + CcCt */
		  red[i]   = PROD(red[i],  (255-Rt[i])) + PROD(Rc,Rt[i]);
		  green[i] = PROD(green[i],(255-Gt[i])) + PROD(Gc,Gt[i]);
		  blue[i]  = PROD(blue[i], (255-Bt[i])) + PROD(Bc,Bt[i]);
		  /* Av = Af */
	       }
	       break;
	    case GL_RGBA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-Ct) + CcCt */
		  red[i]   = PROD(red[i],  (255-Rt[i])) + PROD(Rc,Rt[i]);
		  green[i] = PROD(green[i],(255-Gt[i])) + PROD(Gc,Gt[i]);
		  blue[i]  = PROD(blue[i], (255-Bt[i])) + PROD(Bc,Bt[i]);
		  /* Av = AfAt */
		  alpha[i] = PROD(alpha[i],At[i]);
	       }
	       break;
	 }
	 break;

      default:
         abort();
   }
#undef PROD

   if (!ctx->Visual->EightBitColor) {
      /* This is a hack!  Rescale input colors from [0,255] to [0,scale]. */
      GLfloat rscale = ctx->Visual->RedScale   * (1.0F/ 255.0F);
      GLfloat gscale = ctx->Visual->GreenScale * (1.0F/ 255.0F);
      GLfloat bscale = ctx->Visual->BlueScale  * (1.0F/ 255.0F);
      GLfloat ascale = ctx->Visual->AlphaScale * (1.0F/ 255.0F);
      for (i=0;i<n;i++) {
	 red[i]   = (GLint) (red[i]   * rscale);
	 green[i] = (GLint) (green[i] * gscale);
	 blue[i]  = (GLint) (blue[i]  * bscale);
	 alpha[i] = (GLint) (alpha[i] * ascale);
      }
   }
}



/*
 * Given an array of fragment colors and texture coordinates, apply
 * 1-D texturing to the fragments.
 * Input:  n - number of fragments
 *         s - array of texture coordinate s values
 *         lambda - array of lambda values
 * InOut:  red, green, blue, alpha - incoming and modifed fragment colors
 */
void gl_texture_pixels_1d( GLcontext *ctx,
                           GLuint n, GLfloat s[], GLfloat lambda[],
			   GLubyte red[], GLubyte green[],
			   GLubyte blue[], GLubyte alpha[] )
{
   GLubyte tred[PB_SIZE];
   GLubyte tgreen[PB_SIZE];
   GLubyte tblue[PB_SIZE];
   GLubyte talpha[PB_SIZE];
   GLuint  i;
   GLfloat c;

   /* Decide if texture can be applied. */
   if (!ctx->Texture.Current1D->Complete) {
      return;
   }

   /*
    * Decide about value of c
    */
   if (ctx->Texture.Current1D->MagFilter==GL_LINEAR
       && (ctx->Texture.Current1D->MinFilter==GL_NEAREST_MIPMAP_NEAREST ||
           ctx->Texture.Current1D->MinFilter==GL_LINEAR_MIPMAP_NEAREST)) {
      c = 0.5F;
   }
   else {
      c = 0.0F;
   }

   /*
    * Compute texel colors.
    */
   if (lambda) {
      for (i=0;i<n;i++)
	 sample_1d_texture( ctx, s[i],lambda[i],
                            &tred[i],&tgreen[i],&tblue[i],&talpha[i],c);
   } 
   else {
      for (i=0;i<n;i++)
	 sample_1d_texture( ctx, s[i],0,
                            &tred[i],&tgreen[i],&tblue[i],&talpha[i],c);
   }

   /* Modify incoming fragment colors according to sampled texels */
   apply_texture( ctx, n,
                  ctx->Texture.Current1D->Image[0]->Format,
                  ctx->Texture.EnvMode,
		  red, green, blue, alpha,
                  tred, tgreen, tblue, talpha );
}



/*
 * Given an array of fragment colors and texture coordinates, apply
 * 2-D texturing to the fragments.
 * Input:  n - number of fragments
 *         s,s - array of texture coordinate (s,t) values
 *         lambda - array of lambda values
 * InOut:  red, green, blue, alpha - incoming and modifed fragment colors
 */
void gl_texture_pixels_2d( GLcontext *ctx,
                           GLuint n,
			   GLfloat s[], GLfloat t[], GLfloat lambda[],
			   GLubyte red[], GLubyte green[],
			   GLubyte blue[], GLubyte alpha[] )
{
   GLubyte tred[PB_SIZE];
   GLubyte tgreen[PB_SIZE];
   GLubyte tblue[PB_SIZE];
   GLubyte talpha[PB_SIZE];
   GLuint i;
   GLfloat c;

   /* Decide if texture can be applied. */
   if (!ctx->Texture.Current2D->Complete) {
      return;
   }

   /*
    * Decide about value of c
    */
   if (ctx->Texture.Current2D->MagFilter==GL_LINEAR
       && (ctx->Texture.Current2D->MinFilter==GL_NEAREST_MIPMAP_NEAREST ||
           ctx->Texture.Current2D->MinFilter==GL_LINEAR_MIPMAP_NEAREST)) {
      c = 0.5F;
   }
   else {
      c = 0.0F;
   }

   /*
    * Compute texel colors.
    */
   if (lambda) {
      for (i=0;i<n;i++) {
	 sample_2d_texture( ctx, s[i], t[i], lambda[i],
			    &tred[i], &tgreen[i], &tblue[i], &talpha[i], c);
      }
   }
   else {
      for (i=0;i<n;i++)
	 sample_2d_texture( ctx, s[i], t[i], 0,
			    &tred[i], &tgreen[i], &tblue[i], &talpha[i], c );
   }

   apply_texture( ctx, n,
                  ctx->Texture.Current2D->Image[0]->Format,
                  ctx->Texture.EnvMode,
		  red, green, blue, alpha,
                  tred, tgreen, tblue, talpha );
}




/*
 * This is called by gl_update_state() if the NEW_TEXTURING bit in
 * ctx->NewState is set.  Basically, we check if we have a complete set
 * of mipmaps when mipmapping is enabled.
 */
void gl_update_texture_state( GLcontext *ctx )
{
   GLint i;
   struct gl_texture_object *t;

   t = ctx->Shared->TexObjectList;
   while (t) {

      /*
       * Determine if we have a complete set of mipmaps
       */
      t->Complete = GL_TRUE;  /* be optimistic */
      if (   t->MinFilter==GL_NEAREST_MIPMAP_NEAREST
          || t->MinFilter==GL_LINEAR_MIPMAP_NEAREST
          || t->MinFilter==GL_NEAREST_MIPMAP_LINEAR
          || t->MinFilter==GL_LINEAR_MIPMAP_LINEAR) {

         /* Test dimension-independent attributes */
         for (i=1; i<MAX_TEXTURE_LEVELS; i++) {
            if (t->Image[i]) {
               if (!t->Image[i]->Data) {
                  t->Complete = GL_FALSE;
                  break;
               }
               if (t->Image[i]->Format != t->Image[0]->Format) {
                  t->Complete = GL_FALSE;
                  break;
               }
               if (t->Image[i]->Border != t->Image[0]->Border) {
                  t->Complete = GL_FALSE;
                  break;
               }
            }
         }

         if (t->Dimensions==1 && t->Image[0]) {
            /* Test 1-D mipmaps */
            GLuint width = t->Image[0]->Width;
            for (i=1; i<MAX_TEXTURE_LEVELS; i++) {
               if (width>1) {
                  width /= 2;
               }
               if (!t->Image[i]) {
                  t->Complete = GL_FALSE;
                  break;
               }
               if (!t->Image[i]->Data) {
                  t->Complete = GL_FALSE;
                  break;
               }
               if (t->Image[i]->Format != t->Image[0]->Format) {
                  t->Complete = GL_FALSE;
                  break;
               }
               if (t->Image[i]->Border != t->Image[0]->Border) {
                  t->Complete = GL_FALSE;
                  break;
               }
               if (t->Image[i]->Width != width ) {
                  t->Complete = GL_FALSE;
                  break;
               }
               if (width==1) {
                  break;
               }
            }
         }
         else if (t->Dimensions==2 && t->Image[0]) {
            /* Test 2-D mipmaps */
            GLuint width = t->Image[0]->Width;
            GLuint height = t->Image[0]->Height;
            for (i=1; i<MAX_TEXTURE_LEVELS; i++) {
               if (width>1) {
                  width /= 2;
               }
               if (height>1) {
                  height /= 2;
               }
               if (!t->Image[i]) {
                  t->Complete = GL_FALSE;
                  break;
               }
               if (t->Image[i]->Width != width) {
                  t->Complete = GL_FALSE;
                  break;
               }
               if (t->Image[i]->Height != height) {
                  t->Complete = GL_FALSE;
                  break;
               }
               if (width==1 && height==1) {
                  break;
               }
            }
         }
         else {
            /* Dimensions = ??? */
         }
      }
      else {
         /* not mipmapping, only need the level 0 texture image */
         if (!t->Image[0] || !t->Image[0]->Data) {
            t->Complete = GL_FALSE;
         }
      }

      t = t->Next;  /* Next texture object */
   }
}



