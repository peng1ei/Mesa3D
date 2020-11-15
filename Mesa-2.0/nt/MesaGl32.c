/*
	MesaGL32.c
*/
#define MESAGL32_C

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <mesagl/MesaGL32.h>
#include "../../src/context.h"
#include "../../src/dd.h"
#include "../../src/xform.h"
#include "../../src/vb.h"
#ifdef PROFILE
#include "../../src/profile.h"
#endif
#include "../../src/wmesadef.h"
#include <wing.h>

/* Bit's used for dest: */
#define FRONT_PIXMAP	1
#define BACK_PIXMAP	2
#define BACK_XIMAGE	4

static PWMC Current = NULL;
WMesaContext WC = NULL;

#ifdef NDEBUG
  #define assert(ignore)	((void) 0)
#else
  void Mesa_Assert(void *Cond,void *File,unsigned Line)
  {
    char Msg[512];
    sprintf(Msg,"%s %s %d",Cond,File,Line);
    MessageBox(NULL,Msg,"Assertion failed.",MB_OK);
    exit(1);
  }
  #define assert(e)	if (!e) Mesa_Assert(#e,__FILE__,__LINE__);
#endif

#define DD_GETDC ((Current->db_flag) ? Current->dib.hDC : Current->hDC)
#define DD_RELEASEDC

#define BEGINGDICALL	if(Current->rgb_flag)wmFlushBits(Current);
#define ENDGDICALL		if(Current->rgb_flag)wmGetBits(Current);

#define FLIP(Y)  (Current->height-(Y)-1)


static void FlushToFile(PWMC pwc, PSTR	szFile);


/* Finish all pending operations and synchronize. */
static void finish(void)
{
   /* no op */
}


//
// We cache all gl draw routines until a flush is made
//
static void flush(void)
{
	STARTPROFILE
	if(Current->rgb_flag && !(Current->dib.fFlushed)){
		wmFlush(Current);
	}
	ENDPROFILE(flush)
   
}



/*
 * Set the color index used to clear the color buffer.
 */
static void clear_index(GLuint index)
{
	STARTPROFILE
  Current->clearpixel = index;
	ENDPROFILE(clear_index)
}



/*
 * Set the color used to clear the color buffer.
 */
static void clear_color( GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   STARTPROFILE
  Current->clearpixel=RGB(r, g, b );
  ENDPROFILE(clear_color)
}



/*
 * Clear the specified region of the color buffer using the clear color
 * or index as specified by one of the two functions above.
 */
static void clear(GLboolean all,GLint x, GLint y, GLint width, GLint height )
{
    STARTPROFILE
	DWORD	dwColor;	
	WORD	wColor;
	LPDWORD	lpdw = (LPDWORD)Current->pbPixels;
	LPWORD	lpw = (LPWORD)Current->pbPixels;
	LPBYTE	lpb;
	BYTE	bPix[12];
	LPBYTE	lpb = Current->pbPixels;

	if (all){
		x=y=0;
		width=Current->width;
		height=Current->height;
	}
	if (Current->rgb_flag==GL_TRUE){
		UINT	nBypp = Current->cColorBits / 8;
		int		i = 0;
		int		iSize;

		if(nBypp == 2){
			iSize = (Current->width * Current->height) / nBypp;

			wColor = BGR16(GetRValue(Current->clearpixel), 
						   GetGValue(Current->clearpixel), 
						   GetBValue(Current->clearpixel));
			dwColor = MAKELONG(wColor, wColor);
		}
		else if nBypp == 4{
			iSize = (Current->width * Current->height);

			dwColor = BGR32(GetRValue(Current->clearpixel), 
						   GetGValue(Current->clearpixel), 
						   GetBValue(Current->clearpixel));
		}
		//
		// This is the 24bit case
		//
		else {

			iSize = (Current->width * Current->height) / nBypp;

			dwColor = BGR24(GetRValue(Current->clearpixel), 
						   GetGValue(Current->clearpixel), 
						   GetBValue(Current->clearpixel));


			while(i < iSize){
				*lpdw = dwColor;
				lpb += nBypp;
				lpdw = (LPDWORD)lpb;
				i++;
			}

			ENDPROFILE(clear)

			return;
		}

		while(i < iSize){
			*lpdw = dwColor;
			lpdw++;
			i++;
		}
	}

	else {
		int i;
		char *Mem=Current->ScreenMem+y*Current->ScanWidth+x;
		for (i=0; i<height; i++){
			memset(Mem,Current->clearpixel,width);
			Mem+=width;
		}
	}

	ENDPROFILE(clear)
}



/* Set the current color index. */
static void set_index(GLuint index)
{
   STARTPROFILE
  Current->pixel=index;
   ENDPROFILE(set_index)
}



/* Set the current RGBA color. */
static void set_color( GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   STARTPROFILE
  Current->pixel = RGB( r, g, b );
   ENDPROFILE(set_color)
}



/* Set the index mode bitplane mask. */
static GLboolean index_mask(GLuint mask)
{
   /* can't implement */
   return GL_FALSE;
}



/* Set the RGBA drawing mask. */
static GLboolean color_mask( GLboolean rmask, GLboolean gmask,
							 GLboolean bmask, GLboolean amask)
{
   /* can't implement */
   return GL_FALSE;
}



/*
 * Set the pixel logic operation.  Return GL_TRUE if the device driver
 * can perform the operation, otherwise return GL_FALSE.  If GL_FALSE
 * is returned, the logic op will be done in software by Mesa.
 */
GLboolean logicop( GLenum op )
{
   /* can't implement */
   return GL_FALSE;
}


