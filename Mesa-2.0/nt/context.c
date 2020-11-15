/* $Id: context.c,v 1.49 1996/02/26 15:14:32 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  1.2
 * Copyright (C) 1995-1996  Brian Paul  (brianp@ssec.wisc.edu)
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
$Log: context.c,v $
 * Revision 1.49  1996/02/26  15:14:32  brianp
 * removed CC.Current.Color, replaced by CC.Current.IntColor
 *
 * Revision 1.48  1996/02/26  15:13:24  brianp
 * added lookup tables and code for optimized lighting
 *
 * Revision 1.47  1996/02/19  21:49:26  brianp
 * added support for software alpha buffering
 *
 * Revision 1.46  1996/02/13  17:46:49  brianp
 * call gl_update_lighting() in gl_udpate_state()
 *
 * Revision 1.45  1996/02/06  04:13:02  brianp
 * added CC.Polygon.CullBits code
 *
 * Revision 1.44  1996/02/06  03:24:36  brianp
 * removed gamma correction code
 *
 * Revision 1.43  1996/01/29  19:04:54  brianp
 * test for CC.Polygon.Unfilled for CC.ComputePlane
 *
 * Revision 1.42  1996/01/23  16:54:29  brianp
 * CC.FastDrawPixels wasn't initialized correctly, added more comments
 *
 * Revision 1.41  1996/01/22  15:36:14  brianp
 * new logic in gl_update_state() for CC.MutablePixels and CC.MonoPixels
 *
 * Revision 1.40  1996/01/12  22:31:10  brianp
 * changed default stencil masks to 0xff (1 byte)
 *
 * Revision 1.39  1996/01/11  15:46:56  brianp
 * free stencil buffer, if any, in gl_destroy_context()
 *
 * Revision 1.38  1996/01/09  19:53:04  brianp
 * fixed a memory leak from failing to incr display list reference count
 *
 * Revision 1.37  1996/01/07  22:49:41  brianp
 * call gl_init_lighting() when creating a context
 *
 * Revision 1.36  1996/01/05  01:21:20  brianp
 * added profiling
 *
 * Revision 1.35  1995/12/30  17:14:23  brianp
 * initialize CC.Current.IntColor
 *
 * Revision 1.34  1995/12/30  00:48:26  brianp
 * check for EightBitColor
 *
 * Revision 1.33  1995/12/12  21:43:18  brianp
 * default state of normalization changed to disabled
 *
 * Revision 1.32  1995/11/22  13:35:00  brianp
 * added MutableColors flag, test for RGBA mode before setting GAMMA_BIT
 *
 * Revision 1.31  1995/11/08  22:09:05  brianp
 * changed GL<type> assertions from == to >=
 *
 * Revision 1.30  1995/11/03  17:41:05  brianp
 * removed unused variables
 *
 * Revision 1.29  1995/11/01  21:44:15  brianp
 * added CC.Light.LastEnabled
 *
 * Revision 1.28  1995/10/27  20:28:27  brianp
 * added glPolygonOffsetEXT() support
 *
 * Revision 1.27  1995/10/19  15:45:20  brianp
 * added gamma support
 * new arguments to gl_new_context()
 *
 * Revision 1.26  1995/10/14  16:26:13  brianp
 * enable dithering by default
 * added SWmasking code
 *
 * Revision 1.25  1995/09/25  16:31:54  brianp
 * reorganized front and back material indexing
 *
 * Revision 1.24  1995/09/15  18:47:32  brianp
 * introduced CC.NewState convention
 * use bitmask flag for CC.Texture.TexGenEnabled
 *
 * Revision 1.23  1995/07/25  16:41:54  brianp
 * made changes for using CC.VertexFunc pointer
 *
 * Revision 1.22  1995/07/24  20:34:16  brianp
 * replaced memset() with MEMSET() and memcpy() with MEMCPY()
 *
 * Revision 1.21  1995/06/19  14:52:37  brianp
 * initialize current texture coordinate, per Asif Khan
 *
 * Revision 1.20  1995/06/05  20:26:24  brianp
 * added Unfilled field to gl_polygon struct
 *
 * Revision 1.19  1995/05/22  21:02:41  brianp
 * Release 1.2
 *
 * Revision 1.18  1995/05/17  13:52:37  brianp
 * implemented glIndexMask(0) and glColorMask(0,0,0,0)
 *
 * Revision 1.17  1995/05/17  13:17:22  brianp
 * changed default CC.Mode value to allow use of real OpenGL headers
 * removed need for CC.MajorMode variable
 *
 * Revision 1.16  1995/05/15  16:07:01  brianp
 * implemented shared/nonshared display lists
 *
 * Revision 1.15  1995/05/12  16:30:14  brianp
 * Texture images stored as bytes, not floats
 *
 * Revision 1.14  1995/04/17  13:51:19  brianp
 * added gl_copy_context() function
 *
 * Revision 1.13  1995/03/27  20:31:26  brianp
 * new Texture.Enabled scheme
 *
 * Revision 1.12  1995/03/24  16:59:56  brianp
 * added gl_update_pixel_logic
 *
 * Revision 1.11  1995/03/24  16:11:41  brianp
 * fixed logicop bug in gl_update_rasterflags
 *
 * Revision 1.10  1995/03/16  20:36:38  brianp
 * added call to gl_update_rasterflags in gl_set_context
 *
 * Revision 1.9  1995/03/10  16:26:43  brianp
 * updated for bleding extensions
 *
 * Revision 1.8  1995/03/09  21:40:14  brianp
 * added ModelViewInvValid initializer
 * added ComputePlane test to gl_update_rasterflags
 *
 * Revision 1.7  1995/03/09  19:07:16  brianp
 * added MESA_DEBUG env var support
 *
 * Revision 1.6  1995/03/08  15:10:02  brianp
 * added support for dd_logicop
 *
 * Revision 1.5  1995/03/04  19:29:44  brianp
 * 1.1 beta revision
 *
 * Revision 1.4  1995/03/02  19:17:54  brianp
 * new RasterMask logic, fixed some comments
 *
 * Revision 1.3  1995/02/27  22:48:28  brianp
 * modified for PB
 *
 * Revision 1.2  1995/02/24  15:19:23  brianp
 * *** empty log message ***
 *
 * Revision 1.1  1995/02/24  14:18:45  brianp
 * Initial revision
 *
 */


/*
 * The gl_context structure holds the current state of the library.
 * Typically, there will be a gl_context structure associated with each
 * window into which we're rendering:
 *    When we open a new rendering window we need a new gl_context.
 *    When we close a rendering window we destroy its gl_context.
 *    When we switch rendering to a different window we change gl_context.
 *
 * Throughout this implementation, references are made to CC which is
 * the Current Context.
 */


#pragma warning( disable : 4244 )

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "context.h"
#include "dd.h"
#include "draw.h"
#include "eval.h"
#include "light.h"
#include "lines.h"
#include "list.h"
#include "macros.h"
#include "pb.h"
#include "points.h"
#include "polygons.h"
/*#include "triangles.h"*/
#include "vb.h"



/* Copy of Current Context (PUBLIC) */
struct gl_context CC;

/* Pointer to Current Context (PRIVATE) */
static struct gl_context *CCptr = NULL;




/**********************************************************************/
/*****                   Profiling functions                      *****/
/**********************************************************************/

#ifdef PROFILE

#ifndef WIN32
#include <sys/times.h>
#include <sys/param.h>
#else
#include <windows.h>
#include <mesagl/mesagl32.h>
#include "../../src/profile.h"
#include "../../src/wmesadef.h"

extern WMesaContext WC;
#endif


