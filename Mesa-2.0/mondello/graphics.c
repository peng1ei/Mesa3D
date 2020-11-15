/* 
  file: graphics.c
*/

#include <stdio.h>
#include <string.h>  /* memcpy() in tritemp.h */

#include "mondello/graphics.h"
#include "mondello/davinci.h"
#include "mondello/clgd5470.h"
#include "mondello/clgd547x.h"
#include "mondello/compiler.h" /* inb(), outb() */
#include "mondello/type.h"

#include "../src/vb.h"
#include "../src/config.h"

/* uncomment the following to insert debugging code */
/*  #define DEBUG_clgd547x */

/* uncomment the following to cause each primitive to be immediately
 * executed.  This is useful for debugging and significantly slows
 * down execution as it busy-waits for each instruction to finish 
 */
/* #define IMMEDIATE_EXECUTION */

/* uncomment the following to have bypass actually executing the 
 * instruction and instead have information about what would have
 * be done dumped to the screen (this only works for polygons)
 */
/* #define DUMP_ON_EXEC */

#define COMPILE_MESA_STUBS  

#define BUSY (!(inWarp(0x24)&0x80000000))

#define EXEC								\
        { *listPtr++=IDLE;						\
	  *listPtr++=IDLE;						\
	  *listPtr++=IDLE;						\
	  outWarp(HOST_INSTRUCTION_REG,branch_ro(0x00000000));		\
	  while(BUSY);							\
	  listPtr=clgd5470LogicalBase;					\
	}

#define EXEC2 { while(BUSY);						\
	        *listPtr++=IDLE;					\
	        *listPtr++=IDLE;					\
	        *listPtr++=IDLE;					\
                if (listNum==0) {					\
                  outWarp(HOST_INSTRUCTION_REG,branch_ro(0x00000000));	\
                  listPtr=listPtr1;					\
                  listMax=listMax1;					\
                  listNum=1;						\
                }							\
                else {							\
                  outWarp(HOST_INSTRUCTION_REG,branch_ro(0x00080000));	\
                  listPtr=listPtr0;					\
                  listMax=listMax0;					\
                  listNum=0;						\
                }							\
              }

#ifdef DUMP_ON_EXEC
#  define CHKEXECPOLY dumpContents();
#else
#  define CHKEXECPOLY CHKEXEC
#endif

#ifdef IMMEDIATE_EXECUTION
#  ifndef CHKEXEC
#    define CHKEXEC EXEC
#  endif
#endif

#ifndef CHKEXEC
#  define CHKEXEC { if (listPtr >= listMax) { 				\
                    EXEC2;						\
                  }							\
                }
#endif

#define SPINWAIT { while(BUSY); }                  
#define SPINWARP EXEC SPINWAIT

#define FRACT 12
#define ONESLOPE 1<<FRACT

#define ASL(x,count) (x<<count)

#define FN(z) (( (-z) & 0x7fff ) >> 1)

uint cx1, cy1, cx2, cy2;
uint zlev=0;
uint yoffs=0;

uint flipHeight=1023;
  
/**************** UTILITY FUNCTIONS ******************/

/* used in tritemp.h for flipping y's */
#define FLIP(Y) (flipHeight-(Y))

void setMondelloHeight(int h)
{
  flipHeight = h-1;
}

#ifdef DUMP_ON_EXEC
void dumpContents()
{
  uint i;
  for(i=0; i<16; i++) {
    printf("R%2d: %8X\n",i+1,*(clgd5470LogicalBase+2+i));
  }
  /* listPtr=clgd5470LogicalBase; */
}
#endif

void clgd547x_flush()
{
#ifdef DEBUG_clgd547x
  printf("clgd547x_flush()\n");
#endif
  EXEC;
}

void clgd547x_finish()
{
#ifdef DEBUG_clgd547x
  printf("clgd547x_finish()\n");
#endif
  EXEC;
}