static void dither( GLboolean enable )
{
   /* No op */
}



static GLboolean set_buffer( GLenum mode )
{
   STARTPROFILE
   /* TODO: this could be better */
   if (mode==GL_FRONT || mode==GL_BACK) {
      return GL_TRUE;
   }
   else {
      return GL_FALSE;
   }
   ENDPROFILE(set_buffer)
}



/* Return characteristics of the output buffer. */
static void buffer_size( GLuint *width, GLuint *height, GLuint *depth )
{
	STARTPROFILE
	int New_Size;
	RECT CR;
	
	GetClientRect(Current->Window,&CR);

	*width=CR.right;
	*height=CR.bottom;
	*depth = Current->depth;

	New_Size=((*width)!=Current->width) || ((*height)!=Current->height);

	if (New_Size){
		Current->width=*width;
		Current->ScanWidth=Current->width;
		Current->height=*height;

		if (Current->db_flag){
			if (Current->rgb_flag==GL_TRUE){
				wmDeleteBackingStore(Current);
				wmCreateBackingStore(Current, Current->width, current->height);
			}
			else{
				Current->IndexFormat->bmiHeader.biWidth=Current->width;

				if (Current->IndexFormat->bmiHeader.biHeight<0)
					Current->IndexFormat->bmiHeader.biHeight=-Current->height;
				else
					Current->IndexFormat->bmiHeader.biHeight=Current->height;

				Current->Compat_BM=WinGCreateBitmap(Current->dib.hDC,Current->IndexFormat,&((void *) Current->ScreenMem));

				DeleteObject(SelectObject(Current->dib.hDC,Current->Compat_BM));
			}
		}
	}

	ENDPROFILE(buffer_size)
}



/**********************************************************************/
/*****           Accelerated point, line, polygon rendering       *****/
/**********************************************************************/


static void fast_rgb_points( GLuint first, GLuint last )
{
   STARTPROFILE
   GLuint i;
   HDC DC=DD_GETDC;
	PWMC	pwc = Current;
   if (VB.MonoColor) {
      /* all drawn with current color */
      for (i=first;i<=last;i++) {
         if (VB.Unclipped[i]) {
            int x, y;
            x =       (GLint) VB.Win[i][0];
            y = FLIP( (GLint) VB.Win[i][1] );
			wmSetPixel(pwc, y,x,GetRValue(Current->pixel), 
					    GetGValue(Current->pixel), GetBValue(Current->pixel));
         }
      }
   }
   else {
      /* draw points of different colors */
      for (i=first;i<=last;i++) {
         if (VB.Unclipped[i]) {
            int x, y;
            unsigned long pixel=RGB(VB.Color[i][0]*255.0,
                                    VB.Color[i][1]*255.0,
                                    VB.Color[i][2]*255.0);
            x =       (GLint) VB.Win[i][0];
            y = FLIP( (GLint) VB.Win[i][1] );
			wmSetPixel(pwc, y,x,VB.Color[i][0]*255.0, 
                                    VB.Color[i][1]*255.0,
                                    VB.Color[i][2]*255.0);
         }
      }
   }
   DD_RELEASEDC;
   ENDPROFILE(fast_rgb_points)
}



/* Return pointer to accerated points function */
static points_func choose_points_function( void )
{
   STARTPROFILE
   if (CC.Point.Size==1.0 && !CC.Point.SmoothFlag && CC.RasterMask==0
       && !CC.Texture.Enabled  && Current->rgb_flag) {
   ENDPROFILE(choose_points_function)
      return fast_rgb_points;
   }
   else {
   ENDPROFILE(choose_points_function)
      return NULL;
   }
}



/* Draw a line using the color specified by VB.Color[pv] */
static void fast_flat_rgb_line( GLuint v0, GLuint v1, GLuint pv )
{
	STARTPROFILE
	int x0, y0, x1, y1;
	unsigned long pixel;
	HDC DC=DD_GETDC;
	HPEN Pen;
	HPEN Old_Pen;

	if (VB.MonoColor) {
	  pixel = Current->pixel;  /* use current color */
	}
	else {
	  pixel = RGB(VB.Color[pv][0]*255.0, VB.Color[pv][1]*255.0, VB.Color[pv][2]*255.0);
	}

	x0 =       (int) VB.Win[v0][0];
	y0 = FLIP( (int) VB.Win[v0][1] );
	x1 =       (int) VB.Win[v1][0];
	y1 = FLIP( (int) VB.Win[v1][1] );


	BEGINGDICALL

	Pen=CreatePen(PS_SOLID,1,pixel);
	Old_Pen=SelectObject(DC,Pen);
	MoveToEx(DC,x0,y0,NULL);
	LineTo(DC,x1,y1);
	SelectObject(DC,Old_Pen);
	DeleteObject(Pen);
	DD_RELEASEDC;

	ENDGDICALL

	ENDPROFILE(fast_flat_rgb_line)
}



/* Return pointer to accerated line function */
static line_func choose_line_function( void )
{
   STARTPROFILE
   if (CC.Line.Width==1.0 && !CC.Line.SmoothFlag && !CC.Line.StippleFlag
       && CC.Light.ShadeModel==GL_FLAT && CC.RasterMask==0
       && !CC.Texture.Enabled && Current->rgb_flag) {
   ENDPROFILE(choose_line_function)
      return fast_flat_rgb_line;
   }
   else {
   ENDPROFILE(choose_line_function)
      return NULL;
   }
}


