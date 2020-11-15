/*
   file: clgd5472.c
   auth: Peter McDermott
   date: Mon Feb 12 16:49:41 CST 1996
*/


#include "mondello/clgd5471.h"
#include "mondello/clgd547x.h"
#include "mondello/compiler.h"     /* outb(), inb() */
#include "mondello/clgd5472.h"
#include <math.h>         /* fabs() in clgd5472CalcClockVals */

/* #define DEBUG_CLGD5472 */

/* reference clock */
#define clgd5472_REF_CLOCK 14.31818E6
#define clgd5472_MAX_CLOCK_IN_KHZ 135000

/* overlay registers */
#define clgd5472_HOSTART 0x12
#define clgd5472_HOEND   0x14
#define clgd5472_VOSTART 0x22
#define clgd5472_VOEND   0x24

/* horizontal display timing registers */
#define clgd5472_HTOTAL  0x10
#define clgd5472_HDEND   0x16
#define clgd5472_HSSTART 0x18
#define clgd5472_HSEND   0x1A
#define clgd5472_HBSTART 0x68
#define clgd5472_HBEND   0x6A

/* vertical display timing registers */
#define clgd5472_VTOTAL  0x20
#define clgd5472_VDEND   0x26
#define clgd5472_VSSTART 0x28
#define clgd5472_VSEND   0x2A
#define clgd5472_VBSTART 0x78
#define clgd5472_VBEND   0x7A

/* various and sundry display timing registers */
#define clgd5472_CRTC         0x50
#define clgd5472_PIXELFMT     0x51
#define clgd5472_SYNC         0x53
#define clgd5472_PCLKNUM      0x57
#define clgd5472_PCLKDEN      0x58
#define clgd5472_MONDELLOMODE 0x70
#define clgd5472_TEST         0xF3  /* used to select internal or ref clk */

/* color control registers */
#define clgd5472_PDOWN           0x02
#define clgd5472_DAC_WRITE_INDEX 0x60
#define clgd5472_DAC_READ_INDEX  0x62
#define clgd5472_DAC_PIXEL_MASK  0x64
#define clgd5472_DAC_DATA        0x65

typedef struct {
  unsigned long htotal;
  unsigned long hdend;
  unsigned long hsstart;
  unsigned long hsend;
  unsigned long vtotal;
  unsigned long vdend;
  unsigned long vsstart;
  unsigned long vsend;
  unsigned long crtc;
  unsigned long pixelfmt;
  unsigned long sync;
  unsigned long pclknum;
  unsigned long pclkden;
  unsigned long hbstart;
  unsigned long hbend;
  unsigned long mondellomode;
  unsigned long vbstart;
  unsigned long vbend;
} clgd5472CRTCRegs, *clgd5472CRTCRegPtr;

clgd5472CRTCRegs regs;

void outDacB(int address, BYTE data);
void outDacW(int address, int data);
int inDacB(int address);

/* default palette values */
static const unsigned char default_red[256]
=
{0, 0, 0, 0, 42, 42, 42, 42, 21, 21, 21, 21, 63, 63, 63, 63,
 0, 5, 8, 11, 14, 17, 20, 24, 28, 32, 36, 40, 45, 50, 56, 63,
 0, 16, 31, 47, 63, 63, 63, 63, 63, 63, 63, 63, 63, 47, 31, 16,
 0, 0, 0, 0, 0, 0, 0, 0, 31, 39, 47, 55, 63, 63, 63, 63,
 63, 63, 63, 63, 63, 55, 47, 39, 31, 31, 31, 31, 31, 31, 31, 31,
 45, 49, 54, 58, 63, 63, 63, 63, 63, 63, 63, 63, 63, 58, 54, 49,
 45, 45, 45, 45, 45, 45, 45, 45, 0, 7, 14, 21, 28, 28, 28, 28,
 28, 28, 28, 28, 28, 21, 14, 7, 0, 0, 0, 0, 0, 0, 0, 0,
 14, 17, 21, 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 24, 21, 17,
 14, 14, 14, 14, 14, 14, 14, 14, 20, 22, 24, 26, 28, 28, 28, 28,
 28, 28, 28, 28, 28, 26, 24, 22, 20, 20, 20, 20, 20, 20, 20, 20,
 0, 4, 8, 12, 16, 16, 16, 16, 16, 16, 16, 16, 16, 12, 8, 4,
 0, 0, 0, 0, 0, 0, 0, 0, 8, 10, 12, 14, 16, 16, 16, 16,
 16, 16, 16, 16, 16, 14, 12, 10, 8, 8, 8, 8, 8, 8, 8, 8,
 11, 12, 13, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 15, 13, 12,
 11, 11, 11, 11, 11, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0};