void clgd547x_setBuffer(int buffer)            /* 0=front, 1=back */
{
  int i;
  
#ifdef DEBUG_clgd547x
  printf("clgd547x_setBuffer(%d)\n",buffer);
#endif

  *listPtr++ = SNOP;
  *listPtr++ = SNOP;
  for(i=0; i<22; i++) {
    *listPtr++ = IDLE;
  }

  EXEC;
  SPINWAIT;
    
  outWarp(MASK_B_REG, ~0x02000000 );

  if (buffer==0) {
    outWarp(REF0_REG,0x00800000);
    yoffs=512;
  }
  else { 
    outWarp(REF0_REG,0x00A00000);
    yoffs=0;    
  }
  
  outWarp(MASK_B_REG, 0xf7ffffff );
  outWarp(MASK_REG, 0xffffffff );  

}

/******************* Area Blit Functions ********************/
#define BLIT_VARS				\
  int r1, r2;					\
  int y_count = y2-y1;

#define BLIT_CODE_1				\
/*  listPtr=clgd5470LogicalBase; */	 	\
  r1=(y_count >> 8) << 24 | (y1+yoffs) << FRACT;\
  r2=y_count<<24 | 0x1000;

#define BLIT_CODE_2				\
  *listPtr++=r1;				\
  *listPtr++=r2;				\
  *listPtr++=x1 << FRACT;			\
  *listPtr++=(x2-x1) << FRACT;


void clgd547x_clearDepthBuffer()
{
  uint x1=0;
  uint y1=0;
  uint x2=clgd547xStateInfo->width;
  uint y2=clgd547xStateInfo->height;
  BLIT_VARS

#ifdef DEBUG_clgd547x
  printf("clgd547x_clearDepthBuffer\n");
#endif
  
  BLIT_CODE_1 

  *listPtr++=SNOP;  
  *listPtr++=load_long_ro(C_AND_A_REG,1);  /* load long */
  *listPtr++=0xffff0000;           
  *listPtr++=load_long_ro(MASK_B_REG, 1);
  *listPtr++=0xf7ffffff;
  *listPtr++=load_short_ro(BANK_MASK_REG, RED_BANK|GREEN_BANK|BLUE_BANK);
  *listPtr++=SNOP;
  *listPtr++=blit_ro(BLTFZ,PRAMPAT+PATA_PATB+PRAMALF,A_ONLY); 
  
  BLIT_CODE_2

  CHKEXEC;
  
}

void clearArea8_2c(uint x1, uint y1, uint x2, uint y2, uint cindex)
{
  BLIT_VARS

#ifdef DEBUG_clgd547x
  printf("clearArea8_2c(%d,%d,%d,%d,%d)\n",x1,y1,x2,y2,cindex);
#endif

  BLIT_CODE_1 

  cindex &= 0x000000ff;
  
  *listPtr++=SNOP;
  *listPtr++=load_long_ro(C_AND_A_REG,1);
  *listPtr++=0xffff0000 | cindex << 8 | cindex;                  
  *listPtr++=load_long_ro(MASK_B_REG, 1);
  *listPtr++=0xf7ffffff;
  *listPtr++=load_short_ro(BANK_MASK_REG, BLUE_BANK );  
  *listPtr++=SNOP;
  *listPtr++=blit_ro(BLTF,PRAMPAT+PATA_PATB+PRAMALF, A_ONLY); 

  BLIT_CODE_2

  CHKEXEC;
}

void clearArea8_3c(uint x1, uint y1, uint x2, uint y2, uint z, uint cindex)
{
  BLIT_VARS

#ifdef DEBUG_clgd547x
  printf("clearArea8_3c(%d,%d,%d,%d,%d,%d)\n",x1,y1,x2,y2,z,cindex);
#endif
  
  BLIT_CODE_1 

  cindex &= 0x000000ff;
  
  *listPtr++=load_short_ro(BANK_MASK_REG, BLUE_BANK );  
  *listPtr++=load_long_ro(C_AND_A_REG,1);  /* load long */
  *listPtr++=0xffff0000 | cindex << 8 | cindex;                  
  *listPtr++=blit_ro(BLTF,PRAMPAT+PATA_PATB+PRAMALF, A_ONLY); 

  BLIT_CODE_2

  /* TOTEST:
  
  *listPtr++=load_long_ro(C_AND_A_REG,1);  
  *listPtr++=0xffff0000 | z;                  
  *listPtr++=(blit_ro(BLTZ,PRAMPAT+PATA_PATB+PRAMALF, A_ONLY) & ~0x0f);     

  */

  CHKEXEC;
  
}