/* Draw a convex polygon using color VB.Color[pv] */
static void fast_flat_rgb_polygon( GLuint n, GLuint vlist[], GLuint pv )
{
   STARTPROFILE
   POINT *Pts=(POINT *) malloc(n*sizeof(POINT));
   HDC DC=DD_GETDC;
   HPEN Pen;
   HBRUSH Brush;
   HPEN Old_Pen;
   HBRUSH Old_Brush;
   GLint pixel;
   GLuint i;

   if (VB.MonoColor) {
      pixel = Current->pixel;  /* use current color */
   }
   else {
      pixel = RGB(VB.Color[pv][0]*255.0, VB.Color[pv][1]*255.0, VB.Color[pv][2]*255.0);
   }

   Pen=CreatePen(PS_SOLID,1,pixel);
   Brush=CreateSolidBrush(pixel);
   Old_Pen=SelectObject(DC,Pen);
   Old_Brush=SelectObject(DC,Brush);

   for (i=0; i<n; i++) {
      int j = vlist[i];
      Pts[i].x =       (int) VB.Win[j][0];
      Pts[i].y = FLIP( (int) VB.Win[j][1] );
   }

   BEGINGDICALL

   Polygon(DC,Pts,n);
   SelectObject(DC,Old_Pen);
   SelectObject(DC,Old_Brush);
   DeleteObject(Pen);
   DeleteObject(Brush);
   DD_RELEASEDC;
   free(Pts);

   ENDGDICALL

   ENDPROFILE(fast_flat_rgb_polygon)
}



/* Return pointer to accerated polygon function */
static polygon_func choose_polygon_function( void )
{
   STARTPROFILE
   if (!CC.Polygon.SmoothFlag && !CC.Polygon.StippleFlag
       && CC.Light.ShadeModel==GL_FLAT && CC.RasterMask==0
       && !CC.Texture.Enabled && Current->rgb_flag==GL_TRUE) {
   ENDPROFILE(choose_polygon_function)
      return fast_flat_rgb_polygon;
   }
   else {
   ENDPROFILE(choose_polygon_function)
      return NULL;
   }
}



/**********************************************************************/
/*****                 Span-based pixel drawing                   *****/
/**********************************************************************/


/* Write a horizontal span of color-index pixels with a boolean mask. */
static void write_index_span( GLuint n, GLint x, GLint y,
							  const GLuint index[],
                              const GLubyte mask[] )
{
   STARTPROFILE
  GLuint i;
  char *Mem=Current->ScreenMem+y*Current->ScanWidth+x;
  assert(Current->rgb_flag==GL_FALSE);
  for (i=0; i<n; i++)
    if (mask[i])
      Mem[i]=index[i];
   ENDPROFILE(write_index_span)
}



/*
 * Write a horizontal span of pixels with a boolean mask.  The current
 * color index is used for all pixels.
 */
static void write_monoindex_span(GLuint n,GLint x,GLint y,const GLubyte mask[])
{
   STARTPROFILE
  GLuint i;
  char *Mem=Current->ScreenMem+y*Current->ScanWidth+x;
  assert(Current->rgb_flag==GL_FALSE);
  for (i=0; i<n; i++)
    if (mask[i])
      Mem[i]=Current->pixel;
   ENDPROFILE(write_monoindex_span)
}

/*
	To improve the performance of this routine, frob the data into an actual scanline
	and call bitblt on the complete scan line instead of SetPixel.
*/

/* Write a horizontal span of color pixels with a boolean mask. */
static void write_color_span( GLuint n, GLint x, GLint y,
			  const GLubyte
			  red[], const GLubyte green[],
			  const GLubyte blue[], const GLubyte alpha[],
			  const GLubyte mask[] )
{
	STARTPROFILE

	PWMC	pwc = Current;

	if (pwc->rgb_flag==GL_TRUE)
	{
		GLuint i;
		HDC DC=DD_GETDC;
		y=FLIP(y);

		if (mask) {
			for (i=0; i<n; i++)
				if (mask[i])
					wmSetPixel(pwc, y, x + i,red[i], green[i], blue[i]);
		}

		else {
			for (i=0; i<n; i++)
				wmSetPixel(pwc, y, x + i, red[i], green[i], blue[i]);
		}

		DD_RELEASEDC;

	}

  else
  {
    GLuint i;
    char *Mem=Current->ScreenMem+y*Current->ScanWidth+x;
    if (mask) {
       for (i=0; i<n; i++)
         if (mask[i])
           Mem[i]=GetNearestPaletteIndex(Current->hPal,RGB(red[i],green[i],blue[i]));
    }
    else {
       for (i=0; i<n; i++)
         Mem[i]=GetNearestPaletteIndex(Current->hPal,RGB(red[i],green[i],blue[i]));
    }
  }
   ENDPROFILE(write_color_span)

}

/*
 * Write a horizontal span of pixels with a boolean mask.  The current color
 * is used for all pixels.
 */
static void write_monocolor_span( GLuint n, GLint x, GLint y,const GLubyte mask[])
{
   STARTPROFILE
  GLuint i;
  HDC DC=DD_GETDC;
  assert(Current->rgb_flag==GL_TRUE);
  y=FLIP(y);

  if(Current->rgb_flag==GL_TRUE){
	  for (i=0; i<n; i++)
		if (mask[i])
		  wmSetPixel(DC,x+i,y,GetRValue(Current->pixel), GetGValue(Current->pixel), GetBValue(Current->pixel));
  }
  else {
	  for (i=0; i<n; i++)
		if (mask[i])
			SetPixel(DC, y, x+i, Current->pixel);
  }

	DD_RELEASEDC;

	ENDPROFILE(write_monocolor_span)
}