static const unsigned char default_green[256]
=
{0, 0, 42, 42, 0, 0, 21, 42, 21, 21, 63, 63, 21, 21, 63, 63,
 0, 5, 8, 11, 14, 17, 20, 24, 28, 32, 36, 40, 45, 50, 56, 63,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 31, 47, 63, 63, 63, 63,
 63, 63, 63, 63, 63, 47, 31, 16, 31, 31, 31, 31, 31, 31, 31, 31,
 31, 39, 47, 55, 63, 63, 63, 63, 63, 63, 63, 63, 63, 55, 47, 39,
 45, 45, 45, 45, 45, 45, 45, 45, 45, 49, 54, 58, 63, 63, 63, 63,
 63, 63, 63, 63, 63, 58, 54, 49, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 7, 14, 21, 29, 28, 28, 28, 28, 28, 28, 28, 28, 21, 14, 7,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 17, 21, 24, 28, 28, 28, 28,
 28, 28, 28, 28, 28, 24, 21, 17, 20, 20, 20, 20, 20, 20, 20, 20,
 20, 22, 24, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 26, 24, 22,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 8, 12, 16, 16, 16, 16,
 16, 16, 16, 16, 16, 12, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 10, 12, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 12, 10,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 13, 15, 16, 16, 16, 16,
 16, 16, 16, 16, 16, 15, 13, 12, 0, 0, 0, 0, 0, 0, 0, 0};
static const unsigned char default_blue[256]
=
{0, 42, 0, 42, 0, 42, 0, 42, 21, 63, 21, 63, 21, 63, 21, 63,
 0, 5, 8, 11, 14, 17, 20, 24, 28, 32, 36, 40, 45, 50, 56, 63,
 63, 63, 63, 63, 63, 47, 31, 16, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 16, 31, 47, 63, 63, 63, 63, 63, 63, 63, 63, 63, 55, 47, 39,
 31, 31, 31, 31, 31, 31, 31, 31, 31, 39, 47, 55, 63, 63, 63, 63,
 63, 63, 63, 63, 63, 58, 54, 49, 45, 45, 45, 45, 45, 45, 45, 45,
 45, 49, 54, 58, 63, 63, 63, 63, 28, 28, 28, 28, 28, 21, 14, 7,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 14, 21, 28, 28, 28, 28,
 28, 28, 28, 28, 28, 24, 21, 17, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 17, 21, 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 26, 24, 22,
 20, 20, 20, 20, 20, 20, 20, 20, 20, 22, 24, 26, 28, 28, 28, 28,
 16, 16, 16, 16, 16, 12, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 4, 8, 12, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 12, 10,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 10, 12, 14, 16, 16, 16, 16,
 16, 16, 16, 16, 16, 15, 13, 12, 11, 11, 11, 11, 11, 11, 11, 11,
 11, 12, 13, 15, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0};


/*----------------------------------------------------------------------------
  outDacB(addr,data) - send a byte to the 5472 DAC at address addr
-----------------------------------------------------------------------------*/
void outDacB(int address, BYTE data)
{
  int i;
  iopl(3);

  for (i=0; i<10; i++)
    outb(0x80,0xaa);

  *((BYTE *)(clgd547xLogicalBase+0x3ffe00+address))=data;

  for (i=0; i<10; i++)
    outb(0x80,0xaa);
  
}

