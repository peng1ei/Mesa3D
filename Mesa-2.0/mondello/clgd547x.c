	/*
   file: clgd547x.c
   auth: Peter McDermott
   date: Mon Feb 12 14:40:26 CST 1996
*/

#include <sys/types.h> /* open(), mmap() */
#include <sys/stat.h>  /* open() */
#include <fcntl.h>     /* open() */
#include <sys/mman.h>  /* mmap() */
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "mondello/clgd547x.h"
#include "mondello/clgd5471.h"  /* int clgd5471VGAIOBase */
#include "mondello/graphics.h"

/*-------------------------------------------------------------------------------------------*/

#define clgd5470_PCI_ID 0x00b41013
#define clgd5470_VGA_ID 0x00b4
#define clgd547x_PATCHLEVEL "0"

int clgd547xPCISlot=-1;        /* PCI slot the card is in */

char *clgd547xPhysicalBase;    /* physical linear memory detected via PCI */
char *clgd547xLogicalBase;     /* logical linear memory (phys. mapped here) */
char *clgd547xLogicalBase2;   
char *clgd547xSecondMegBase;   /* logical base of frame buffer mem (2nd meg) */
int clgd547xLogicalSize=0;

int clgd547xProbed=0;
int clgd547xInMondelloMode=0;     /* card is in or not in extended mode */
int clgd547xAlreadyInited=0;      /* whether or not the card has been inited */

clgd547xState *clgd547xStateInfo=0;        /* global state information */

#include "modes.h" /* the modes structure for setting a given mode */

/*-------------------------------------------------------------------------------------------*/
clgd547xState *clgd547xCreateState()
{
  clgd547xState *state=(clgd547xState*)malloc(sizeof(clgd547xState));
  state->inMondelloMode=0;
  state->s5471=clgd5471CreateState();
  state->s5472=clgd5472CreateState();
  state->hostMem=(BYTE*)malloc(1024*1024);
  return state;
}

/*-------------------------------------------------------------------------------------------*/
void clgd547xDeleteState(clgd547xState *state)
{
  clgd5471DeleteState(state->s5471);
  clgd5472DeleteState(state->s5472);
  free(state->hostMem);
  free(state);
}

/*-------------------------------------------------------------------------------------------*/
void clgd547xSaveState(clgd547xState *state)
{
  clgd5471SaveState(state->s5471);
  clgd5472SaveState(state->s5472);
}

/*-------------------------------------------------------------------------------------------*/
void clgd547xRestoreState(clgd547xState *state)
{
  if(state->inMondelloMode) {
    memcpy(clgd547xLogicalBase,state->hostMem,1024*1024);
  }
  clgd5472RestoreState(state->s5472);
  clgd5471RestoreState(state->s5471);
  state->inMondelloMode=0;

  /* force back to textMode--let's try to stay sane! */
  textMode();
}

/*-------------------------------------------------------------------------------------------*/
/* tests 1 meg of memory */

int clgd547xTestMegabyte(unsigned int *ptr) 
{
  int i,j;
  int errors=0;
  static unsigned int pattern[4]= 
    { 0x00000000, 0xAAAAAAAA, 0x55555555, 0xffffffff };
  
  for(j=0; j<4; j++)
  {
    for(i=0;i<1024*1024/4;i++)
      ptr[i]=pattern[j];
      
    for(i=0;i<1024*1024/4;i++)
      if(ptr[i]!=pattern[j]) {
        errors++;
      }
  }
  return errors;  
}