void clear8c(uint cindex)
{
#ifdef DEBUG_clgd547x
  printf("clear8(%d)\n",cindex);
#endif
  clearArea8_2c(0,0,clgd547xStateInfo->width,clgd547xStateInfo->height,cindex);
}

void clearArea24_2c(uint cx1, uint cy1, uint cx2, uint cy2,
		    uint r, uint g, uint b, uint a)
#define RGBA
#include "clrtemp.h"

void clearArea24_3c(uint cx1, uint cy1, uint cx2, uint cy2, uint cz,
		    uint r, uint g, uint b, uint a)
#define RGBA
#include "clrtemp.h"

void clear24(uint r, uint g, uint b)
#define SETUP 				\
	cx1=cy1=0; 			\
	cx2=clgd547xStateInfo->width; 	\
	cy2=clgd547xStateInfo->height;
#define RGBA
#include "clrtemp.h"

void clear24c(uint r, uint g, uint b, uint a)
{
  clearArea24_2c(0,0,clgd547xStateInfo->width,clgd547xStateInfo->height,
  		 r,g,b,a);
}

/*************** LINE FUNCTIONS *******************/  
void line8_2fc(uint x1, uint y1, uint x2, uint y2, uint cindex)
#define FLAT_INDEX
#define OPCODE make_ro(VECTFZG2P,IOUTPAT+PATA_OFFB+PRAMALF,ZFLAT,A_ONLY)
#include "linetemp.h"

void line8_2s(uint x1, uint y1, uint i1, uint x2, uint y2, uint i2)
#define INTERP_INDEX
#define OPCODE make_ro(VECTFZG2P,IOUTPAT+PATA_OFFB+PRAMALF,ZFLAT,A_ONLY)
#include "linetemp.h"

void line8_3fc(uint x1, uint y1, uint z1, uint x2, uint y2, uint z2, uint cindex)
#define THREED
#define FLAT_INDEX
#define OPCODE make_ro(VECTFZG2P,IOUTPAT+PATA_OFFB+PRAMALF,ZNORMAL,A_ONLY)
#include "linetemp.h"

void line8_3s(uint x1, uint y1, uint z1, uint i1, uint x2, uint y2, uint z2, uint i2)
#define THREED
#define INTERP_INDEX
#define OPCODE make_ro(VECTFZG2P,IOUTPAT+PATA_OFFB+PRAMALF,ZNORMAL,A_ONLY)
#include "linetemp.h"

void line24_2fc(uint x1, uint y1, uint x2, uint y2, uint r, uint g, uint b, uint a)
#define FLAT_COLOR
#define OPCODE make_ro(VECTFZG2P,IOUTPAT+PATA_OFFB+PRAMALF,ZNORMAL,A_ONLY)
#include "linetemp.h"

void line24_2s(uint x1, uint y1, uint r1, uint g1, uint b1, uint a1, 
               uint x2, uint y2, uint r2, uint g2, uint b2, uint a2)
#define INTERP_COLOR
#define OPCODE make_ro(VECTFZG2P,IOUTPAT+PATA_OFFB+PRAMALF,ZNORMAL,A_ONLY)
#include "linetemp.h"

void line24_3fc(uint x1, uint y1, uint z1, uint x2, uint y2, uint z2, 
               uint r, uint g, uint b, uint a)
#define THREED
#define FLAT_COLOR
#define OPCODE make_ro(VECTFZG2P,IOUTPAT+PATA_OFFB+PRAMALF,ZNORMAL,A_ONLY)
#include "linetemp.h"

void line24_3s(uint x1, uint y1, uint z1, uint r1, uint g1, uint b1, uint a1, 
               uint x2, uint y2, uint z2, uint r2, uint g2, uint b2, uint a2)
