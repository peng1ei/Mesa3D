/* 
  file: clgd5471.c
*/

#include "mondello/clgd547x.h"
#include "mondello/type.h"
#include "mondello/lut.h"
#include "mondello/clgd5471.h"
#include "mondello/clgd5472.h"
#include "mondello/compiler.h"

BYTE clgd5471SequencerRegs[] =
  {0x01,0x02,0x03,0x04,0x06,0x07,0x0f,0x1e,0x1f};
int clgd5471NumSequencerRegs = sizeof(clgd5471SequencerRegs)/sizeof(BYTE);

BYTE clgd5471GraphicsRegs[]=
  {0x05,0x06,0x07,0x17,0x38};
int clgd5471NumGraphicsRegs = sizeof(clgd5471GraphicsRegs)/sizeof(BYTE);

BYTE clgd5471VGAIOBase;

BYTE textSeq[]  =  { 0, 3, 0, 2, 18, 0, 23, 51, 28 };
BYTE textGraph[] = { 16, 14, 0, 0, 0 };
BYTE textMisc = { 0 };
  
/*-----------------------------------------------------------------------------
  clgd5471Unlock - unlocks the extended registers on the '71
-----------------------------------------------------------------------------*/
void clgd5471Unlock()
{
  outb(0x3c4,0x06);
  outb(0x3c5,0x12); 
}

/*----------------------------------------------------------------------------
  clgd5471Lock - locks the extended registers on the '71
-----------------------------------------------------------------------------*/
void clgd5471Lock()
{
  outb(0x3c4,0x06);
  outb(0x3c5,0x00); 
}

/*-----------------------------------------------------------------------------
  clgd5471CreateState - allocate a new state structure
-----------------------------------------------------------------------------*/
clgd5471State *clgd5471CreateState()
{
  clgd5471State *s=(clgd5471State*)malloc(sizeof(clgd5471State));

  s->validState=0;

  s->sequencerRegs=(BYTE*)malloc(clgd5471NumSequencerRegs*sizeof(BYTE));
  s->graphicsRegs=(BYTE*)malloc(clgd5471NumGraphicsRegs*sizeof(BYTE));

  s->hostMemory=(BYTE*)malloc(1024*1024); /* one meg video memory */
  return s;
}

/*-----------------------------------------------------------------------------
  clgd5471DeleteState - delete state structure
-----------------------------------------------------------------------------*/
void clgd5471DeleteState(clgd5471State *s)
{
  free(s->sequencerRegs);
  free(s->graphicsRegs);
  free(s->hostMemory);
  free(s);
}

/*-----------------------------------------------------------------------------
   getRegs - get a series of registers 
-----------------------------------------------------------------------------*/
void getRegs(BYTE *regs, int port, BYTE *regAddress, int num)
{
  int i;

  clgd5471Unlock();
    
  for(i=0; i<num; i++) {
    outb(port,regAddress[i]);
    regs[i]=inb(port+1);
  }
}

/*-----------------------------------------------------------------------------
   setRegs - set a series of registers 
-----------------------------------------------------------------------------*/
void setRegs(BYTE *regs, int port, BYTE *regAddress, int num)
{
  int i;

  clgd5471Unlock();
  
  for(i=0; i<num; i++) {
    outb(port,regAddress[i]);
    outb(port+1,regs[i]);
  }  
}

/*-----------------------------------------------------------------------------
   clgd5471SaveState - save video state
-----------------------------------------------------------------------------*/
void clgd5471SaveState(clgd5471State *s)
{  
  iopl(3);
  getRegs(s->sequencerRegs,0x3C4,clgd5471SequencerRegs,clgd5471NumSequencerRegs);
  getRegs(s->graphicsRegs,0x3CE,clgd5471GraphicsRegs,clgd5471NumGraphicsRegs);
  
  /* what register is at 0x3c6 -- find the IBM I/O reg map */
  inb(0x3c6);
  inb(0x3c6);
  inb(0x3c6);
  inb(0x3c6);
  s->miscReg=inb(0x3c6);
  iopl(0);

}

/*-----------------------------------------------------------------------------
   clgd5471RestoreState - restore the video state
-----------------------------------------------------------------------------*/
void clgd5471RestoreState(clgd5471State *s)
{
  iopl(3);

  setRegs(s->sequencerRegs,0x3C4,clgd5471SequencerRegs,clgd5471NumSequencerRegs);
  setRegs(s->graphicsRegs,0x3CE,clgd5471GraphicsRegs,clgd5471NumGraphicsRegs);

  inb(0x3c6);
  inb(0x3c6);
  inb(0x3c6);
  inb(0x3c6);
  outb(0x3c6,s->miscReg);

  iopl(0);
}

/*---------------------------------------------------------------------------*/
void textMode()
{
  clgd5472RestoreState(0);  /* turns off 5472 */

  iopl(3);
  setRegs(textSeq,0x3c4,clgd5471SequencerRegs,clgd5471NumSequencerRegs);
  setRegs(textGraph,0x3ce,clgd5471GraphicsRegs,clgd5471NumGraphicsRegs);

  inb(0x3c6);
  inb(0x3c6);
  inb(0x3c6);
  inb(0x3c6);
  outb(0x3c6,textMisc);
  iopl(0);
}

/*---------------------------------------------------------------------------*/

clgd5471Init()
{
  clgd5471VGAIOBase=(inb(0x3cc) & 0x01) ? 0x3d0 : 0x3b0;
}