/**********************************************************************/
/*****                   Array-based pixel drawing                *****/
/**********************************************************************/


/* Write an array of pixels with a boolean mask. */
static void write_index_pixels( GLuint n, const GLint x[], const GLint y[],
								const GLuint index[], const GLubyte mask[] )
{
   STARTPROFILE
   GLuint i;
   assert(Current->rgb_flag==GL_FALSE);
   for (i=0; i<n; i++) {
      if (mask[i]) {
         char *Mem=Current->ScreenMem+y[i]*Current->ScanWidth+x[i];
         *Mem = index[i];
      }
   }
   ENDPROFILE(write_index_pixels)
}



/*
 * Write an array of pixels with a boolean mask.  The current color
 * index is used for all pixels.
 */
static void write_monoindex_pixels( GLuint n,
									const GLint x[], const GLint y[],
                                    const GLubyte mask[] )
{
   STARTPROFILE
   GLuint i;
   assert(Current->rgb_flag==GL_FALSE);
   for (i=0; i<n; i++) {
      if (mask[i]) {
         char *Mem=Current->ScreenMem+y[i]*Current->ScanWidth+x[i];
         *Mem = Current->pixel;
      }
   }
   ENDPROFILE(write_monoindex_pixels)
}



/* Write an array of pixels with a boolean mask. */
static void write_color_pixels( GLuint n, const GLint x[], const GLint y[],
								const GLubyte r[], const GLubyte g[],
                                const GLubyte b[], const GLubyte a[],
                                const GLubyte mask[] )
{
	STARTPROFILE
	GLuint i;
	PWMC	pwc = Current;
	HDC DC=DD_GETDC;
	assert(Current->rgb_flag==GL_TRUE);
	for (i=0; i<n; i++)
		if (mask[i])
			wmSetPixel(pwc, FLIP(y[i]),x[i],r[i],g[i],b[i]);
	DD_RELEASEDC;
	ENDPROFILE(write_color_pixels)
}



/*
 * Write an array of pixels with a boolean mask.  The current color
 * is used for all pixels.
 */
static void write_monocolor_pixels( GLuint n,
									const GLint x[], const GLint y[],
                                    const GLubyte mask[] )
{
	STARTPROFILE
	GLuint i;
	PWMC	pwc = Current;
	HDC DC=DD_GETDC;
	assert(Current->rgb_flag==GL_TRUE);
	for (i=0; i<n; i++)
		if (mask[i])
			wmSetPixel(pwc, FLIP(y[i]),x[i],GetRValue(Current->pixel), 
					    GetGValue(Current->pixel), GetBValue(Current->pixel));
	DD_RELEASEDC;
	ENDPROFILE(write_monocolor_pixels)
}



/**********************************************************************/
/*****            Read spans/arrays of pixels                     *****/
/**********************************************************************/


/* Read a horizontal span of color-index pixels. */
static void read_index_span( GLuint n, GLint x, GLint y, GLuint index[])
{
   STARTPROFILE
   GLuint i;
  char *Mem=Current->ScreenMem+y*Current->ScanWidth+x;
  assert(Current->rgb_flag==GL_FALSE);
  for (i=0; i<n; i++)
    index[i]=Mem[i];
   ENDPROFILE(read_index_span)
}




/* Read an array of color index pixels. */
static void read_index_pixels( GLuint n, const GLint x[], const GLint y[],
							   GLuint indx[], const GLubyte mask[] )
{
   STARTPROFILE
   GLuint i;
  assert(Current->rgb_flag==GL_FALSE);
  for (i=0; i<n; i++) {
     if (mask[i]) {
        indx[i]=*(Current->ScreenMem+y[i]*Current->ScanWidth+x[i]);
     }
  }
   ENDPROFILE(read_index_pixels)
}



/* Read a horizontal span of color pixels. */
static void read_color_span( GLuint n, GLint x, GLint y,
							 GLubyte red[], GLubyte green[],
                             GLubyte blue[], GLubyte alpha[] )
{
   STARTPROFILE
  UINT i;
  COLORREF Color;
  HDC DC=DD_GETDC;
  assert(Current->rgb_flag==GL_TRUE);
  y=FLIP(y);
  for (i=0; i<n; i++)
  {
    Color=GetPixel(DC,x+i,y);
    red[i]=GetRValue(Color);
    green[i]=GetGValue(Color);
    blue[i]=GetBValue(Color);
    alpha[i]=255;
  }
  DD_RELEASEDC;
  memset(alpha,0,n*sizeof(GLint));
   ENDPROFILE(read_color_span)
}


/* Read an array of color pixels. */
static void read_color_pixels( GLuint n, const GLint x[], const GLint y[],
							   GLubyte red[], GLubyte green[],
                               GLubyte blue[], GLubyte alpha[],
                               const GLubyte mask[] )
{
   STARTPROFILE
  GLuint i;
  COLORREF Color;
  HDC DC=DD_GETDC;
  assert(Current->rgb_flag==GL_TRUE);
  for (i=0; i<n; i++) {
     if (mask[i]) {
        Color=GetPixel(DC,x[i],FLIP(y[i]));
        red[i]=GetRValue(Color);
        green[i]=GetGValue(Color);
        blue[i]=GetBValue(Color);
        alpha[i]=255;
     }
  }
  DD_RELEASEDC;
  memset(alpha,0,n*sizeof(GLint));
   ENDPROFILE(read_color_pixels)
}



/**********************************************************************/
/**********************************************************************/



