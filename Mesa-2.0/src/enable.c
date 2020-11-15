/* $Id: enable.c,v 1.3 1996/09/27 01:26:40 brianp Exp $ */

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
 * $Log: enable.c,v $
 * Revision 1.3  1996/09/27 01:26:40  brianp
 * removed unused variables
 *
 * Revision 1.2  1996/09/15 14:17:30  brianp
 * now use GLframebuffer and GLvisual
 *
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


#include <string.h>
#include "context.h"
#include "depth.h"
#include "draw.h"
#include "enable.h"
#include "light.h"
#include "dlist.h"
#include "macros.h"
#include "stencil.h"
#include "types.h"




/*
 * Perform glEnable and glDisable calls.
 */
static void gl_enable( GLcontext* ctx, GLenum cap, GLboolean state )
{
   GLuint p;

   if (INSIDE_BEGIN_END(ctx)) {
      if (state) {
	 gl_error( ctx, GL_INVALID_OPERATION, "glEnable" );
      }
      else {
	 gl_error( ctx, GL_INVALID_OPERATION, "glDisable" );
      }
      return;
   }

   switch (cap) {
      case GL_ALPHA_TEST:
         if (ctx->Color.AlphaEnabled!=state) {
            ctx->Color.AlphaEnabled = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_AUTO_NORMAL:
	 ctx->Eval.AutoNormal = state;
	 break;
      case GL_BLEND:
         if (ctx->Color.BlendEnabled!=state) {
            ctx->Color.BlendEnabled = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_CLIP_PLANE0:
      case GL_CLIP_PLANE1:
      case GL_CLIP_PLANE2:
      case GL_CLIP_PLANE3:
      case GL_CLIP_PLANE4:
      case GL_CLIP_PLANE5:
	 ctx->Transform.ClipEnabled[cap-GL_CLIP_PLANE0] = state;
	 /* Check if any clip planes enabled */
         ctx->Transform.AnyClip = GL_FALSE;
         for (p=0;p<MAX_CLIP_PLANES;p++) {
            if (ctx->Transform.ClipEnabled[p]) {
               ctx->Transform.AnyClip = GL_TRUE;
               break;
            }
         }
	 break;
      case GL_COLOR_MATERIAL:
         if (ctx->Light.ColorMaterialEnabled!=state) {
            ctx->Light.ColorMaterialEnabled = state;
            if (state) {
               GLfloat color[4];
               color[0] = ctx->Current.IntColor[0] * ctx->Visual->InvRedScale;
               color[1] = ctx->Current.IntColor[1] * ctx->Visual->InvGreenScale;
               color[2] = ctx->Current.IntColor[2] * ctx->Visual->InvBlueScale;
               color[3] = ctx->Current.IntColor[3] * ctx->Visual->InvAlphaScale;
               /* update material with current color */
               gl_Materialfv( ctx, ctx->Light.ColorMaterialFace,
                            ctx->Light.ColorMaterialMode, color );
            }
            ctx->NewState |= NEW_LIGHTING;
         }
	 break;
      case GL_CULL_FACE:
         if (ctx->Polygon.CullFlag!=state) {
            ctx->Polygon.CullFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_DEPTH_TEST:
	 if (ctx->Depth.Test!=state) {
            ctx->Depth.Test = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 if (state && ctx->Visual->DepthBits>0 && !ctx->Buffer->Depth) {
	    /* need to allocate a depth buffer now */
	    (*ctx->Driver.AllocDepthBuffer)(ctx);
	 }
         break;
      case GL_DITHER:
	 ctx->Color.DitherFlag = state;
         if (ctx->Driver.Dither) {
            (*ctx->Driver.Dither)( ctx, state );
         }
         ctx->NewState |= NEW_RASTER_OPS;
	 break;
      case GL_FOG:
	 if (ctx->Fog.Enabled!=state) {
            ctx->Fog.Enabled = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_LIGHT0:
      case GL_LIGHT1:
      case GL_LIGHT2:
      case GL_LIGHT3:
      case GL_LIGHT4:
      case GL_LIGHT5:
      case GL_LIGHT6:
      case GL_LIGHT7:
         ctx->Light.Light[cap-GL_LIGHT0].Enabled = state;
         ctx->NewState |= NEW_LIGHTING;
         break;
      case GL_LIGHTING:
         if (ctx->Light.Enabled!=state) {
            ctx->Light.Enabled = state;
            ctx->NewState |= NEW_LIGHTING;
         }
         break;
      case GL_LINE_SMOOTH:
	 if (ctx->Line.SmoothFlag!=state) {
            ctx->Line.SmoothFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_LINE_STIPPLE:
	 if (ctx->Line.StippleFlag!=state) {
            ctx->Line.StippleFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_LOGIC_OP:
	 if (ctx->Color.LogicOpEnabled!=state) {
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 ctx->Color.LogicOpEnabled = state;
	 break;
      case GL_MAP1_COLOR_4:
	 ctx->Eval.Map1Color4 = state;
	 break;
      case GL_MAP1_INDEX:
	 ctx->Eval.Map1Index = state;
	 break;
      case GL_MAP1_NORMAL:
	 ctx->Eval.Map1Normal = state;
	 break;
      case GL_MAP1_TEXTURE_COORD_1:
	 ctx->Eval.Map1TextureCoord1 = state;
	 break;
      case GL_MAP1_TEXTURE_COORD_2:
	 ctx->Eval.Map1TextureCoord2 = state;
	 break;
      case GL_MAP1_TEXTURE_COORD_3:
	 ctx->Eval.Map1TextureCoord3 = state;
	 break;
      case GL_MAP1_TEXTURE_COORD_4:
	 ctx->Eval.Map1TextureCoord4 = state;
	 break;
      case GL_MAP1_VERTEX_3:
	 ctx->Eval.Map1Vertex3 = state;
	 break;
      case GL_MAP1_VERTEX_4:
	 ctx->Eval.Map1Vertex4 = state;
	 break;
      case GL_MAP2_COLOR_4:
	 ctx->Eval.Map2Color4 = state;
	 break;
      case GL_MAP2_INDEX:
	 ctx->Eval.Map2Index = state;
	 break;
      case GL_MAP2_NORMAL:
	 ctx->Eval.Map2Normal = state;
	 break;
      case GL_MAP2_TEXTURE_COORD_1: 
	 ctx->Eval.Map2TextureCoord1 = state;
	 break;
      case GL_MAP2_TEXTURE_COORD_2:
	 ctx->Eval.Map2TextureCoord2 = state;
	 break;
      case GL_MAP2_TEXTURE_COORD_3:
	 ctx->Eval.Map2TextureCoord3 = state;
	 break;
      case GL_MAP2_TEXTURE_COORD_4:
	 ctx->Eval.Map2TextureCoord4 = state;
	 break;
      case GL_MAP2_VERTEX_3:
	 ctx->Eval.Map2Vertex3 = state;
	 break;
      case GL_MAP2_VERTEX_4:
	 ctx->Eval.Map2Vertex4 = state;
	 break;
      case GL_NORMALIZE:
	 ctx->Transform.Normalize = state;
	 break;
      case GL_POINT_SMOOTH:
	 if (ctx->Point.SmoothFlag!=state) {
            ctx->Point.SmoothFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_POLYGON_SMOOTH:
	 if (ctx->Polygon.SmoothFlag!=state) {
            ctx->Polygon.SmoothFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_POLYGON_STIPPLE:
	 if (ctx->Polygon.StippleFlag!=state) {
            ctx->Polygon.StippleFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_POLYGON_OFFSET_POINT:
         if (ctx->Polygon.OffsetPoint!=state) {
            ctx->Polygon.OffsetPoint = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
         break;
      case GL_POLYGON_OFFSET_LINE:
         if (ctx->Polygon.OffsetLine!=state) {
            ctx->Polygon.OffsetLine = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
         break;
      case GL_POLYGON_OFFSET_FILL:
         if (ctx->Polygon.OffsetFill!=state) {
            ctx->Polygon.OffsetFill = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
         break;
      case GL_SCISSOR_TEST:
         if (ctx->Scissor.Enabled!=state) {
            ctx->Scissor.Enabled = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_STENCIL_TEST:
	 if (ctx->Stencil.Enabled!=state) {
            ctx->Stencil.Enabled = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 if (state && ctx->Visual->StencilBits>0 && !ctx->Buffer->Stencil) {
	    /* need to allocate a stencil buffer now */
	    gl_alloc_stencil_buffer(ctx);
	 }
	 break;
      case GL_TEXTURE_1D:
         if (ctx->Visual->RGBAflag) {
            /* texturing only works in RGB mode */
            if (state) {
               ctx->Texture.Enabled |= 1;
            }
            else {
               ctx->Texture.Enabled &= 2;
            }
            ctx->NewState |= (NEW_RASTER_OPS | NEW_TEXTURING);
         }
	 break;
      case GL_TEXTURE_2D:
         if (ctx->Visual->RGBAflag) {
            /* texturing only works in RGB mode */
            if (state) {
               ctx->Texture.Enabled |= 2;
            }
            else {
               ctx->Texture.Enabled &= 1;
            }
            ctx->NewState |= (NEW_RASTER_OPS | NEW_TEXTURING);
         }
	 break;
      case GL_TEXTURE_GEN_Q:
         if (state) {
            ctx->Texture.TexGenEnabled |= Q_BIT;
         }
         else {
            ctx->Texture.TexGenEnabled &= ~Q_BIT;
         }
         ctx->NewState |= NEW_TEXTURING;
	 break;
      case GL_TEXTURE_GEN_R:
         if (state) {
            ctx->Texture.TexGenEnabled |= R_BIT;
         }
         else {
            ctx->Texture.TexGenEnabled &= ~R_BIT;
         }
         ctx->NewState |= NEW_TEXTURING;
	 break;
      case GL_TEXTURE_GEN_S:
	 if (state) {
            ctx->Texture.TexGenEnabled |= S_BIT;
         }
         else {
            ctx->Texture.TexGenEnabled &= ~S_BIT;
         }
         ctx->NewState |= NEW_TEXTURING;
	 break;
      case GL_TEXTURE_GEN_T:
         if (state) {
            ctx->Texture.TexGenEnabled |= T_BIT;
         }
         else {
            ctx->Texture.TexGenEnabled &= ~T_BIT;
         }
         ctx->NewState |= NEW_TEXTURING;
	 break;
      default:
	 if (state) {
	    gl_error( ctx, GL_INVALID_ENUM, "glEnable" );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glDisable" );
	 }
         break;
   }
}




void gl_Enable( GLcontext* ctx, GLenum cap )
{
   gl_enable( ctx, cap, GL_TRUE );
}



void gl_Disable( GLcontext* ctx, GLenum cap )
{
   gl_enable( ctx, cap, GL_FALSE );
}



GLboolean gl_IsEnabled( GLcontext* ctx, GLenum cap )
{
   switch (cap) {
      case GL_ALPHA_TEST:
         return ctx->Color.AlphaEnabled;
      case GL_AUTO_NORMAL:
	 return ctx->Eval.AutoNormal;
      case GL_BLEND:
         return ctx->Color.BlendEnabled;
      case GL_CLIP_PLANE0:
      case GL_CLIP_PLANE1:
      case GL_CLIP_PLANE2:
      case GL_CLIP_PLANE3:
      case GL_CLIP_PLANE4:
      case GL_CLIP_PLANE5:
	 return ctx->Transform.ClipEnabled[cap-GL_CLIP_PLANE0];
      case GL_COLOR_MATERIAL:
	 return ctx->Light.ColorMaterialEnabled;
      case GL_CULL_FACE:
         return ctx->Polygon.CullFlag;
      case GL_DEPTH_TEST:
         return ctx->Depth.Test;
      case GL_DITHER:
	 return ctx->Color.DitherFlag;
      case GL_FOG:
	 return ctx->Fog.Enabled;
      case GL_LIGHTING:
         return ctx->Light.Enabled;
      case GL_LIGHT0:
      case GL_LIGHT1:
      case GL_LIGHT2:
      case GL_LIGHT3:
      case GL_LIGHT4:
      case GL_LIGHT5:
      case GL_LIGHT6:
      case GL_LIGHT7:
         return ctx->Light.Light[cap-GL_LIGHT0].Enabled;
      case GL_LINE_SMOOTH:
	 return ctx->Line.SmoothFlag;
      case GL_LINE_STIPPLE:
	 return ctx->Line.StippleFlag;
      case GL_LOGIC_OP:
	 return ctx->Color.LogicOpEnabled;
      case GL_MAP1_COLOR_4:
	 return ctx->Eval.Map1Color4;
      case GL_MAP1_INDEX:
	 return ctx->Eval.Map1Index;
      case GL_MAP1_NORMAL:
	 return ctx->Eval.Map1Normal;
      case GL_MAP1_TEXTURE_COORD_1:
	 return ctx->Eval.Map1TextureCoord1;
      case GL_MAP1_TEXTURE_COORD_2:
	 return ctx->Eval.Map1TextureCoord2;
      case GL_MAP1_TEXTURE_COORD_3:
	 return ctx->Eval.Map1TextureCoord3;
      case GL_MAP1_TEXTURE_COORD_4:
	 return ctx->Eval.Map1TextureCoord4;
      case GL_MAP1_VERTEX_3:
	 return ctx->Eval.Map1Vertex3;
      case GL_MAP1_VERTEX_4:
	 return ctx->Eval.Map1Vertex4;
      case GL_MAP2_COLOR_4:
	 return ctx->Eval.Map2Color4;
      case GL_MAP2_INDEX:
	 return ctx->Eval.Map2Index;
      case GL_MAP2_NORMAL:
	 return ctx->Eval.Map2Normal;
      case GL_MAP2_TEXTURE_COORD_1: 
	 return ctx->Eval.Map2TextureCoord1;
      case GL_MAP2_TEXTURE_COORD_2:
	 return ctx->Eval.Map2TextureCoord2;
      case GL_MAP2_TEXTURE_COORD_3:
	 return ctx->Eval.Map2TextureCoord3;
      case GL_MAP2_TEXTURE_COORD_4:
	 return ctx->Eval.Map2TextureCoord4;
      case GL_MAP2_VERTEX_3:
	 return ctx->Eval.Map2Vertex3;
      case GL_MAP2_VERTEX_4:
	 return ctx->Eval.Map2Vertex4;
      case GL_NORMALIZE:
	 return ctx->Transform.Normalize;
      case GL_POINT_SMOOTH:
	 return ctx->Point.SmoothFlag;
      case GL_POLYGON_SMOOTH:
	 return ctx->Polygon.SmoothFlag;
      case GL_POLYGON_STIPPLE:
	 return ctx->Polygon.StippleFlag;
      case GL_POLYGON_OFFSET_POINT:
	 return ctx->Polygon.OffsetPoint;
      case GL_POLYGON_OFFSET_LINE:
	 return ctx->Polygon.OffsetLine;
      case GL_POLYGON_OFFSET_FILL:
	 return ctx->Polygon.OffsetFill;
      case GL_SCISSOR_TEST:
	 return ctx->Scissor.Enabled;
      case GL_STENCIL_TEST:
	 return ctx->Stencil.Enabled;
      case GL_TEXTURE_1D:
	 return (ctx->Texture.Enabled & 1) ? GL_TRUE : GL_FALSE;
      case GL_TEXTURE_2D:
	 return (ctx->Texture.Enabled & 2) ? GL_TRUE : GL_FALSE;
      case GL_TEXTURE_GEN_Q:
	 return (ctx->Texture.TexGenEnabled & Q_BIT) ? GL_TRUE : GL_FALSE;
      case GL_TEXTURE_GEN_R:
	 return (ctx->Texture.TexGenEnabled & R_BIT) ? GL_TRUE : GL_FALSE;
      case GL_TEXTURE_GEN_S:
	 return (ctx->Texture.TexGenEnabled & S_BIT) ? GL_TRUE : GL_FALSE;
      case GL_TEXTURE_GEN_T:
	 return (ctx->Texture.TexGenEnabled & T_BIT) ? GL_TRUE : GL_FALSE;

      /*
       * CLIENT STATE!!!
       */
      case GL_VERTEX_ARRAY:
         return ctx->Array.VertexEnabled;
      case GL_NORMAL_ARRAY:
         return ctx->Array.NormalEnabled;
      case GL_COLOR_ARRAY:
         return ctx->Array.ColorEnabled;
      case GL_INDEX_ARRAY:
         return ctx->Array.IndexEnabled;
      case GL_TEXTURE_COORD_ARRAY:
         return ctx->Array.TexCoordEnabled;
      case GL_EDGE_FLAG_ARRAY:
         return ctx->Array.EdgeFlagEnabled;
      default:
	 gl_error( ctx, GL_INVALID_ENUM, "glIsEnabled" );
	 return GL_FALSE;
   }
}




void gl_client_state( GLcontext *ctx, GLenum cap, GLboolean state )
{
   switch (cap) {
      case GL_VERTEX_ARRAY:
         ctx->Array.VertexEnabled = state;
         break;
      case GL_NORMAL_ARRAY:
         ctx->Array.NormalEnabled = state;
         break;
      case GL_COLOR_ARRAY:
         ctx->Array.ColorEnabled = state;
         break;
      case GL_INDEX_ARRAY:
         ctx->Array.IndexEnabled = state;
         break;
      case GL_TEXTURE_COORD_ARRAY:
         ctx->Array.TexCoordEnabled = state;
         break;
      case GL_EDGE_FLAG_ARRAY:
         ctx->Array.EdgeFlagEnabled = state;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glEnable/DisableClientState" );
   }
}



void gl_EnableClientState( GLcontext *ctx, GLenum cap )
{
   gl_client_state( ctx, cap, GL_TRUE );
}



void gl_DisableClientState( GLcontext *ctx, GLenum cap )
{
   gl_client_state( ctx, cap, GL_FALSE );
}