/*
 * Return system time in seconds.
 * NOTE:  this implementation may not be very portable!
 */
GLdouble gl_time( void )
{
#ifndef WIN32
   static GLdouble prev_time = 0.0;
   static GLdouble time;
   struct tms tm;
   clock_t clk;

   clk = times(&tm);

#ifdef CLK_TCK
   time = (double)clk / (double)CLK_TCK;
#else
   time = (double)clk / (double)HZ;
#endif

#else
   static DWORD	prev_time = 0;
   static DWORD	time;
   time = GetTickCount();
#endif /* WIN32 */

   if (time>prev_time) {
      prev_time = time;
      return (GLdouble)time;
   }
   else {
	  return (GLdouble)prev_time;
   }
}



/*
 * Reset the timing/profiling counters
 */
static void init_timings( struct gl_context *c )
{
   c->BeginEndCount = 0;
   c->BeginEndTime = 0.0;
   c->VertexCount = 0;
   c->VertexTime = 0.0;
   c->PointCount = 0;
   c->PointTime = 0.0;
   c->LineCount = 0;
   c->LineTime = 0.0;
   c->PolygonCount = 0;
   c->PolygonTime = 0.0;
   c->ClearCount = 0;
   c->ClearTime = 0.0;
   c->SwapCount = 0;
   c->SwapTime = 0.0;
}



/*
 * Print the accumulated timing/profiling data.
 */