void setup_DD_pointers( void )
{
   DD.finish = finish;
   DD.flush = flush;

   DD.clear_index = clear_index;
   DD.clear_color = clear_color;
   DD.clear = clear;

   DD.index = set_index;
   DD.color = set_color;
   DD.index_mask = index_mask;
   DD.color_mask = color_mask;

   DD.logicop = logicop;
   DD.dither = dither;

   DD.set_buffer = set_buffer;
   DD.buffer_size = buffer_size;

   DD.get_points_func = choose_points_function;
   DD.get_line_func = choose_line_function;
   DD.get_polygon_func = choose_polygon_function;

   /* Pixel/span writing functions: */
   DD.write_color_span       = write_color_span;
   DD.write_monocolor_span   = write_monocolor_span;
   DD.write_color_pixels     = write_color_pixels;
   DD.write_monocolor_pixels = write_monocolor_pixels;
   DD.write_index_span       = write_index_span;
   DD.write_monoindex_span   = write_monoindex_span;
   DD.write_index_pixels     = write_index_pixels;
   DD.write_monoindex_pixels = write_monoindex_pixels;

   /* Pixel/span reading functions: */
   DD.read_index_span = read_index_span;
   DD.read_color_span = read_color_span;
   DD.read_index_pixels = read_index_pixels;
   DD.read_color_pixels = read_color_pixels;
}

//
// MesaGL32 is the DLL version of MesaGL for Win32
//

/**********************************************************************/
/*****                  WMesa API Functions                       *****/
/**********************************************************************/



#define PAL_SIZE 256
static void GetPalette(HPALETTE Pal,RGBQUAD *aRGB)
{
   STARTPROFILE
	int i;
	HDC hdc;
	struct
	{
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[PAL_SIZE];
	} Palette =
	{
		0x300,
		PAL_SIZE
	};
	hdc=GetDC(NULL);
	if (Pal!=NULL)
    GetPaletteEntries(Pal,0,PAL_SIZE,Palette.aEntries);
  else
    GetSystemPaletteEntries(hdc,0,PAL_SIZE,Palette.aEntries);
	if (GetSystemPaletteUse(hdc) == SYSPAL_NOSTATIC)
	{
		for(i = 0; i <PAL_SIZE; i++)
			Palette.aEntries[i].peFlags = PC_RESERVED;
		Palette.aEntries[255].peRed = 255;
		Palette.aEntries[255].peGreen = 255;
		Palette.aEntries[255].peBlue = 255;
		Palette.aEntries[255].peFlags = 0;
		Palette.aEntries[0].peRed = 0;
		Palette.aEntries[0].peGreen = 0;
		Palette.aEntries[0].peBlue = 0;
		Palette.aEntries[0].peFlags = 0;
	}
	else
	{
		int nStaticColors;
		int nUsableColors;
		nStaticColors = GetDeviceCaps(hdc, NUMCOLORS)/2;
		for (i=0; i<nStaticColors; i++)
			Palette.aEntries[i].peFlags = 0;
		nUsableColors = PAL_SIZE-nStaticColors;
		for (; i<nUsableColors; i++)
			Palette.aEntries[i].peFlags = PC_RESERVED;
		for (; i<PAL_SIZE-nStaticColors; i++)
			Palette.aEntries[i].peFlags = PC_RESERVED;
		for (i=PAL_SIZE-nStaticColors; i<PAL_SIZE; i++)
			Palette.aEntries[i].peFlags = 0;
	}
	ReleaseDC(NULL,hdc);
  for (i=0; i<PAL_SIZE; i++)
  {
    aRGB[i].rgbRed=Palette.aEntries[i].peRed;
    aRGB[i].rgbGreen=Palette.aEntries[i].peGreen;
    aRGB[i].rgbBlue=Palette.aEntries[i].peBlue;
    aRGB[i].rgbReserved=Palette.aEntries[i].peFlags;
  }
  	  ENDPROFILE(GetPalette)
}


WMesaContext APIENTRY WMesaCreateContext( HWND hWnd, HPALETTE Pal, HDC hDC, GLboolean rgb_flag,
                                 GLboolean db_flag )
{
  BITMAPINFO *Rec;
  HDC DC;
  RECT CR;
  WMesaContext c;

  c = (wmesa_context *) calloc(1,sizeof(wmesa_context));
  if (!c)
    return NULL;

  c->Window=hWnd;

  if (rgb_flag==GL_FALSE)
  {
    c->rgb_flag = GL_FALSE;
    c->pixel = 1;
    db_flag=GL_TRUE; // WinG requires double buffering
    //c->gl_ctx->BufferDepth = windepth;
  }
  else
  {
    c->rgb_flag = GL_TRUE;
    c->pixel = 0;
  }
  GetClientRect(c->Window,&CR);
  c->width=CR.right;
  c->height=CR.bottom;
  if (db_flag)
  {
    c->db_flag = 1;
    /* Double buffered */
    if (c->rgb_flag==GL_TRUE)
    {
      DC = c->hDC = hDC;
	  wmCreateBackingStore(c, c->width, c->height);
    }
    else
    {
      c->dib.hDC=WinGCreateDC();
      Rec=(BITMAPINFO *) malloc(sizeof(BITMAPINFO)+(PAL_SIZE-1)*sizeof(RGBQUAD));
      c->hPal=Pal;
      GetPalette(Pal,Rec->bmiColors);
      WinGRecommendDIBFormat(Rec);
      Rec->bmiHeader.biWidth=c->width;
      Rec->bmiHeader.biHeight*=c->height;
      Rec->bmiHeader.biClrUsed=PAL_SIZE;
      if (Rec->bmiHeader.biPlanes!=1 || Rec->bmiHeader.biBitCount!=8)
      {
        MessageBox(NULL,"Error.","This code presumes a 256 color, single plane, WinG Device.\n",MB_OK);
        exit(1);
      }
      c->Compat_BM=WinGCreateBitmap(c->dib.hDC,Rec,&((void *) c->ScreenMem));
      c->Old_Compat_BM=SelectObject(c->dib.hDC,c->Compat_BM);
      WinGSetDIBColorTable(c->dib.hDC,0,PAL_SIZE,Rec->bmiColors);
      c->IndexFormat=Rec;
      c->ScanWidth=c->width;
      if ((c->ScanWidth%sizeof(long))!=0)
        c->ScanWidth+=(sizeof(long)-(c->ScanWidth%sizeof(long)));
    }
  }
  else
  {
    /* Single Buffered */
	c->db_flag = 0;
	DC = c->hDC = hDC;
	wmCreateBackingStore(c, c->width, c->height);
  }

  /* allocate a new Mesa context */
  c->gl_ctx = gl_new_context( rgb_flag,
                              255.0, 255.0, 255.0, 255.0,
                              db_flag, NULL);

  setup_DD_pointers();

  return c;
}