/*----------------------------------------------------------------------------
  outDacW(addr,data) send a word to the 5472 DAC at address addr
-----------------------------------------------------------------------------*/
void outDacW(int address, int data)
{
  int i;
 
  iopl(3);
  for (i=0; i<10; i++)
    outb(0x80,0xaa);
  
  *((char *)((unsigned)clgd547xLogicalBase+0x3ffe00+address))=data & 0x00ff;
  *((char *)((unsigned)clgd547xLogicalBase+0x3ffe00+address+1))=
    (data >> 8)&0xff;

  for (i=0; i<10; i++)
    outb(0x80,0xaa);
  
}

/*----------------------------------------------------------------------------
  inDacB(addr) read a byte from the 5472 DAC at address addr
-----------------------------------------------------------------------------*/
int inDacB(int address)
{
  int i;
  char c;

  iopl(3);

  for (i=0; i<10; i++)
    outb(0x80,0xaa);

  c=*((char *)((unsigned)clgd547xLogicalBase+0x3ffe00+address));
  
  return ((int)c)&0xff;  
}

/*---------------------------------------------------------------------------
  clgd5472InitLUT()
  Loads the Look-Up Table with all black. 
-----------------------------------------------------------------------------*/
void clgd5472InitLUT(void)
{
    unsigned int i;

    outDacB(clgd5472_DAC_PIXEL_MASK,0xff);  /* get rid of cmap mask */
    outDacB(clgd5472_DAC_WRITE_INDEX, 0);   /* set the write index to 0 */
    iopl(3);
    for (i = 0; i < 256; i++) { /* clear LUT */
      outb(0x80,0xaa);
      outb(0x80,0xaa);
      outb(0x80,0xaa);
      outb(0x80,0xaa);
      outb(0x80,0xaa);
      outDacB(clgd5472_DAC_DATA,default_red[i]);
      outDacB(clgd5472_DAC_DATA,default_green[i]);
      outDacB(clgd5472_DAC_DATA,default_blue[i]);
    }
    iopl(0);
}

/*---------------------------------------------------------------------------
  clgd5472WriteIndex()
  Writes r,g,b tuple to given index of LUT
-----------------------------------------------------------------------------*/
void clgd5472WriteIndex(uint index, uint red, uint green, uint blue)
{
    unsigned int i;

#ifdef DEBUG_CLGD5472
    printf("clgd5472WriteIndex() - writing index %d=(%d,%d,%d)\n",index,red,green,blue);
#endif
     
    outDacB(clgd5472_DAC_PIXEL_MASK,0xff);  /* get rid of cmap mask */
    outDacB(clgd5472_DAC_WRITE_INDEX,index);   /* set the write index to 0 */
    iopl(3);
    outb(0x80,0xaa);
    outb(0x80,0xaa);
    outb(0x80,0xaa);
    outb(0x80,0xaa);
    outb(0x80,0xaa);
    outDacB(clgd5472_DAC_DATA, red);
    outDacB(clgd5472_DAC_DATA, green);
    outDacB(clgd5472_DAC_DATA, blue);
    iopl(0);
}


/*-----------------------------------------------------------------------------
  CalcClockVals - calculate the proper clock numerator, denominator and 
                 post scalar given a clock frequency.
-----------------------------------------------------------------------------*/
void clgd5472CalcClockVals(long freq, int *inP, int *inQ, int *inPostScale)
{
  int p,q,p2,q2;
  double calcError;
  double calcError2;
  float pDivQ;

  if (freq > 135E6) {   /* frequency out of range */
    printf("%d clock too high\n",freq);
    return;
  }

  if (freq < 40E6) {    /* need to apply post scalar? */
    *inPostScale=2;
    freq*=2;
  }
  else {
    *inPostScale=1;
  }

  calcError2=100000E6;     /* initial error */

  pDivQ=freq/clgd5472_REF_CLOCK;
  for(q=1;q<32;q++) {
    if ((clgd5472_REF_CLOCK/q >= 200E3) && (clgd5472_REF_CLOCK/q <= 1E6)) {
      for(p=q;p<127;p++) {
        calcError=fabs(pDivQ-(p/q));
        if (calcError<calcError2) {
          calcError2=calcError;
          q2=q;
          p2=p;
        }
      }
    }
  }   
  *inP=p2;
  *inQ=q2;   
}