#define THREED
#define INTERP_COLOR
#define OPCODE make_ro(VECTFZG2P,IOUTPAT+PATA_OFFB+PRAMALF,ZNORMAL,A_ONLY)
#include "linetemp.h"

/************** TRIANGLE FUNCTIONS *********************/
#ifndef COMPILE_MESA_STUBS

void triangle8_3fc(uint x1, uint y1, uint z1,
                   uint x2, uint y2, uint z2,
                   uint x3, uint y3, uint z3,
                   uint cindex)
#define NOVB
#define FLAT_INDEX
#define OPCODE make_ro(POLYFZG2P, PATA_PATB+IOUTPAT+PRAMALF,ZNORMAL,A_ONLY)
#include "tritemp.h"

void triangle8_3s(uint x1, uint y1, uint z1, uint i1,
                  uint x2, uint y2, uint z2, uint i2,
                  uint x3, uint y3, uint z3, uint i3)
#define NOVB
#define INTERP_INDEX
#define OPCODE make_ro(POLYFZG2P,PATA_PATB+IOUTPAT+PRAMALF,ZNORMAL,A_ONLY)
#include "tritemp.h"


void triangle24_3fc(uint x1, uint y1, uint z1,
                    uint x2, uint y2, uint z2,
                    uint x3, uint y3, uint z3,
                    uint r, uint g, uint b, uint a)
#define NOVB
#define OPCODE make_ro(POLYFZG2P,IOUTPAT+PATA_PATB+PRAMALF,ZMASK,A_ONLY)
#define OPCODE2 make_ro(POLYFZG2P,IOUTPAT+PATA_PATB+PRAMALF,ZNORMAL,A_ONLY)
#define FLAT_COLOR
#include "tritemp.h"

void triangle24_3s(uint x1, uint y1, uint z1, uint r1, uint g1, uint b1, uint a1,
                   uint x2, uint y2, uint z2, uint r2, uint g2, uint b2, uint a2,
                   uint x3, uint y3, uint z3, uint r3, uint g3, uint b3, uint a3)
#define NOVB
#define OPCODE make_ro(POLYFZG2P,IOUTPAT+PATA_PATB+PRAMALF,ZMASK,A_ONLY)
#define OPCODE2 make_ro(POLYFZG2P,IOUTPAT+PATA_PATB+PRAMALF,ZNORMAL,A_ONLY)
#define INTERP_COLOR
#include "tritemp.h"

#endif

#ifdef COMPILE_MESA_STUBS

/* function called by MESA */

void triangle8_3fcv(uint v0, uint v1, uint v2, uint pv)
#define FLAT_INDEX
#define OPCODE make_ro(POLYFZG2P, PATA_PATB+IOUTPAT+PRAMALF,ZNORMAL,A_ONLY)
#include "tritemp.h"

void triangle8_3sv(uint v0, uint v1, uint v2)
#define INTERP_INDEX
#define OPCODE make_ro(POLYFZG2P,PATA_PATB+IOUTPAT+PRAMALF,ZNORMAL,A_ONLY)
#include "tritemp.h"

void triangle24_3fcv(uint v0, uint v1, uint v2, uint pv)
#define OPCODE make_ro(POLYFZG2P,IOUTPAT+PATA_PATB+PRAMALF,ZMASK,A_ONLY)
#define OPCODE2 make_ro(POLYFZG2P,IOUTPAT+PATA_PATB+PRAMALF,ZNORMAL,A_ONLY)
#define FLAT_COLOR
#include "tritemp.h"

void triangle24_3sv(uint v0, uint v1, uint v2)
#define OPCODE make_ro(POLYFZG2P,IOUTPAT+PATA_PATB+PRAMALF,ZMASK,A_ONLY)
#define OPCODE2 make_ro(POLYFZG2P,IOUTPAT+PATA_PATB+PRAMALF,ZNORMAL,A_ONLY)
#define INTERP_COLOR
#include "tritemp.h"

#endif