static void print_timings( struct gl_context *c )
{
   GLdouble beginendrate;
   GLdouble vertexrate;
   GLdouble pointrate;
   GLdouble linerate;
   GLdouble polygonrate;
   GLdouble overhead;
   GLdouble clearrate;
   GLdouble swaprate;
#ifdef WIN32
	GLdouble buffer_sizeRate = 0.0;
	GLdouble choose_line_functionRate = 0.0;
	GLdouble choose_points_functionRate = 0.0;
	GLdouble choose_polygon_functionRate = 0.0;
	GLdouble clearRate = 0.0;
	GLdouble clear_colorRate = 0.0;
	GLdouble clear_indexRate = 0.0;
	GLdouble fast_flat_rgb_lineRate = 0.0;
	GLdouble fast_flat_rgb_polygonRate = 0.0;
	GLdouble fast_rgb_pointsRate = 0.0;
	GLdouble flushRate = 0.0;
	GLdouble GetPaletteRate = 0.0;
	GLdouble read_color_pixelsRate = 0.0;
	GLdouble read_color_spanRate = 0.0;
	GLdouble read_index_pixelsRate = 0.0;
	GLdouble read_index_spanRate = 0.0;
	GLdouble set_bufferRate = 0.0;
	GLdouble set_colorRate = 0.0;
	GLdouble set_indexRate = 0.0;
	GLdouble write_color_pixelsRate = 0.0;
	GLdouble write_color_spanRate = 0.0;
	GLdouble write_index_pixelsRate = 0.0;
	GLdouble write_index_spanRate = 0.0;
	GLdouble write_monocolor_pixelsRate = 0.0;
	GLdouble write_monocolor_spanRate = 0.0;
	GLdouble write_monoindex_pixelsRate = 0.0;
	GLdouble write_monoindex_spanRate = 0.0;
   char		szDebug[4096];
   char		szTmp[256];

   if ( WC->profile.buffer_sizeTime > 0.0 ) {
	   buffer_sizeRate = WC->profile.buffer_sizeCount / WC->profile.buffer_sizeTime;
   }
   if ( WC->profile.choose_line_functionTime > 0.0 ) {
	   choose_line_functionRate = WC->profile.choose_line_functionCount / WC->profile.choose_line_functionTime;
   }
   if ( WC->profile.choose_points_functionTime > 0.0 ) {
	   choose_points_functionRate = WC->profile.choose_points_functionCount / WC->profile.choose_points_functionTime;
   }
   if ( WC->profile.choose_polygon_functionTime > 0.0 ) {
	   choose_polygon_functionRate = WC->profile.choose_polygon_functionCount / WC->profile.choose_polygon_functionTime;
   }
   if ( WC->profile.clearTime > 0.0 ) {
	   clearRate = WC->profile.clearCount / WC->profile.clearTime;
   }
   if ( WC->profile.clear_colorTime > 0.0 ) {
	   clear_colorRate = WC->profile.clear_colorCount / WC->profile.clear_colorTime;
   }
   if ( WC->profile.clear_indexTime > 0.0 ) {
	   clear_indexRate = WC->profile.clear_indexCount / WC->profile.clear_indexTime;
   }
   if ( WC->profile.fast_flat_rgb_lineTime > 0.0 ) {
	   fast_flat_rgb_lineRate = WC->profile.fast_flat_rgb_lineCount / WC->profile.fast_flat_rgb_lineTime;
   }
   if ( WC->profile.fast_flat_rgb_polygonTime > 0.0 ) {
	   fast_flat_rgb_polygonRate = WC->profile.fast_flat_rgb_polygonCount / WC->profile.fast_flat_rgb_polygonTime;
   }
   if ( WC->profile.fast_rgb_pointsTime > 0.0 ) {
	   fast_rgb_pointsRate = WC->profile.fast_rgb_pointsCount / WC->profile.fast_rgb_pointsTime;
   }
   if ( WC->profile.flushTime > 0.0 ) {
	   flushRate = WC->profile.flushCount / WC->profile.flushTime;
   }
   if ( WC->profile.GetPaletteTime > 0.0 ) {
	   GetPaletteRate = WC->profile.GetPaletteCount / WC->profile.GetPaletteTime;
   }
   if ( WC->profile.read_color_pixelsTime > 0.0 ) {
	   read_color_pixelsRate = WC->profile.read_color_pixelsCount / WC->profile.read_color_pixelsTime;
   }
   if ( WC->profile.read_color_spanTime > 0.0 ) {
	   read_color_spanRate = WC->profile.read_color_spanCount / WC->profile.read_color_spanTime;
   }
   if ( WC->profile.read_index_pixelsTime > 0.0 ) {
	   read_index_pixelsRate = WC->profile.read_index_pixelsCount / WC->profile.read_index_pixelsTime;
   }
   if ( WC->profile.read_index_spanTime > 0.0 ) {
	   read_index_spanRate = WC->profile.read_index_spanCount / WC->profile.read_index_spanTime;
   }
   if ( WC->profile.set_bufferTime > 0.0 ) {
	   set_bufferRate = WC->profile.set_bufferCount / WC->profile.set_bufferTime;
   }
   if ( WC->profile.set_colorTime > 0.0 ) {
	   set_colorRate = WC->profile.set_colorCount / WC->profile.set_colorTime;
   }
   if ( WC->profile.set_indexTime > 0.0 ) {
	   set_indexRate = WC->profile.set_indexCount / WC->profile.set_indexTime;
   }
   if ( WC->profile.write_color_pixelsTime > 0.0 ) {
	   write_color_pixelsRate = WC->profile.write_color_pixelsCount / WC->profile.write_color_pixelsTime;
   }
   if ( WC->profile.write_color_spanTime > 0.0 ) {
	   write_color_spanRate = WC->profile.write_color_spanCount / WC->profile.write_color_spanTime;
   }
   if ( WC->profile.write_index_pixelsTime > 0.0 ) {
	   write_index_pixelsRate = WC->profile.write_index_pixelsCount / WC->profile.write_index_pixelsTime;
   }
   if ( WC->profile.write_index_spanTime > 0.0 ) {
	   write_index_spanRate = WC->profile.write_index_spanCount / WC->profile.write_index_spanTime;
   }
   if ( WC->profile.write_monocolor_pixelsTime > 0.0 ) {
	   write_monocolor_pixelsRate = WC->profile.write_monocolor_pixelsCount / WC->profile.write_monocolor_pixelsTime;
   }
   if ( WC->profile.write_monocolor_spanTime > 0.0 ) {
	   write_monocolor_spanRate = WC->profile.write_monocolor_spanCount / WC->profile.write_monocolor_spanTime;
   }
   if ( WC->profile.write_monoindex_pixelsTime > 0.0 ) {
	   write_monoindex_pixelsRate = WC->profile.write_monoindex_pixelsCount / WC->profile.write_monoindex_pixelsTime;
   }
   if ( WC->profile.write_monoindex_spanTime > 0.0 ) {
	   write_monoindex_spanRate = WC->profile.write_monoindex_spanCount / WC->profile.write_monoindex_spanTime;
   }

#endif

   if (c->BeginEndTime>0.0) {
      beginendrate = c->BeginEndCount / c->BeginEndTime;
   }
   else {
      beginendrate = 0.0;
   }
   if (c->VertexTime>0.0) {
      vertexrate = c->VertexCount / c->VertexTime;
   }
   else {
      vertexrate = 0.0;
   }
   if (c->PointTime>0.0) {
      pointrate = c->PointCount / c->PointTime;
   }
   else {
      pointrate = 0.0;
   }
   if (c->LineTime>0.0) {
      linerate = c->LineCount / c->LineTime;
   }
   else {
      linerate = 0.0;
   }
   if (c->PolygonTime>0.0) {
      polygonrate = c->PolygonCount / c->PolygonTime;
   }
   else {
      polygonrate = 0.0;
   }
   if (c->ClearTime>0.0) {
      clearrate = c->ClearCount / c->ClearTime;
   }
   else {
      clearrate = 0.0;
   }
   if (c->SwapTime>0.0) {
      swaprate = c->SwapCount / c->SwapTime;
   }
   else {
      swaprate = 0.0;
   }


   overhead = c->BeginEndTime - c->VertexTime - c->PointTime
              - c->LineTime - c->PolygonTime;

#ifndef WIN32
   printf("                          Count   Time (s)    Rate (/s) \n");
   printf("--------------------------------------------------------\n");
   printf("glBegin/glEnd           %7d  %8.3f   %10.3f\n",
          c->BeginEndCount, c->BeginEndTime, beginendrate);
   printf("  vertexes transformed  %7d  %8.3f   %10.3f\n",
          c->VertexCount, c->VertexTime, vertexrate );
   printf("  points rasterized     %7d  %8.3f   %10.3f\n",
          c->PointCount, c->PointTime, pointrate );
   printf("  lines rasterized      %7d  %8.3f   %10.3f\n",
          c->LineCount, c->LineTime, linerate );
   printf("  polygons rasterized   %7d  %8.3f   %10.3f\n",
          c->PolygonCount, c->PolygonTime, polygonrate );
   printf("  overhead                       %8.3f\n", overhead );
   printf( "glClear                 %7d  %8.3f   %10.3f\n",
          c->ClearCount, c->ClearTime, clearrate );
   printf( "SwapBuffers             %7d  %8.3f   %10.3f\n",
          c->SwapCount, c->SwapTime, swaprate );
#else
   sprintf(szTmp, "                          Count   Time (ms)    Rate (/ms) \n");
   strcpy(szDebug, szTmp);
   sprintf(szTmp, "--------------------------------------------------------\n");
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
          "glBegin/glEnd", c->BeginEndCount, c->BeginEndTime, beginendrate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
          "  vertexes transformed", c->VertexCount, c->VertexTime, vertexrate );
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
          "  points rasterized", c->PointCount, c->PointTime, pointrate );
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
          "  lines rasterized", c->LineCount, c->LineTime, linerate );
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
          "  polygons rasterized", c->PolygonCount, c->PolygonTime, polygonrate );
   strcat(szDebug, szTmp);
   sprintf(szTmp, "  overhead                       %8.6g\n", overhead );
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
          "glClear", c->ClearCount, c->ClearTime, clearrate );
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
          "SwapBuffers", c->SwapCount, c->SwapTime, swaprate );
   strcat(szDebug, szTmp);
   OutputDebugString(szDebug);

   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "buffer_size", WC->profile.buffer_sizeCount, 
	   WC->profile.buffer_sizeTime, buffer_sizeRate);
   strcpy(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "choose_line_function", WC->profile.choose_line_functionCount, 
	   WC->profile.choose_line_functionTime, choose_line_functionRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "choose_points_function", WC->profile.choose_points_functionCount, 
	   WC->profile.choose_points_functionTime, choose_points_functionRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "choose_polygon_function", WC->profile.choose_polygon_functionCount, 
	   WC->profile.choose_polygon_functionTime, choose_polygon_functionRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "clear", WC->profile.clearCount, 
	   WC->profile.clearTime, clearRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "clear_color", WC->profile.clear_colorCount, 
	   WC->profile.clear_colorTime, clear_colorRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "clear_index", WC->profile.clear_indexCount, 
	   WC->profile.clear_indexTime, clear_indexRate);
   strcat(szDebug, szTmp);
   OutputDebugString(szDebug);

   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "fast_flat_rgb_line", WC->profile.fast_flat_rgb_lineCount, 
	   WC->profile.fast_flat_rgb_lineTime, fast_flat_rgb_lineRate);
   strcpy(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "fast_flat_rgb_polygon", WC->profile.fast_flat_rgb_polygonCount, 
	   WC->profile.fast_flat_rgb_polygonTime, fast_flat_rgb_polygonRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "fast_rgb_points", WC->profile.fast_rgb_pointsCount, 
	   WC->profile.fast_rgb_pointsTime, fast_rgb_pointsRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "flush", WC->profile.flushCount, 
	   WC->profile.flushTime, flushRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "GetPalette", WC->profile.GetPaletteCount, 
	   WC->profile.GetPaletteTime, GetPaletteRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "read_color_pixels", WC->profile.read_color_pixelsCount, 
	   WC->profile.read_color_pixelsTime, read_color_pixelsRate);
   strcat(szDebug, szTmp);
   OutputDebugString(szDebug);

   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "read_color_span", WC->profile.read_color_spanCount, 
	   WC->profile.read_color_spanTime, read_color_spanRate);
   strcpy(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "read_index_pixels", WC->profile.read_index_pixelsCount, 
	   WC->profile.read_index_pixelsTime, read_index_pixelsRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "read_index_span", WC->profile.read_index_spanCount, 
	   WC->profile.read_index_spanTime, read_index_spanRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "set_buffer", WC->profile.set_bufferCount, 
	   WC->profile.set_bufferTime, set_bufferRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "set_color", WC->profile.set_colorCount, 
	   WC->profile.set_colorTime, set_colorRate);
   strcat(szDebug, szTmp);
   OutputDebugString(szDebug);

   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "set_index", WC->profile.set_indexCount, 
	   WC->profile.set_indexTime, set_indexRate);
   strcpy(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "write_color_pixels", WC->profile.write_color_pixelsCount, 
	   WC->profile.write_color_pixelsTime, write_color_pixelsRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "write_color_span", WC->profile.write_color_spanCount, 
	   WC->profile.write_color_spanTime, write_color_spanRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "write_index_pixels", WC->profile.write_index_pixelsCount, 
	   WC->profile.write_index_pixelsTime, write_index_pixelsRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "write_index_span", WC->profile.write_index_spanCount, 
	   WC->profile.write_index_spanTime, write_index_spanRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "write_monocolor_pixels", WC->profile.write_monocolor_pixelsCount, 
	   WC->profile.write_monocolor_pixelsTime, write_monocolor_pixelsRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "write_monocolor_span", WC->profile.write_monocolor_spanCount, 
	   WC->profile.write_monocolor_spanTime, write_monocolor_spanRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "write_monoindex_pixels", WC->profile.write_monoindex_pixelsCount, 
	   WC->profile.write_monoindex_pixelsTime, write_monoindex_pixelsRate);
   strcat(szDebug, szTmp);
   sprintf(szTmp, "%-23s %7d  %8.6g   %10.6g\n",
	   "write_monoindex_span", WC->profile.write_monoindex_spanCount, 
	   WC->profile.write_monoindex_spanTime, write_monoindex_spanRate);
   strcat(szDebug, szTmp);

   OutputDebugString(szDebug);
