/* WMesaDef.h */


#ifndef WMESADEF_H
#define WMESADEF_H
/*
struct wmesa_context
{
	struct gl_context *gl_ctx;
	HDC Compat_DC;            
	HDC	hDC;
	HBITMAP Old_Compat_BM,Compat_BM;
	GLboolean db_flag;
	GLboolean rgb_flag;
	GLuint depth;
	unsigned long pixel;
	unsigned long clearpixel;
	char *ScreenMem; // WinG memory
	BITMAPINFO *IndexFormat;
	HPALETTE hPal; // Current Palette
};

*/
#define REDBITS		0x03
#define REDSHIFT	0x00
#define GREENBITS	0x03
#define GREENSHIFT	0x03
#define BLUEBITS	0x02
#define BLUESHIFT	0x06


typedef struct _dibSection{
	HDC		hDC;
	HANDLE	hFileMap;
	BOOL	fFlushed;
	LPVOID	base;
}WMDIBSECTION, *PWMDIBSECTION;

typedef struct _wmesa_context{
	struct gl_context *gl_ctx;	/* the main library context */
	HWND				Window;
    HDC                 hDC;
    HPALETTE            hPalette;
    HPALETTE            hOldPalette;
    HPEN                hPen;
    HPEN                hOldPen;
    HCURSOR             hOldCursor;
    COLORREF            crColor;
    /* 3D projection stuff */
    RECT                drawRect;
    UINT                uiDIBoffset;
    /* OpenGL stuff */
    HPALETTE            hGLPalette;
	GLuint				width;
	GLuint				height;
	GLuint				ScanWidth;
	GLboolean			db_flag;	/* double buffered? */
	GLboolean			rgb_flag;	/* RGB mode? */
	GLuint				depth;		/* bits per pixel (1, 8, 24, etc) */
	ULONG				pixel;	/* current color index or RGBA pixel value */
	ULONG				clearpixel; /* pixel for clearing the color buffers */
	PSTR				ScreenMem; // WinG memory
	BITMAPINFO			*IndexFormat;
	HPALETTE			hPal; // Current Palette
    //PWINDOWSTRUCT     stuff
    BITMAPINFO          bmi;
    HBITMAP             hbmDIB;
    HBITMAP             hOldBitmap;
	HBITMAP				Old_Compat_BM;
	HBITMAP				Compat_BM;            /* Bitmap for double buffering */
    PBYTE               pbPixels;
    int                 nColors;
	BYTE				cColorBits;
	WMDIBSECTION		dib;
#ifdef PROFILE
	MESAPROF	profile;
#endif
}wmesa_context, *PWMC;


#define PAGE_FILE		0xffffffff


char ColorMap16[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,
0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,
0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,
0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,
0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,
0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,
0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,
0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,
0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,
0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,
0x17,0x17,0x17,0x17,0x17,0x17,0x17,0x17,
0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,
0x1A,0x1A,0x1A,0x1A,0x1A,0x1A,0x1A,0x1A,
0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,
0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,
0x1D,0x1D,0x1D,0x1D,0x1D,0x1D,0x1D,0x1D,
0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,
0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F};


#define BGR16(r,g,b)	((WORD)(((BYTE)(ColorMap16[b]) | ((BYTE)(ColorMap16[g]) << 5)) | (((WORD)(BYTE)(ColorMap16[r])) << 10)))
#define BGR24(r,g,b)	(((DWORD)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16))) << 8)
#define BGR32(r,g,b)	((DWORD)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))

#endif