static short clgd5472Inited = FALSE;

/*-----------------------------------------------------------------------------
  clgd5472CalcCRTCRegs - Calculates 5472 parameters for a given mode
-----------------------------------------------------------------------------*/

void clgd5472CalcCRTCRegs(clgd5472CRTCRegs *crtcRegs, mode *m)
{
  int p,q,postScale;
  char outstr[128];
  
#ifdef DEBUG
  printModeInfo(mode);
#endif

  crtcRegs->hdend=crtcRegs->hbstart= m->CrtcHDisplay/8;
  crtcRegs->hbend=crtcRegs->htotal=  m->CrtcHTotal/8;
  crtcRegs->hsstart=                 m->CrtcHSyncStart/8;
  crtcRegs->hsend=                   m->CrtcHSyncEnd/8;

#ifdef DEBUG
  printf("CalcCRTCRegs: register values:\n");
  sprintf(outstr,"CalcCRTCRegs:   hdend, hbstart: %d\n",crtcRegs->hbstart);
  printf(outstr);
  sprintf(outstr,"CalcCRTCRegs:   hbend, htotal:  %d\n",crtcRegs->hbend);
  printf(outstr);
  sprintf(outstr,"CalcCRTCRegs:   hsstart:        %d\n",crtcRegs->hsstart);
  printf(outstr);
  sprintf(outstr,"CalcCRTCRegs:   hsend:          %d\n",crtcRegs->hsend);
  printf(outstr);
#endif

  if (m->Flags & V_INTERLACE) {
    crtcRegs->crtc=11;
    crtcRegs->vtotal=crtcRegs->vbend=  (m->CrtcVTotal >> 1)-1;
    crtcRegs->vbstart=crtcRegs->vdend= (m->CrtcVDisplay >> 1)-1;
    crtcRegs->vsstart=                 (m->CrtcVSyncStart >> 1)-1;
    crtcRegs->vsend=                   (m->CrtcVSyncEnd >> 1)-1;
  }
  else {
    crtcRegs->crtc=3;
    crtcRegs->vtotal=crtcRegs->vbend=  m->CrtcVTotal;
    crtcRegs->vbstart=crtcRegs->vdend= m->CrtcVDisplay;
    crtcRegs->vsstart=                 m->CrtcVSyncStart;
    crtcRegs->vsend=                   m->CrtcVSyncEnd;
  }

#ifdef DEBUG
  sprintf(outstr,"CalcCRTCRegs:   crtc:           %d\n",crtcRegs->crtc);
  printf(outstr);
  sprintf(outstr,"CalcCRTCRegs:   vbend, vtotal:  %d\n",crtcRegs->vbend);
  printf(outstr);
  sprintf(outstr,"CalcCRTCRegs:   vbstart, vdend: %d\n",crtcRegs->vbstart);
  printf(outstr);
  sprintf(outstr,"CalcCRTCRegs:   vsstart:        %d\n",crtcRegs->vsstart);
  printf(outstr);
  sprintf(outstr,"CalcCRTCRegs:   vsend:          %d\n",crtcRegs->vsend);
  printf(outstr);
#endif

  switch (m->bitsPerPixel) {
  case 8: 
    crtcRegs->pixelfmt=3;
    crtcRegs->mondellomode=0x10;
    break;   
  case 24:
    crtcRegs->pixelfmt=8;
    crtcRegs->mondellomode=0x00;
    break; 
  case 32: /* not supported */
    crtcRegs->pixelfmt=12;
    crtcRegs->mondellomode=0x16;
    break;
  }

#ifdef DEBUG
  sprintf(outstr,"CalcCRTCRegs:   pixelfmt:       %d\n",crtcRegs->pixelfmt);
  printf(outstr);
#endif

  if (m->Flags & V_NHSYNC) 
    crtcRegs->sync=12;
  else 
    crtcRegs->sync=76;

  if (m->Flags & V_NVSYNC) 
    crtcRegs->sync|=0;
  else 
    crtcRegs->sync|=128;

#ifdef DEBUG
  sprintf(outstr,"CalcCRTCRegs:   sync:           %d\n",crtcRegs->sync);
  printf(outstr);
#endif

  clgd5472CalcClockVals(m->Clock*1000*1000,&p,&q,&postScale);

  crtcRegs->pclknum = p << 1;
  crtcRegs->pclkden = (postScale==2)?(0x01|(q<<3)) : (q<<3);

#ifdef DEBUG
  sprintf(outstr,"CalcCRTCRegs:   pclknum:        %d\n",crtcRegs->pclknum);
  printf(outstr);
  sprintf(outstr,"CalcCRTCRegs:   pclkden:        %d\n",crtcRegs->pclkden);
  printf(outstr);
#endif

}