#endif
}
#endif





/**********************************************************************/
/*****       Context allocation, initialization, destroying       *****/
/**********************************************************************/


/*
 * Allocate and initialize a display list group.
 */
static struct gl_list_group *alloc_display_list_group( void )
{
   struct gl_list_group *lg;
   GLuint i;

   lg = (struct gl_list_group*) malloc( sizeof(struct gl_list_group) );
   for (i=0;i<MAX_DISPLAYLISTS;i++) {
      lg->List[i] = NULL;
      lg->Reserved[i] = GL_FALSE;
   }
   lg->RefCount = 0;
   return lg;
}



/*
 * Initialize the nth light.  Note that the defaults for light 0 are
 * different than the other lights.
 */
static void init_light( struct gl_light *l, GLuint n )
{
   ASSIGN_4V( l->Ambient, 0.0, 0.0, 0.0, 1.0 );
   if (n==0) {
      ASSIGN_4V( l->Diffuse, 1.0, 1.0, 1.0, 1.0 );
      ASSIGN_4V( l->Specular, 1.0, 1.0, 1.0, 1.0 );
   }
   else {
      ASSIGN_4V( l->Diffuse, 0.0, 0.0, 0.0, 1.0 );
      ASSIGN_4V( l->Specular, 0.0, 0.0, 0.0, 1.0 );
   }
   ASSIGN_4V( l->Position, 0.0, 0.0, 1.0, 0.0 );
   ASSIGN_3V( l->Direction, 0.0, 0.0, -1.0 );
   l->SpotExponent = 0.0;
   gl_compute_spot_exp_table( l );
   l->SpotCutoff = 180.0;
   l->CosCutoff = -1.0;
   l->ConstantAttenuation = 1.0;
   l->LinearAttenuation = 0.0;
   l->QuadraticAttenuation = 0.0;
   l->Enabled = GL_FALSE;
}



static void init_lightmodel( struct gl_lightmodel *lm )
{
   ASSIGN_4V( lm->Ambient, 0.2, 0.2, 0.2, 1.0 );
   lm->LocalViewer = GL_FALSE;
   lm->TwoSide = GL_FALSE;
}


static void init_material( struct gl_material *m )
{
   ASSIGN_4V( m->Ambient,  0.2, 0.2, 0.2, 1.0 );
   ASSIGN_4V( m->Diffuse,  0.8, 0.8, 0.8, 1.0 );
   ASSIGN_4V( m->Specular, 0.0, 0.0, 0.0, 1.0 );
   ASSIGN_4V( m->Emission, 0.0, 0.0, 0.0, 1.0 );
   m->Shininess = 0.0;
   m->AmbientIndex = 0;
   m->DiffuseIndex = 1;
   m->SpecularIndex = 1;
   gl_compute_material_shine_table( m );
}



/*
 * Initialize a gl_context structure to default values.
 */