void APIENTRY WMesaDestroyContext( WMesaContext c )
{
	WC = c;
	gl_destroy_context( c->gl_ctx );

	if (c->db_flag){
		wmDeleteBackingStore(c);
	}
	free( (void *) c );
}



void APIENTRY WMesaMakeCurrent( WMesaContext c )
{
	if(!c){
		Current = c;
		return;
	}
	
	//
	// A little optimization
	// If it already is current,
	// don't set it again
	//
	if(Current == c)
		return;

	gl_set_context( c->gl_ctx );
	Current = c;
	setup_DD_pointers();
	if (Current->gl_ctx->Viewport.Width==0) {
	  /* initialize viewport to window size */
	  gl_viewport( 0, 0, Current->width, Current->height );
	}
}



void APIENTRY WMesaSwapBuffers( void )
{
  HDC DC = Current->hDC;
  if (Current->db_flag)
  {
    if (Current->rgb_flag)
		wmFlush(Current);
    else
      WinGBitBlt(DC,0,0,Current->width,Current->height,Current->dib.hDC,0,0);
  }
}



void APIENTRY WMesaPaletteChange(HPALETTE Pal)
{
  if (Current && Current->rgb_flag==GL_FALSE)
  {
    Current->hPal=Pal;
    GetPalette(Pal,Current->IndexFormat->bmiColors);
    WinGSetDIBColorTable(Current->dib.hDC,0,PAL_SIZE,Current->IndexFormat->bmiColors);
  }
}

//
// Free up the dib section that was created
//
BOOL wmDeleteBackingStore(PWMC pwc)
{
	SelectObject(pwc->dib.hDC, pwc->hOldBitmap);
	DeleteDC(pwc->dib.hDC);
	DeleteObject(pwc->hbmDIB);
	UnmapViewOfFile(pwc->dib.base);
	CloseHandle(pwc->dib.hFileMap);

}


//
// This function creates the DIB section that is used for combined
// GL and GDI calls
//
BOOL WINAPI wmCreateBackingStore(PWMC pwc, long lxSize, long lySize)
{
    HDC hdc = pwc->hDC;
    LPBITMAPINFO pbmi = &(pwc->bmi);
	int		iUsage;

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = lxSize;
    pbmi->bmiHeader.biHeight= -lySize;
    pbmi->bmiHeader.biPlanes = 1;
    pbmi->bmiHeader.biBitCount = GetDeviceCaps(pwc->hDC, BITSPIXEL);
    pbmi->bmiHeader.biCompression = BI_RGB;
    pbmi->bmiHeader.biSizeImage = 0;
    pbmi->bmiHeader.biXPelsPerMeter = 0;
    pbmi->bmiHeader.biYPelsPerMeter = 0;
    pbmi->bmiHeader.biClrUsed = 0;
    pbmi->bmiHeader.biClrImportant = 0;

	iUsage = (pbmi->bmiHeader.biBitCount <= 8) ? DIB_PAL_COLORS : DIB_RGB_COLORS;

	pwc->cColorBits = pbmi->bmiHeader.biBitCount;
	pwc->ScanWidth = lxSize;

	wmCreateDIBSection(hdc, pwc, pbmi, iUsage, (void **)&(pwc->pbPixels));

	if ((iUsage == DIB_PAL_COLORS) && !(pwc->hGLPalette)) {
		wmCreatePalette( pwc );
		wmSetDibColors( pwc );
	}

	return(TRUE);

}


//
// This function copies one scan line in a DIB section to another
//
BOOL WINAPI wmSetDIBits(PWMC pwc, UINT uiScanWidth, UINT uiNumScans, UINT nBypp, UINT uiNewWidth, LPBYTE pBits)
{
	UINT uiScans = 0;
	LPBYTE	pDest = pwc->pbPixels;
	DWORD	dwNextScan = uiScanWidth;
	DWORD	dwNewScan = uiNewWidth;
	DWORD	dwScanWidth = (uiScanWidth * nBypp);

	//
	// We need to round up to the nearest DWORD
	// and multiply by the number of bytes per
	// pixel
	//
	dwNextScan = (((dwNextScan * nBypp)+ 3) & ~3);
	dwNewScan = (((dwNewScan * nBypp)+ 3) & ~3);

	for(uiScans = 0; uiScans < uiNumScans; uiScans++){
		CopyMemory(pDest, pBits, dwScanWidth);
		pBits += dwNextScan;
		pDest += dwNewScan;
	}

	return(TRUE);

}