/*-----------------------------------------------------------------------------
  setCsrSize - sets the size of the cursor (0x0 to 64x64)
-----------------------------------------------------------------------------*/
void clgd5472SetCsrSize(int xSize, int ySize)
{
  outDacB(0x32,(char)xSize & 0x3f);
  outDacB(0x35,(char)ySize & 0x3f);
}

/*-----------------------------------------------------------------------------
  setCsrPos - sets the position of the cursor
-----------------------------------------------------------------------------*/
void clgd5472SetCsrPos(int xPos, int yPos) 
{
  outDacW(0x30,xPos & 0x0fff);
  outDacW(0x33,yPos & 0x0fff);
}

/*-----------------------------------------------------------------------------
 clgd5472SetCRTCRegs --
      Initializes the clgd5472 for the currently selected CRTC parameters.
-----------------------------------------------------------------------------*/
void clgd5472SetCRTCRegs(crtcRegs)
     clgd5472CRTCRegPtr crtcRegs;
{
  int i;

  iopl(3);
  
  printf("clgd5472SetCRTCRegs: setting up 5471\n");
  outSeq(0x2101); /* stop '71 screen refresh to DRAM, 8 dot clock */
  outSeq(0x0f02); /* enable all maps for writing */
  outSeq(0x0003); /* pri font = 2nd font = @ 0K offset in RAM */
  outSeq(0x0804); /* select no Chain-4, no Odd/Even, 64K window in ram mode */
  outSeq(0x1206); /* unlock extended regs */
  outSeq(0x1107); /* map high memory in */
  outSeq(0x100f); /* select 32 bit dram bus width  */
  outSeq(0x321e); /* bit 0=0, Vlck no div by 2 */
  
  i=33;
  i=(int)(((float)(i*8)/14.318)+0.5);
  i=i<<8 | 0x401f;
  outw(0x3c4,i);
  
  outGraph(0x0005); /* write mode 0, 2/4/16 color mode, vid shift EGA */
  outGraph(0x0506); /* graphics (!text), map 64K at A000:0000 */
  outGraph(0x0f07); /* set all planes to active for color compare */
  outGraph(0x8017); /* ?? */
  outGraph(0x8038); /* enable extended Mondello mode */

  printf("clgd5472SetCRTCRegs: setting up 5472\n");

  inb (0x3c6);
  inb (0x3c6);
  inb (0x3c6);
  inb (0x3c6);
  outb (0x3c6, 0xc6);

  outDacB(0xf3,0x80); /* use external clock */
  outDacB(0x00,0x04); /* put 5472 into extended mode */
  outDacB(0x02,0x00); /* normal DAC operation */

  printf("SetCRTCRegs: seting overlay regs\n");
  outDacW(clgd5472_HOSTART,0);
  outDacW(clgd5472_HOEND,0);
  outDacW(clgd5472_VOSTART,0);
  outDacW(clgd5472_VOEND,0);

  printf("SetCRTCRegs: setting horizontals\n");
  outDacW(clgd5472_HTOTAL,crtcRegs->htotal);
  outDacW(clgd5472_HDEND,crtcRegs->hdend);
  outDacW(clgd5472_HSSTART,crtcRegs->hsstart);
  outDacB(clgd5472_HSEND,crtcRegs->hsend);
  outDacW(clgd5472_HBSTART,crtcRegs->hbstart);
  outDacB(clgd5472_HBEND,crtcRegs->hbend);

  printf("SetCRTCRegs: setting verticals\n");
  outDacW(clgd5472_VTOTAL,crtcRegs->vtotal);
  outDacW(clgd5472_VDEND,crtcRegs->vdend);
  outDacW(clgd5472_VSSTART,crtcRegs->vsstart);
  outDacB(clgd5472_VSEND,crtcRegs->vsend);
  outDacW(clgd5472_VBSTART,crtcRegs->vbstart);
  outDacB(clgd5472_VBEND,crtcRegs->vbend);

  printf("SetCRTCRegs: setting misc\n");
  outDacB(0x64,0xff); /* palette mask reg */

  outDacB(0x66,0x08); /* set palette access mode to allow access to csr clrs */

  outDacB(0x60,0x00); /* access cursor background color */
  for(i=0;i<10;i++)
    outb(0x80,0xaa);
  outDacB(0x65,0x00); /* set cursor background color to black */
  outDacB(0x65,0x00);
  outDacB(0x65,0x00);

  outDacB(0x60,0x0f); /* access cursor foreground color */
  for(i=0;i<10;i++)
    outb(0x80,0xaa);
  outDacB(0x65,0xff);  /* set csr foreground color to white */
  outDacB(0x65,0xff);
  outDacB(0x65,0xff);

  outDacB(0x66,0x00);  /* set palette access mode back to normal */

  outDacB(0x37,0x00);  /* set csr size to 64x64 */
  clgd5472SetCsrSize(10,10);
  clgd5472SetCsrPos(128,128);
  outDacB(0x3c,0x00); /* Overlay control */
  outDacB(0x4c,0x00); /* Color Key */
/*  outDacB(0x70,0x10); */ /* select first of 2 buffers */  
/*  outDacB(0x71,0x01); */ /* inconsitency between Goran's code and book! */

  outDacB(clgd5472_CRTC,crtcRegs->crtc);
  outDacB(clgd5472_PIXELFMT,crtcRegs->pixelfmt);
  outDacB(clgd5472_SYNC,crtcRegs->sync);
  outDacB(clgd5472_PCLKNUM,crtcRegs->pclknum);
  outDacB(clgd5472_PCLKDEN,crtcRegs->pclkden);
  outDacB(clgd5472_MONDELLOMODE,crtcRegs->mondellomode);
  outDacB(0x71,0x01);
  outDacB(clgd5472_TEST,0x00); /* use internal clock */ 
  outDacB(0xf0,0x00); /* why is this done?! */
  outDacB(0x66,0x00); /* palette state */
  outDacB(0x64,0xff); /* palette pixel mask */
 
  clgd5472Inited = TRUE;
  
  iopl(0);
}

void clgd5472SetMode(mode *m) 
{ 
  clgd5472CalcCRTCRegs(&regs,m);
  clgd5472SetCRTCRegs(&regs);
}

clgd5472State *clgd5472CreateState()
{
  return (clgd5472State*)malloc(sizeof(clgd5472State));  
}

void clgd5472DeleteState(clgd5472State *state)
{
  free(state);
}

void clgd5472SaveState(clgd5472State *state)
{
}

void clgd5472RestoreState(clgd5472State *state)
{
  /* turn off Mondello Mode */
  iopl(3);
  outDacB(0x00,0x00);
  iopl(0);
}

void setBuffer(int buffer)
{
  clgd547x_setBuffer(buffer);
} 