void gl_initialize_context( struct gl_context *c )
{
   static GLfloat identity[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
   };
   GLuint i;

   if (c) {
      /* Transformation matrices and stacks */
      c->ModelViewStackDepth = 0;
      MEMCPY( c->ModelViewMatrix, identity, 16*sizeof(GLfloat) );
      MEMCPY( c->ModelViewInv, identity, 16*sizeof(GLfloat) );
      c->ModelViewInvValid = GL_TRUE;

      c->ProjectionStackDepth = 0;
      MEMCPY( c->ProjectionMatrix, identity, 16*sizeof(GLfloat) );

      c->TextureStackDepth = 0;
      MEMCPY( c->TextureMatrix, identity, 16*sizeof(GLfloat) );
      c->IdentityTexMat = GL_TRUE;

      /* Accumulate buffer group */
      ASSIGN_4V( c->Accum.ClearColor, 0.0, 0.0, 0.0, 0.0 );

      /* Color buffer group */
      c->Color.IndexMask = 0xffffffff;
      c->Color.ColorMask = 0xf;
      c->Color.SWmasking = GL_FALSE;
      c->Color.ClearIndex = 0;
      ASSIGN_4V( c->Color.ClearColor, 0.0, 0.0, 0.0, 0.0 );
      c->Color.DrawBuffer = GL_FRONT;
      c->Color.AlphaEnabled = GL_FALSE;
      c->Color.AlphaFunc = GL_ALWAYS;
      c->Color.AlphaRef = 0.0;
      c->Color.BlendEnabled = GL_FALSE;
      c->Color.BlendSrc = GL_ONE;
      c->Color.BlendDst = GL_ZERO;
      c->Color.BlendEquation = GL_FUNC_ADD_EXT;
      ASSIGN_4V( c->Color.BlendColor, 0.0, 0.0, 0.0, 0.0 );
      c->Color.LogicOpEnabled = GL_FALSE;
      c->Color.SWLogicOpEnabled = GL_FALSE;
      c->Color.LogicOp = GL_COPY;
      c->Color.DitherFlag = GL_TRUE;

      /* Current group */
      c->Current.Index = 1;
      ASSIGN_3V( c->Current.Normal, 0.0, 0.0, 1.0 );
      c->Current.IntColor[0] = (GLint) c->RedScale;
      c->Current.IntColor[1] = (GLint) c->GreenScale;
      c->Current.IntColor[2] = (GLint) c->BlueScale;
      c->Current.IntColor[3] = (GLint) c->AlphaScale;
      ASSIGN_4V( c->Current.RasterPos, 0.0, 0.0, 0.0, 1.0 );
      c->Current.RasterPosValid = GL_TRUE;
      c->Current.RasterIndex = 1;
      ASSIGN_4V( c->Current.TexCoord, 0.0, 0.0, 0.0, 1.0 );
      ASSIGN_4V( c->Current.RasterColor, 1.0, 1.0, 1.0, 1.0 );
      c->Current.EdgeFlag = GL_TRUE;

      /* Depth buffer group */
      c->Depth.Test = GL_FALSE;
      c->Depth.Clear = 1.0;
      c->Depth.Func = GL_LESS;
      c->Depth.Mask = GL_TRUE;

      /* Evaluators group */
      c->Eval.Map1Color4 = GL_FALSE;
      c->Eval.Map1Index = GL_FALSE;
      c->Eval.Map1Normal = GL_FALSE;
      c->Eval.Map1TextureCoord1 = GL_FALSE;
      c->Eval.Map1TextureCoord2 = GL_FALSE;
      c->Eval.Map1TextureCoord3 = GL_FALSE;
      c->Eval.Map1TextureCoord4 = GL_FALSE;
      c->Eval.Map1Vertex3 = GL_FALSE;
      c->Eval.Map1Vertex4 = GL_FALSE;
      c->Eval.Map2Color4 = GL_FALSE;
      c->Eval.Map2Index = GL_FALSE;
      c->Eval.Map2Normal = GL_FALSE;
      c->Eval.Map2TextureCoord1 = GL_FALSE;
      c->Eval.Map2TextureCoord2 = GL_FALSE;
      c->Eval.Map2TextureCoord3 = GL_FALSE;
      c->Eval.Map2TextureCoord4 = GL_FALSE;
      c->Eval.Map2Vertex3 = GL_FALSE;
      c->Eval.Map2Vertex4 = GL_FALSE;
      c->Eval.AutoNormal = GL_FALSE;
      c->Eval.MapGrid1un = 1;
      c->Eval.MapGrid1u1 = 0.0;
      c->Eval.MapGrid1u2 = 1.0;
      c->Eval.MapGrid2un = 1;
      c->Eval.MapGrid2vn = 1;
      c->Eval.MapGrid2u1 = 0.0;
      c->Eval.MapGrid2u2 = 1.0;
      c->Eval.MapGrid2v1 = 0.0;
      c->Eval.MapGrid2v2 = 1.0;

      /* Fog group */
      c->Fog.Enabled = GL_FALSE;
      c->Fog.Mode = GL_EXP;
      ASSIGN_4V( c->Fog.Color, 0.0, 0.0, 0.0, 0.0 );
      c->Fog.Index = 0.0;
      c->Fog.Density = 1.0;
      c->Fog.Start = 0.0;
      c->Fog.End = 1.0;

      /* Hint group */
      c->Hint.PerspectiveCorrection = GL_DONT_CARE;
      c->Hint.PointSmooth = GL_DONT_CARE;
      c->Hint.LineSmooth = GL_DONT_CARE;
      c->Hint.PolygonSmooth = GL_DONT_CARE;
      c->Hint.Fog = GL_DONT_CARE;

      /* Lighting group */
      for (i=0;i<MAX_LIGHTS;i++) {
	 init_light( &c->Light.Light[i], i );
      }
      init_lightmodel( &c->Light.Model );
      init_material( &c->Light.Material[0] );
      init_material( &c->Light.Material[1] );
      c->Light.ShadeModel = GL_SMOOTH;
      c->Light.Enabled = GL_FALSE;
      c->Light.ColorMaterialFace = GL_FRONT_AND_BACK;
      c->Light.ColorMaterialMode = GL_AMBIENT_AND_DIFFUSE;
      c->Light.ColorMaterialEnabled = GL_FALSE;
      c->Light.LastEnabled = -1;

      /* Line group */
      c->Line.SmoothFlag = GL_FALSE;
      c->Line.StippleFlag = GL_FALSE;
      c->Line.Width = 1.0;
      c->Line.StipplePattern = 0xffff;
      c->Line.StippleFactor = 1;

      /* Display List group */
      c->List.ListBase = 0;

      /* Pixel group */
      c->Pixel.RedBias = 0.0;
      c->Pixel.RedScale = 1.0;
      c->Pixel.GreenBias = 0.0;
      c->Pixel.GreenScale = 1.0;
      c->Pixel.BlueBias = 0.0;
      c->Pixel.BlueScale = 1.0;
      c->Pixel.AlphaBias = 0.0;
      c->Pixel.AlphaScale = 1.0;
      c->Pixel.DepthBias = 0.0;
      c->Pixel.DepthScale = 1.0;
      c->Pixel.IndexOffset = 0;
      c->Pixel.IndexShift = 0;
      c->Pixel.ZoomX = 1.0;
      c->Pixel.ZoomY = 1.0;
      c->Pixel.MapColorFlag = GL_FALSE;
      c->Pixel.MapStencilFlag = GL_FALSE;
      c->Pixel.MapStoSsize = 1;
      c->Pixel.MapItoIsize = 1;
      c->Pixel.MapItoRsize = 1;
      c->Pixel.MapItoGsize = 1;
      c->Pixel.MapItoBsize = 1;
      c->Pixel.MapItoAsize = 1;
      c->Pixel.MapRtoRsize = 1;
      c->Pixel.MapGtoGsize = 1;
      c->Pixel.MapBtoBsize = 1;
      c->Pixel.MapAtoAsize = 1;
      c->Pixel.MapStoS[0] = 0;
      c->Pixel.MapItoI[0] = 0;
      c->Pixel.MapItoR[0] = 0.0;
      c->Pixel.MapItoG[0] = 0.0;
      c->Pixel.MapItoB[0] = 0.0;
      c->Pixel.MapItoA[0] = 0.0;
      c->Pixel.MapRtoR[0] = 0.0;
      c->Pixel.MapGtoG[0] = 0.0;
      c->Pixel.MapBtoB[0] = 0.0;
      c->Pixel.MapAtoA[0] = 0.0;

      /* Point group */
      c->Point.SmoothFlag = GL_FALSE;
      c->Point.Size = 1.0;

      /* Polygon group */
      c->Polygon.CullFlag = GL_FALSE;
      c->Polygon.CullFaceMode = GL_BACK;
      c->Polygon.FrontFace = GL_CCW;
      c->Polygon.FrontMode = GL_FILL;
      c->Polygon.BackMode = GL_FILL;
      c->Polygon.Unfilled = GL_FALSE;
      c->Polygon.SmoothFlag = GL_FALSE;
      c->Polygon.StippleFlag = GL_FALSE;
      c->Polygon.OffsetFactor = 0.0;
      c->Polygon.OffsetBias = 0.0;
      c->Polygon.OffsetEnabled = GL_FALSE;

      /* Polygon Stipple group */
      MEMSET( c->PolygonStipple, 0xff, 32*sizeof(GLuint) );

      /* Scissor group */
      c->Scissor.Enabled = GL_FALSE;
      c->Scissor.X = 0;
      c->Scissor.Y = 0;
      c->Scissor.Width = 0;
      c->Scissor.Height = 0;

      /* Stencil group */
      c->Stencil.Enabled = GL_FALSE;
      c->Stencil.Function = GL_ALWAYS;
      c->Stencil.FailFunc = GL_KEEP;
      c->Stencil.ZPassFunc = GL_KEEP;
      c->Stencil.ZFailFunc = GL_KEEP;
      c->Stencil.Ref = 0;
      c->Stencil.ValueMask = 0xff;
      c->Stencil.Clear = 0;
      c->Stencil.WriteMask = 0xff;

      /* Texture group */
      c->Texture.Enabled = 0;
      c->Texture.EnvMode = GL_MODULATE;
      ASSIGN_4V( c->Texture.EnvColor, 0.0, 0.0, 0.0, 0.0 );
      ASSIGN_4V( c->Texture.BorderColor, 0.0, 0.0, 0.0, 0.0 );
      c->Texture.TexGenEnabled = 0;
      c->Texture.GenModeS = GL_EYE_LINEAR;
      c->Texture.GenModeT = GL_EYE_LINEAR;
      c->Texture.GenModeR = GL_EYE_LINEAR;
      c->Texture.GenModeQ = GL_EYE_LINEAR;
      ASSIGN_4V( c->Texture.ObjectPlaneS, 1.0, 0.0, 0.0, 0.0 );
      ASSIGN_4V( c->Texture.ObjectPlaneT, 0.0, 1.0, 0.0, 0.0 );
      ASSIGN_4V( c->Texture.ObjectPlaneR, 0.0, 0.0, 0.0, 0.0 );
      ASSIGN_4V( c->Texture.ObjectPlaneQ, 0.0, 0.0, 0.0, 0.0 );
      ASSIGN_4V( c->Texture.EyePlaneS, 1.0, 0.0, 0.0, 0.0 );
      ASSIGN_4V( c->Texture.EyePlaneT, 0.0, 1.0, 0.0, 0.0 );
      ASSIGN_4V( c->Texture.EyePlaneR, 0.0, 0.0, 0.0, 0.0 );
      ASSIGN_4V( c->Texture.EyePlaneQ, 0.0, 0.0, 0.0, 0.0 );
      c->Texture.WrapS1D = GL_REPEAT;
      c->Texture.WrapT1D = GL_REPEAT;
      c->Texture.WrapS2D = GL_REPEAT;
      c->Texture.WrapT2D = GL_REPEAT;
      c->Texture.MinFilter1D = GL_NEAREST_MIPMAP_LINEAR;
      c->Texture.MagFilter1D = GL_LINEAR;
      c->Texture.MinFilter2D = GL_NEAREST_MIPMAP_LINEAR;
      c->Texture.MagFilter2D = GL_LINEAR;

      /* Transformation group */
      c->Transform.MatrixMode = GL_MODELVIEW;
      c->Transform.Normalize = GL_FALSE;
      for (i=0;i<MAX_CLIP_PLANES;i++) {
	 c->Transform.ClipEnabled[i] = GL_FALSE;
         ASSIGN_4V( c->Transform.ClipEquation[i], 0.0, 0.0, 0.0, 0.0 );
      }
      c->Transform.AnyClip = GL_FALSE;

      /* Viewport group */
      c->Viewport.X = 0;
      c->Viewport.Y = 0;
      c->Viewport.Width = 0;
      c->Viewport.Height = 0;   
      c->Viewport.Near = 0.0;
      c->Viewport.Far = 1.0;
      c->Viewport.Sx = 0.0;  /* Sx, Tx, Sy, Ty are computed later */
      c->Viewport.Tx = 0.0;
      c->Viewport.Sy = 0.0;
      c->Viewport.Ty = 0.0;
      c->Viewport.Sz = 0.5;
      c->Viewport.Tz = 0.5;

      /* Pixel transfer */
      c->PackAlignment = 4;
      c->PackRowLength = 0;
      c->PackSkipPixels = 0;
      c->PackSkipRows = 0;
      c->PackSwapBytes = GL_FALSE;
      c->PackLSBFirst = GL_FALSE;
      c->UnpackAlignment = 4;
      c->UnpackRowLength = 0;
      c->UnpackSkipPixels = 0;
      c->UnpackSkipRows = 0;
      c->UnpackSwapBytes = GL_FALSE;
      c->UnpackLSBFirst = GL_FALSE;

      /* Feedback */
      c->FeedbackType = GL_2D;   /* TODO: verify */
      c->FeedbackBuffer = NULL;
      c->FeedbackBufferSize = 0;
      c->FeedbackCount = 0;

      /* Selection/picking */
      c->SelectBuffer = NULL;
      c->SelectBufferSize = 0;
      c->SelectBufferCount = 0;
      c->SelectHits = 0;
      c->NameStackDepth = 0;

      /* Attribute stack */
      c->AttribStackDepth = 0;

      /* Texture maps */
      for (i=0;i<MAX_TEXTURE_LEVELS;i++) {
	 c->TextureComponents1D[i] = 0;
	 c->TextureWidth1D[i] = 0;
	 c->TextureBorder1D[i] = 0;
	 c->TextureImage1D[i] = NULL;
	 c->TextureImage1DDeleteFlag[i] = GL_FALSE;
	 c->TextureComponents2D[i] = 0;
	 c->TextureWidth2D[i] = 0;
	 c->TextureHeight2D[i] = 0;
	 c->TextureBorder2D[i] = 0;
	 c->TextureImage2D[i] = NULL;
	 c->TextureImage2DDeleteFlag[i] = GL_FALSE;
      }

      /*** Miscellaneous ***/
      c->NewState = GL_TRUE;
      c->RenderMode = GL_RENDER;
      c->Mode = GL_BITMAP;

      c->StippleCounter = 0;
      c->NeedNormals = GL_FALSE;

      if (c->RedScale==255.0F && c->GreenScale==255.0F && c->BlueScale==255.0F
          && c->AlphaScale==255.0F) {
         c->EightBitColor = GL_TRUE;
      }
      else {
         c->EightBitColor = GL_FALSE;
      }
      c->FastDrawPixels = c->RGBAflag && c->EightBitColor;

      c->VertexFunc = gl_execute_vertex;
      c->PointsFunc = NULL;
      c->LineFunc = NULL;
      c->PolygonFunc = NULL;
      c->AuxPolygonFunc = NULL;

      c->CallDepth = 0;
      c->ExecuteFlag = GL_TRUE;
      c->CompileFlag = GL_FALSE;

      c->BufferWidth = 0;
      c->BufferHeight = 0;
      c->DepthBuffer = NULL;
      c->AccumBuffer = NULL;
      c->StencilBuffer = NULL;

      c->ErrorValue = GL_NO_ERROR;
   }
}