/*----------------------------------------------------------------------------
  probe - check to see if the chipset is there.
-----------------------------------------------------------------------------*/
int clgd547xProbe()
{
  int i;
  char outstr[128];

  if (getuid()) {
    printf("This program must be run as root to directly access video hardware\n");
    exit(1);
  }

  printf("probe entered\n");
  iopl(3);  /* allow access to all IO ports for now */

  /* locate the card on the bus (code taken from XF86's scanpci.c) */
    
  PCIUnlock();
  printf("Probe: sweeping for card ");
  for (i=0; i < 0x20; i++) {
    printf(".");
    if (PCIIn(i,0)==clgd5470_PCI_ID) {
      clgd547xPCISlot=i;
      clgd547xPhysicalBase=(char*)(PCIIn(i,0x10) & 0xfffffff0);
      break;
    }
  }
  PCILock();
  printf("\n");

  if (clgd547xPCISlot==-1) {
    printf("Probe: clgd5470 not found on PCI bus\n");
    return(-1);
  }
  else {
    printf("Probe: card found on PCI bus, slot %d\n",i);
    printf("Probe: card base address 0x%x\n",clgd547xPhysicalBase);
  }
  
  clgd547xProbed=1;
  return(1);
}

/*-------------------------------------------------------------------------------------------*/
int clgd547xInit()
{
  int i;

  if (!clgd547xProbed) {
    if(clgd547xProbe()<0) {
      printf("clgd547xInit() - probe failed, exiting\n");
      exit(1);
    }
  }
      
  clgd5471Init();

  iopl(3);   

  /* Maps clgd547x linear address space into video memory */

  printf("Init: mapping in linear memory\n");
    
  if ((i=open("/dev/mem",O_RDWR))<0) {
    printf("init: can't open /dev/mem for mmap\n");
    return -1;
  }

  clgd547xLogicalSize=4*1024*1024;  /* assume a 4 meg window for now */  
  clgd547xLogicalBase=(caddr_t)mmap((caddr_t)0,clgd547xLogicalSize,PROT_READ|PROT_WRITE,
  			    MAP_SHARED,i,(off_t)clgd547xPhysicalBase);
  close(i);
                            
  if ((int)clgd547xLogicalBase==-1) {
    printf("init: mmap failed\n");
    return -1;
  }
  
  printf("Leaving init...\n");

  iopl(0);
  return 0;
}


/*-------------------------------------------------------------------------------------------*/

void clgd547xSetMode(mode *m)
{
  clgd5472SetMode(m);
  
  if (!clgd547xStateInfo->inMondelloMode) {
    /* copy host memory */
    memcpy(clgd547xStateInfo->hostMem,clgd547xLogicalBase,1024*1024);
  }    

  clgd5470Init(m->bitsPerPixel);

  clgd547xStateInfo->inMondelloMode=1;  
  clgd547xStateInfo->width=m->CrtcHDisplay;
  clgd547xStateInfo->height=m->CrtcVDisplay;
}

/*-------------------------------------------------------------------------------------------*/

int currentMode=0;
 
int clgd547x_getxdim()
{
  return modes[currentMode].CrtcHDisplay;
}

int clgd547x_getydim()
{
  return modes[currentMode].CrtcVDisplay;
}

int clgd547x_getdepth()
{
  return modes[currentMode].bitsPerPixel;
}

int clgd547x_getcolors()
{
  return (1<<modes[currentMode].bitsPerPixel);
}

void clgd547x_setcolorindex(uint index, uint red, uint green, uint blue)
{
  clgd5472WriteIndex(index, red, green, blue);
}

void clgd547x_clear()
{
  clearArea8_2c(0,0,clgd547x_getxdim(),clgd547x_getydim(),0);
}

void clgd547x_init()
{  
  char ch;
  int done=0;
  currentMode=0;
  
  if(clgd547xProbe()<0) {
    printf("Cirrus Logic GD 547x not found, exiting.\n");
    exit(255);
  }
 
  clgd547xInit();
  
  clgd547xStateInfo=clgd547xCreateState();
  clgd547xSaveState(clgd547xStateInfo);
  clgd547xSetMode(&modes[currentMode]);
  clgd5472InitLUT(); 
}

void clgd547x_done()
{
  clgd547xRestoreState(clgd547xStateInfo);  
  clgd547xDeleteState(clgd547xStateInfo);
}