BOOL WINAPI wmSetPixelFormat( PWMC pwdc, HDC hDC, DWORD dwFlags )
{
	return(TRUE);
}

static unsigned char threeto8[8] = {
	0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

static unsigned char twoto8[4] = {
	0, 0x55, 0xaa, 0xff
};

static unsigned char oneto8[2] = {
	0, 255
};

static unsigned char componentFromIndex(UCHAR i, UINT nbits, UINT shift)
{
	unsigned char val;

	val = i >> shift;
	switch (nbits) {

		case 1:
			val &= 0x1;
			return oneto8[val];

		case 2:
			val &= 0x3;
			return twoto8[val];

		case 3:
			val &= 0x7;
			return threeto8[val];

		default:
			return 0;
	}
}

void WINAPI wmCreatePalette( PWMC pwdc )
{
    /* Create a compressed and re-expanded 3:3:2 palette */
  	BYTE            i;
	LOGPALETTE     *pPal;
    BYTE           rb, rs, gb, gs, bb, bs;

    pwdc->nColors = 0x100;

	pPal = (PLOGPALETTE)malloc(sizeof(LOGPALETTE) + pwdc->nColors * sizeof(PALETTEENTRY));
    memset( pPal, 0, sizeof(LOGPALETTE) + pwdc->nColors * sizeof(PALETTEENTRY) );

	pPal->palVersion = 0x300;

    rb = REDBITS;
    rs = REDSHIFT;
    gb = GREENBITS;
    gs = GREENSHIFT;
    bb = BLUEBITS;
    bs = BLUESHIFT;

    if (pwdc->db_flag) {

        /* Need to make two palettes: one for the screen DC and one for the DIB. */
	    pPal->palNumEntries = pwdc->nColors;
	    for (i = 0; i < pwdc->nColors; i++) {
		    pPal->palPalEntry[i].peRed = componentFromIndex( i, rb, rs );
		    pPal->palPalEntry[i].peGreen = componentFromIndex( i, gb, gs );
		    pPal->palPalEntry[i].peBlue = componentFromIndex( i, bb, bs );
		    pPal->palPalEntry[i].peFlags = 0;
	    }
    	pwdc->hGLPalette = CreatePalette( pPal );
    	pwdc->hPalette = CreatePalette( pPal );
    } 

	else {
	    pPal->palNumEntries = pwdc->nColors;
	    for (i = 0; i < pwdc->nColors; i++) {
		    pPal->palPalEntry[i].peRed = componentFromIndex( i, rb, rs );
		    pPal->palPalEntry[i].peGreen = componentFromIndex( i, gb, gs );
		    pPal->palPalEntry[i].peBlue = componentFromIndex( i, bb, bs );
		    pPal->palPalEntry[i].peFlags = 0;
	    }
    	pwdc->hGLPalette = CreatePalette( pPal );
    }
	
	free(pPal);

}

//
// This function sets the color table of a DIB section
// to match that of the destination DC
//
BOOL WINAPI wmSetDibColors(PWMC pwc)
{
    RGBQUAD			*pColTab, *pRGB;
    PALETTEENTRY	*pPal, *pPE;
    int				i, nColors;
	BOOL			bRet=TRUE;
	DWORD			dwErr=0;

    /* Build a color table in the DIB that maps to the
       selected palette in the DC.
	*/
    nColors = 1 << pwc->cColorBits;
	pPal = (PALETTEENTRY *)malloc( nColors * sizeof(PALETTEENTRY));
    memset( pPal, 0, nColors * sizeof(PALETTEENTRY) );
    GetPaletteEntries( pwc->hGLPalette, 0, nColors, pPal );
    pColTab = (RGBQUAD *)malloc( nColors * sizeof(RGBQUAD));
    for (i = 0, pRGB = pColTab, pPE = pPal; i < nColors; i++, pRGB++, pPE++) {
        pRGB->rgbRed = pPE->peRed;
        pRGB->rgbGreen = pPE->peGreen;
        pRGB->rgbBlue = pPE->peBlue;
    }
	if(pwc->db_flag)
	    bRet = SetDIBColorTable(pwc->hDC, 0, nColors, pColTab );

	if(!bRet)
		dwErr = GetLastError();

    free( pColTab );
    free( pPal );

	return(bRet);
}

void WINAPI wmSetPixel(PWMC pwc, int iScanLine, int iPixel, BYTE r, BYTE g, BYTE b)
{
	LPBYTE	lpb = pwc->pbPixels;
	LPDWORD	lpdw;
	LPWORD	lpw;
	UINT	nBypp = pwc->cColorBits / 8;
	UINT	nOffset = iPixel % nBypp;

	// Move the pixel buffer pointer to the scanline that we
	// want to access

	pwc->dib.fFlushed = FALSE;

	lpb += pwc->ScanWidth * iScanLine;
	// Now move to the desired pixel
	lpb += iPixel * nBypp;

	lpdw = (LPDWORD)lpb;
	lpw = (LPWORD)lpb;

	if(nBypp == 2)
		*lpw = BGR16(r,g,b);
	else if (nBypp == 3){
		*lpdw = BGR24(r,g,b);
	}
	else
		*lpdw = BGR32(r,g,b);

}

void WINAPI wmCreateDIBSection(
	HDC	 hDC,
    PWMC pwc,	// handle of device context
    CONST BITMAPINFO *pbmi,	// address of structure containing bitmap size, format, and color data
    UINT iUsage,	// color data type indicator: RGB values or palette indices
    VOID **ppvBits	// pointer to variable to receive a pointer to the bitmap's bit values
)
{
	DWORD	dwSize = 0;
	DWORD	dwScanWidth;
	UINT	nBypp = pwc->cColorBits / 8;
	HDC		hic;

	dwScanWidth = (((pwc->ScanWidth * nBypp)+ 3) & ~3);

	pwc->ScanWidth = dwScanWidth;

	dwSize = sizeof(BITMAPINFO) + (dwScanWidth * pwc->height);

	pwc->dib.hFileMap = CreateFileMapping((HANDLE)PAGE_FILE,
										  NULL,
										  PAGE_READWRITE | SEC_COMMIT,
										  0,
										  dwSize,
										  NULL);

	if (!pwc->dib.hFileMap)
		return;

	pwc->dib.base = MapViewOfFile(pwc->dib.hFileMap,
								  FILE_MAP_ALL_ACCESS,
								  0,
								  0,
								  0);

	if(!pwc->dib.base){
		CloseHandle(pwc->dib.hFileMap);
		return;
	}

	*ppvBits = ((LPBYTE)pwc->dib.base) + sizeof(BITMAPINFO);

	pwc->dib.hDC = CreateCompatibleDC(hDC);

	CopyMemory(pwc->dib.base, pbmi, sizeof(BITMAPINFO));

	hic = CreateIC("display", NULL, NULL, NULL);

	pwc->hbmDIB = CreateDIBitmap(hic,
						 &(pwc->bmi.bmiHeader),
						 CBM_INIT,
						 pwc->pbPixels,
						 &(pwc->bmi),
						 DIB_RGB_COLORS);

	pwc->hOldBitmap = SelectObject(pwc->dib.hDC, pwc->hbmDIB);

	DeleteDC(hic);

	return;

}

//
// Get bits from memory DC and read into "back buffer"
//
BOOL wmGetBits(PWMC pwc)
{
	int		iRet;

	iRet = GetDIBits(pwc->dib.hDC,
			  pwc->hbmDIB,
			  0,
			  pwc->height,
			  pwc->pbPixels,
			  &(pwc->bmi),
			  DIB_RGB_COLORS);

	return(iRet);

}



//
// Flush the DIBits from memory buffer to the memory DC
// and clear the buffer
//
BOOL wmFlushBits(PWMC pwc)
{
	int		iRet;

	iRet = SetDIBits(pwc->dib.hDC,
			  pwc->hbmDIB,
			  0,
			  pwc->height,
			  pwc->pbPixels,
			  &(pwc->bmi),
			  DIB_RGB_COLORS);

	ZeroMemory(pwc->pbPixels, pwc->ScanWidth * pwc->height);

	return(iRet);

}

//
// Blit memory DC to screen DC
//
BOOL WINAPI wmFlush(PWMC pwc)
{
	BOOL	bRet = 0;
	DWORD	dwErr = 0;


	wmFlushBits(pwc);

	bRet = BitBlt(pwc->hDC, 0, 0, pwc->width, pwc->height, 
		   pwc->dib.hDC, 0, 0, SRCCOPY);

	if(!bRet)
		dwErr = GetLastError();

	pwc->dib.fFlushed = TRUE;

	return(TRUE);

}

/************************************************************
wgl Stubs
************************************************************/
HGLRC WINAPI wglCreateContext(HDC  hdc)
{
	return(NULL);
}

HDC WINAPI wglGetCurrentDC(VOID)
{
	return(NULL);
}

BOOL WINAPI wglDeleteContext(HGLRC  hglrc)
{
	WMesaDestroyContext((WMesaContext)hglrc );
	return(TRUE);
}

HGLRC WINAPI wglGetCurrentContext(VOID)
{
	return((HGLRC)Current);
}

PROC WINAPI wglGetProcAddress(LPCSTR  lpszProc)
{
	return(NULL);
}

BOOL WINAPI wglMakeCurrent(HDC  hdc, HGLRC  hglrc)
{
	WMesaMakeCurrent((WMesaContext)hglrc);
	return(TRUE);
}

BOOL WINAPI wlgMakeShareLists(HGLRC  hglrc1, HGLRC  hglrc2)
{
	return(FALSE);
}

BOOL WINAPI wglUseFontBitmaps(
    HDC  hdc,	//Device context whose font will be used
    DWORD  first,	//Glyph that is the first of a run of glyphs to be turned into bitmap display lists
    DWORD  count,	//Number of glyphs to turn into bitmap display lists
    DWORD  listBase 	//Specifies starting display list
)
{
	return(FALSE);
}

BOOL WINAPI wglUseFontOutlines(
    HDC  hdc,	//Device context of the outline font
    DWORD  first,	//First glyph to be turned into a display list
    DWORD  count,	//Number of glyphs to be turned into display lists
    DWORD  listBase,	//Specifies the starting display list
    FLOAT  deviation,	//Specifies the maximum chordal deviation from the true outlines
    FLOAT  extrusion,	//Extrusion value in the negative z direction
    int  format,	//Specifies line segments or polygons in display lists
    LPGLYPHMETRICSFLOAT  lpgmf 	//Address of buffer to receive glyphs metric data
)
{
	return(FALSE);
}

BOOL WINAPI SwapBuffers(
    HDC  hdc	//Device context whose buffers get swapped
)
{
	WMesaSwapBuffers();
	return(TRUE);
}