/*
 * Allocate and initialize a gl_context structure.
 * Input:  rgb_flag - GL_TRUE or GL_FALSE to indicate if using RGB mode
 *         redscale, greenscale, bluescale, alphascale - if in RGB mode
 *               these values are used to scale floating point color values
 *               in [0,1] to integers in [0,scale]
 *         db_flag - GL_TRUE or GL_FALSE to indicate if using double buffering
 *         sharelist - another context to share display lists with or NULL
 */
struct gl_context *gl_new_context( GLboolean rgb_flag,
                                   GLfloat redscale,
                                   GLfloat greenscale,
                                   GLfloat bluescale,
                                   GLfloat alphascale,
                                   GLboolean db_flag,
                                   struct gl_context *shareList )
{
   struct gl_context *c;

   /* do some implementation tests */
   assert( sizeof(GLbyte) >= 1 );
   assert( sizeof(GLshort) >= 2 );
   assert( sizeof(GLint) >= 4 );
   assert( sizeof(GLubyte) >= 1 );
   assert( sizeof(GLushort) >= 2 );
   assert( sizeof(GLuint) >= 4 );

   gl_init_lists();
   gl_init_eval();
   gl_init_vb();

   c = (struct gl_context *) malloc( sizeof(struct gl_context) );
   if (c) {
      c->RGBAflag = rgb_flag;

      c->RedScale = redscale;
      c->GreenScale = greenscale;
      c->BlueScale = bluescale;
      c->AlphaScale = alphascale;

      gl_initialize_context( c );

      if (db_flag) {
         c->DBflag = GL_TRUE;
         c->Color.DrawBuffer = GL_BACK;
         c->Pixel.ReadBuffer = GL_BACK;
      }
      else {
         c->DBflag = GL_FALSE;
         c->Color.DrawBuffer = GL_FRONT;
         c->Pixel.ReadBuffer = GL_FRONT;
      }

      if (shareList) {
	 /* share the group of display lists of another context */
	 c->ListGroup = shareList->ListGroup;
      }
      else {
	 /* allocate new group of display lists */
	 c->ListGroup = alloc_display_list_group();
      }
      c->ListGroup->RefCount++;

#ifdef PROFILE
      init_timings( c );
#endif

      /* software alpha buffers */
      {
         char *alpha = getenv("MESA_ALPHA");
         c->FrontAlphaEnabled = c->BackAlphaEnabled = GL_FALSE;
         if (alpha) {
            if (strstr(alpha,"front")) {
               c->FrontAlphaEnabled = GL_TRUE;
            }
            if (strstr(alpha,"back")) {
               c->BackAlphaEnabled = GL_TRUE;
            }
         }
      }

   }
   return c;
}




/*
 * Destroy a gl_context structure.
 */
void gl_destroy_context( struct gl_context *c )
{
   if (c) {

#ifdef PROFILE
#ifndef WIN32
      if (getenv("MESA_PROFILE")) {
#else
	  if (TRUE){
#endif
         struct gl_context *cc;

         if (c==CCptr) {
            cc = &CC;
         }
         else {
            cc = c;
         }
         print_timings( cc );
      }
#endif

      if (c->DepthBuffer)
	 free(c->DepthBuffer);
      if (c->AccumBuffer)
	 free(c->AccumBuffer);
      if (c->StencilBuffer)
         free(c->StencilBuffer);

      c->ListGroup->RefCount--;
      assert(c->ListGroup->RefCount>=0);
      if (c->ListGroup->RefCount==0) {
	 /* free display list group */
	 free( c->ListGroup );
      }
      free( (void *) c );
      if (c==CCptr) {
         CCptr = NULL;
      }

   }
}



/*
 * Set the current context.
 */
void gl_set_context( struct gl_context *c )
{
   if (c) {
      /* "write back" current context */
      if (CCptr) {
         MEMCPY( CCptr, &CC, sizeof(struct gl_context) );
      }
      /* "load" new context */
      CCptr = c;
      MEMCPY( &CC, c, sizeof(struct gl_context) );
   }
   PB_INIT( GL_BITMAP );
   CC.NewState = GL_TRUE;
}



/*
 * Copy attribute groups from one context to another.
 * Input:  src - source context
 *         dst - destination context
 *         mask - bitwise OR of GL_*_BIT flags
 */
void gl_copy_context( struct gl_context *src, struct gl_context *dst,
		      GLuint mask )
{
   if (src==CCptr) {
      src = &CC;
   }
   else if (dst==CCptr) {
      dst = &CC;
   }

   if (mask & GL_ACCUM_BUFFER_BIT) {
      MEMCPY( &dst->Accum, &src->Accum, sizeof(struct gl_accum_attrib) );
   }
   if (mask & GL_COLOR_BUFFER_BIT) {
      MEMCPY( &dst->Color, &src->Color, sizeof(struct gl_colorbuffer_attrib) );
   }
   if (mask & GL_CURRENT_BIT) {
      MEMCPY( &dst->Current, &src->Current, sizeof(struct gl_current_attrib) );
   }
   if (mask & GL_DEPTH_BUFFER_BIT) {
      MEMCPY( &dst->Depth, &src->Depth, sizeof(struct gl_depthbuffer_attrib) );
   }
   if (mask & GL_ENABLE_BIT) {
      /* no op */
   }
   if (mask & GL_EVAL_BIT) {
      MEMCPY( &dst->Eval, &src->Eval, sizeof(struct gl_eval_attrib) );
   }
   if (mask & GL_FOG_BIT) {
      MEMCPY( &dst->Fog, &src->Fog, sizeof(struct gl_fog_attrib) );
   }
   if (mask & GL_HINT_BIT) {
      MEMCPY( &dst->Hint, &src->Hint, sizeof(struct gl_hint_attrib) );
   }
   if (mask & GL_LIGHTING_BIT) {
      MEMCPY( &dst->Light, &src->Light, sizeof(struct gl_light_attrib) );
   }
   if (mask & GL_LINE_BIT) {
      MEMCPY( &dst->Line, &src->Line, sizeof(struct gl_line_attrib) );
   }
   if (mask & GL_LIST_BIT) {
      MEMCPY( &dst->List, &src->List, sizeof(struct gl_list_attrib) );
   }
   if (mask & GL_PIXEL_MODE_BIT) {
      MEMCPY( &dst->Pixel, &src->Pixel, sizeof(struct gl_pixel_attrib) );
   }
   if (mask & GL_POINT_BIT) {
      MEMCPY( &dst->Point, &src->Point, sizeof(struct gl_point_attrib) );
   }
   if (mask & GL_POLYGON_BIT) {
      MEMCPY( &dst->Polygon, &src->Polygon, sizeof(struct gl_polygon_attrib) );
   }
   if (mask & GL_POLYGON_STIPPLE_BIT) {
      MEMCPY( &dst->PolygonStipple, &src->PolygonStipple, 32*sizeof(GLuint) );
   }
   if (mask & GL_SCISSOR_BIT) {
      MEMCPY( &dst->Scissor, &src->Scissor, sizeof(struct gl_scissor_attrib) );
   }
   if (mask & GL_STENCIL_BUFFER_BIT) {
      MEMCPY( &dst->Stencil, &src->Stencil, sizeof(struct gl_stencil_attrib) );
   }
   if (mask & GL_TEXTURE_BIT) {
      MEMCPY( &dst->Texture, &src->Texture, sizeof(struct gl_texture_attrib) );
   }
   if (mask & GL_TRANSFORM_BIT) {
      MEMCPY( &dst->Transform, &src->Transform, sizeof(struct gl_transform_attrib) );
   }
   if (mask & GL_VIEWPORT_BIT) {
      MEMCPY( &dst->Viewport, &src->Viewport, sizeof(struct gl_viewport_attrib) );
   }
}



/*
 * This is Mesa's error handler.  Normally, all that's done is the updating
 * of the current error value.  If Mesa is compiled with -DDEBUG or if the
 * environment variable "MESA_DEBUG" is defined then a real error message
 * is printed to stderr.
 * Input:  error - the error value
 *         s - a diagnostic string
 */
void gl_error( GLenum error, char *s )
{
   GLboolean debug;

#ifdef DEBUG
   debug = GL_TRUE;
#else
   if (getenv("MESA_DEBUG")) {
      debug = GL_TRUE;
   }
   else {
      debug = GL_FALSE;
   }
#endif

   if (debug) {
      char errstr[1000];

      switch (error) {
	 case GL_NO_ERROR:
	    strcpy( errstr, "GL_NO_ERROR" );
	    break;
	 case GL_INVALID_VALUE:
	    strcpy( errstr, "GL_INVALID_VALUE" );
	    break;
	 case GL_INVALID_ENUM:
	    strcpy( errstr, "GL_INVALID_ENUM" );
	    break;
	 case GL_INVALID_OPERATION:
	    strcpy( errstr, "GL_INVALID_OPERATION" );
	    break;
	 case GL_STACK_OVERFLOW:
	    strcpy( errstr, "GL_STACK_OVERFLOW" );
	    break;
	 case GL_STACK_UNDERFLOW:
	    strcpy( errstr, "GL_STACK_UNDERFLOW" );
	    break;
	 case GL_OUT_OF_MEMORY:
	    strcpy( errstr, "GL_OUT_OF_MEMORY" );
	    break;
	 default:
	    strcpy( errstr, "unknown" );
	    break;
      }
      fprintf( stderr, "Mesa Error (%s): %s\n", errstr, s );
   }

   if (CC.ErrorValue==GL_NO_ERROR) {
      CC.ErrorValue = error;
   }
}



GLenum glGetError( void )
{
   GLenum e;

   if (!CCptr) {
      /* No current context */
      return GL_NO_ERROR;
   }

   if (INSIDE_BEGIN_END) {
      gl_error( GL_INVALID_OPERATION, "glGetError" );
      return GL_INVALID_OPERATION;
   }

   e = CC.ErrorValue;
   CC.ErrorValue = GL_NO_ERROR;
   return e;
}



/*
 * Since the device driver may or may not support pixel logic ops we
 * have to make some extensive tests whenever glLogicOp, glBlendFunc,
 * glBlendEquation, glEn/Disable( GL_LOGIC_OP ), glEn/Disable( GL_BLEND ),
 * or glPopAttrib is called.
 */
static void update_pixel_logic( void )
{
   if (CC.RGBAflag) {
      /* RGBA mode blending w/ Logic Op */
      if (CC.Color.BlendEnabled && CC.Color.BlendEquation==GL_LOGIC_OP) {
	 if ((*DD.logicop)( CC.Color.LogicOp )) {
	    /* Device driver can do logic, don't have to do it in software */
	    CC.Color.SWLogicOpEnabled = GL_FALSE;
	 }
	 else {
	    /* Device driver can't do logic op so we do it in software */
	    CC.Color.SWLogicOpEnabled = GL_TRUE;
	 }
      }
      else {
	 /* no logic op */
	 (void) (*DD.logicop)( GL_COPY );
	 CC.Color.SWLogicOpEnabled = GL_FALSE;
      }
   }
   else {
      /* CI mode Logic Op */
      if (CC.Color.LogicOpEnabled) {
	 if ((*DD.logicop)( CC.Color.LogicOp )) {
	    /* Device driver can do logic, don't have to do it in software */
	    CC.Color.SWLogicOpEnabled = GL_FALSE;
	 }
	 else {
	    /* Device driver can't do logic op so we do it in software */
	    CC.Color.SWLogicOpEnabled = GL_TRUE;
	 }
      }
      else {
	 /* no logic op */
	 (void) (*DD.logicop)( GL_COPY );
	 CC.Color.SWLogicOpEnabled = GL_FALSE;
      }
   }
}



/*
 * Check if software implemented RGBA or Color Index masking is needed.
 */
static void update_pixel_masking( void )
{
   if (CC.RGBAflag) {
      if (CC.Color.ColorMask==0xf) {
         /* disable masking */
         (void) (*DD.color_mask)( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
         CC.Color.SWmasking = GL_FALSE;
      }
      else {
         /* Ask DD to do color masking, if it can't we'll do it in software */
         GLboolean red   = (CC.Color.ColorMask & 8) ? GL_TRUE : GL_FALSE;
         GLboolean green = (CC.Color.ColorMask & 4) ? GL_TRUE : GL_FALSE;
         GLboolean blue  = (CC.Color.ColorMask & 2) ? GL_TRUE : GL_FALSE;
         GLboolean alpha = (CC.Color.ColorMask & 1) ? GL_TRUE : GL_FALSE;
         if ((*DD.color_mask)( red, green, blue, alpha )) {
            CC.Color.SWmasking = GL_FALSE;
         }
         else {
            CC.Color.SWmasking = GL_TRUE;
         }
      }
   }
   else {
      if (CC.Color.IndexMask==0xffffffff) {
         /* disable masking */
         (void) (*DD.index_mask)( 0xffffffff );
         CC.Color.SWmasking = GL_FALSE;
      }
      else {
         /* Ask DD to do index masking, if it can't we'll do it in software */
         if ((*DD.index_mask)( CC.Color.IndexMask )) {
            CC.Color.SWmasking = GL_FALSE;
         }
         else {
            CC.Color.SWmasking = GL_TRUE;
         }
      }
   }
}



/*
 * Recompute the value of CC.RasterMask, CC.ClipMask, etc. according to
 * the current context.
 */
static void update_rasterflags( void )
{
   CC.RasterMask = 0;

   if (CC.Color.AlphaEnabled)		CC.RasterMask |= ALPHATEST_BIT;
   if (CC.Color.BlendEnabled)		CC.RasterMask |= BLEND_BIT;
   if (CC.Depth.Test)			CC.RasterMask |= DEPTH_BIT;
   if (CC.Fog.Enabled)			CC.RasterMask |= FOG_BIT;
   if (CC.Color.SWLogicOpEnabled)	CC.RasterMask |= LOGIC_OP_BIT;
   if (CC.Scissor.Enabled)		CC.RasterMask |= SCISSOR_BIT;
   if (CC.Stencil.Enabled)		CC.RasterMask |= STENCIL_BIT;
   if (CC.Color.SWmasking)		CC.RasterMask |= MASKING_BIT;
   if (CC.FrontAlphaEnabled)		CC.RasterMask |= ALPHABUF_BIT;
   if (CC.BackAlphaEnabled)		CC.RasterMask |= ALPHABUF_BIT;

   /* Recompute ClipMask (what has to be interpolated when clipping) */
   CC.ClipMask = 0;
   if (CC.Texture.Enabled) {
      CC.ClipMask |= CLIP_TEXTURE_BIT;
   }
   if (CC.Light.ShadeModel==GL_SMOOTH) {
      if (CC.RGBAflag) {
	 CC.ClipMask |= CLIP_FCOLOR_BIT;
	 if (CC.Light.Model.TwoSide) {
	    CC.ClipMask |= CLIP_BCOLOR_BIT;
	 }
      }
      else {
	 CC.ClipMask |= CLIP_FINDEX_BIT;
	 if (CC.Light.Model.TwoSide) {
	    CC.ClipMask |= CLIP_BINDEX_BIT;
	 }
      }
   }


   /* Check if the equation of the plane for polygons has to be computed. */
   CC.ComputePlane = CC.Depth.Test || CC.Polygon.CullFlag
                  || CC.Light.Model.TwoSide || CC.Texture.Enabled
                  || CC.Polygon.OffsetEnabled || CC.Polygon.Unfilled;
}



/*
 * If CC.NewState==GL_TRUE then this function MUST be called before
 * rendering any primitive.  Basically, function pointers and miscellaneous
 * flags are updated to reflect the current state of the state machine.
 */
void gl_update_state( void )
{
   update_pixel_logic();
   update_pixel_masking();
   update_rasterflags();

   gl_update_lighting();

   /* Check if normal vectors are needed */
   if (CC.Light.Enabled
       || (CC.Texture.GenModeS==GL_SPHERE_MAP
           && (CC.Texture.TexGenEnabled & S_BIT))
       || (CC.Texture.GenModeT==GL_SPHERE_MAP
           && (CC.Texture.TexGenEnabled & T_BIT))) {
      CC.NeedNormals = GL_TRUE;
   }
   else {
      CC.NeedNormals = GL_FALSE;
   }

   /* Check if incoming colors can be modified during rasterization */
   if (CC.Fog.Enabled ||
       CC.Texture.Enabled ||
       CC.Color.BlendEnabled ||
       CC.Color.SWmasking ||
       CC.Color.SWLogicOpEnabled) {
      CC.MutablePixels = GL_TRUE;
   }
   else {
      CC.MutablePixels = GL_FALSE;
   }

   /* Check if all pixels generated are likely to be the same color */
   if (CC.Light.ShadeModel==GL_SMOOTH ||
       CC.Light.Enabled ||
       CC.Fog.Enabled ||
       CC.Texture.Enabled ||
       CC.Color.BlendEnabled ||
       CC.Color.SWmasking ||
       CC.Color.SWLogicOpEnabled) {
      CC.MonoPixels = GL_FALSE;       /* pixels probably multicolored */
   }
   else {
      /* pixels will all be same color, only glColor() can invalidate this. */
      CC.MonoPixels = GL_TRUE;
   }

   CC.Polygon.CullBits = 0;
   if (CC.Polygon.CullFlag) {
      if (CC.Polygon.CullFaceMode==GL_FRONT ||
          CC.Polygon.CullFaceMode==GL_FRONT_AND_BACK) {
         CC.Polygon.CullBits |= 1;
      }
      if (CC.Polygon.CullFaceMode==GL_BACK ||
          CC.Polygon.CullFaceMode==GL_FRONT_AND_BACK) {
         CC.Polygon.CullBits |= 2;
      }
   }

   gl_set_point_function();
   gl_set_line_function();
   gl_set_polygon_function();
   /*gl_set_triangle_function();*/

   CC.NewState = GL_FALSE;
}

