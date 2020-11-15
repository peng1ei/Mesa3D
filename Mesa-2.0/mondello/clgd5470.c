/* 

   file: clgd5470.c
   
*/

#include "mondello/compiler.h"
#include "mondello/clgd5470.h"
#include "mondello/clgd5471.h"
#include "mondello/clgd547x.h"
#include "mondello/davinci.h"

/* exported globals */

unsigned int *clgd5470LogicalBase;
unsigned int *clgd5470LogicalBase2;

unsigned int listStart=0;

#define MAXINSTR 4096;  /* 4096 bytes is maximum instruction length   */
unsigned int *listPtr;	/* where next instruction is to be written    */ 
unsigned int *listMax;	/* threshold for starting execution of instrs */
unsigned int listNum;   /* list number being written to               */
unsigned int *listPtr0; /* base of list 0                             */
unsigned int *listMax0; /* threshold of list 0                        */
unsigned int *listPtr1; /* base of list 1                             */
unsigned int *listMax1; /* threshold of list 1                        */

/*----------------------------------------------------------------------------
  inWarp(addr) reads a long from the Warp chip at address addr
-----------------------------------------------------------------------------*/
int inWarp(int address) 
{
  return *(unsigned int*)(clgd547xLogicalBase + 0x3ffd00 + (address << 2));
}

/*----------------------------------------------------------------------------
  outWarp(addr) writes a long to the Warp chip at address addr
-----------------------------------------------------------------------------*/
void outWarp(int address, unsigned int data)
{
  BYTE* addr;
  addr=(BYTE*)clgd547xLogicalBase;
  addr=addr+0x3ffc00;
  addr=addr+(address << 2);
  *((unsigned int*)addr)=data; 
/*  *(clgd547xLogicalBase + (0x3ffc00) + (address << 2)) = data; */
}

void dumpMem(int address, int range) 
{
  int i;
  BYTE ch;
  BYTE *t=(BYTE*)(((unsigned int)clgd547xLogicalBase)+address);
  for(i=0; i<range; i++) {
    ch=*t++;
    if ((ch>' ') && (ch<128)) {
      printf("%.2x %c -- ",ch,ch);
    }
    else
      printf("%.2x . -- ",ch);
  }
  printf("\n");
}

void wm(unsigned int *x)
{
  printf("%8x\n",*x);
}

void writem(unsigned int *x, int val)
{
  *x=val;
}

void debugWarp() 
{
  printf("control register      %8X %d\n",inWarp(0x20),inWarp(0x20));
  printf("scratch register      %8X %d\n",inWarp(0x21),inWarp(0x21));
  printf("mask a register       %8X %d\n",inWarp(0x22),inWarp(0x22));  
  printf("mask b register       %8X %d\n",inWarp(0x23),inWarp(0x23));
  printf("status                %8X %d\n",inWarp(0x24),inWarp(0x24));
/*  printf("bank enable register  %8X %d\n",inWarp(),inWarp());
  printf("bank mask register    %8X %d\n",inWarp(),inWarp());
  printf("bank compare register %8X %d\n",inWarp(),inWarp()); */
  printf("R1      %8X %8d  ",inWarp(0x28),inWarp(0x28));
  printf("R2      %8X %8d\n",inWarp(0x29),inWarp(0x29));
  printf("R3      %8X %8d  ",inWarp(0x2A),inWarp(0x2A));
  printf("R4      %8X %8d\n",inWarp(0x2B),inWarp(0x2B));
  printf("R5      %8X %8d  ",inWarp(0x2C),inWarp(0x2C));
  printf("R6      %8X %8d\n",inWarp(0x2D),inWarp(0x2D));
  printf("R7      %8X %8d  ",inWarp(0x2E),inWarp(0x2E));
  printf("R8      %8X %8d\n",inWarp(0x2F),inWarp(0x2F));
  printf("R9      %8X %8d  ",inWarp(0x30),inWarp(0x30));
  printf("R10     %8X %8d\n",inWarp(0x31),inWarp(0x31));
  printf("R11     %8X %8d  ",inWarp(0x32),inWarp(0x32));
  printf("R12     %8X %8d\n",inWarp(0x33),inWarp(0x33));
  printf("R13     %8X %8d  ",inWarp(0x34),inWarp(0x34));
  printf("R14     %8X %8d\n",inWarp(0x35),inWarp(0x35));
  printf("R15     %8X %8d  ",inWarp(0x36),inWarp(0x36));
  printf("R16     %8X %8d\n",inWarp(0x37),inWarp(0x37));
  printf("R17     %8X %8d  ",inWarp(0x38),inWarp(0x38));
  printf("R18     %8X %8d\n",inWarp(0x39),inWarp(0x39));
}

int test5470IO()
{
  int i;
  int errors=0;

  for (i=0x28; i<0x3a; i++) {
    outWarp(i,i);
  }

  for (i=0x28; i<0x3a; i++) {
    if(inWarp(i)!=i) errors++;
  }

  for (i=0x28; i<0x3a; i++) {
    outWarp(i,0);
  }
  return errors;
}

void testLoc(unsigned int *loc)
{
  int i=0;
  int done=0;
  int errors;
    
  while(!done) {
    *loc=i;
    if (*loc!=i) {
      printf(" test failed for %d\n",i);
      errors++;
      if (errors>20) done=1;
    }
    if (i==0xFFFFFFFF) {
      done=1;
    }  
    i++;
  }
}

/*-----------------------------------------------------------------------------
  clgd5470Init - enable the 5470, then bring it into a reasonable mode
-----------------------------------------------------------------------------*/
void clgd5470Init(int bpp) {
  int i,status;
  BYTE *t;
       
  clgd5470LogicalBase=(unsigned int*)clgd547xLogicalBase;
  
  printf("init5470: entering\n");
  
  /* bring Warp out of reset */
  printf("init5470: enabling 5470\n");

  iopl(3);
  outGraph(0xD078);
  *clgd5470LogicalBase=0xffffffff;
  status=*clgd5470LogicalBase;
  outGraph(0xD038);
  iopl(0);  

  sleep(1);
  if(inWarp(CONTROL_REG) & 0x00800000) {
    printf("5470 ok\n");
  }
  else {
    printf("failure to initialize 5470\n");
  }

  printf("init5470: 5470 IO Reg results: %d\n",test5470IO());

  printf("init5470: initializing 5470 video information\n");

  /* open up everything for modification */
  outWarp(MASK_B_REG,0xfDffffff);  
  outWarp(MASK_REG,0xffffffff);   

  /* outWarp(REF0_REG,0x00800000); */ /* 12/24 bpp modes */ 
  outWarp(REF0_REG,0x00800000);    /* 0x00000000 */ /* 8 bpp */
  outWarp(REF1_REG,0x00000000);    /* 0x00000800 */ /* set screen start addr = */ 
  outWarp(MASK_REG,0x00000000);
  outWarp(REF0_REG,0x61000000);
  outWarp(REF1_REG,0x00000000);

  outWarp(REF2_REG,0x00000400);    /* non db 8bpp */
  outWarp(REF3_REG,0x00000000);    /* set cursor base address */ 
  outWarp(REF4_REG,0x00000000);    /* set screen start time */
  outWarp(REF5_REG,      1200);    /* set screen end time */
  outWarp(REF6_REG,0x00000000);    /* set hw window start time */
  outWarp(REF7_REG,      1200);    /* set hw window end time */
  outWarp(REF8_REG,0x00000000);    /* set csr start time */
  outWarp(REF9_REG,0x00000000);    /* set csr end time */
  outWarp(REFA_REG,279);           /* 479 */ /* Wait timer end line */
  outWarp(REFB_REG,887); /*(int)(10e-3 / 512 / 22e-9));*/ /* set refresh time interval */
  outWarp(REFC_REG,0x00000002);    /* load local refresh minimum */
  outWarp(REFD_REG,0x00000008);    /* load local refresh maximum */
  outWarp(REFE_REG,0x00000008);    /* load host refresh minimum */
  outWarp(REFF_REG,0x00000020);    /* load host refresh max */

  outWarp(MASK_B_REG, 0x00000000);
  outWarp(DISABLE_REG,  0xffffffff);  /* 0x00000000 */
  outWarp(PATTERN_RAM,  0x00000000);  /* clear the pattern ram */
  outWarp(PATTERN_RAM1, 0x00000000);
  outWarp(PATTERN_RAM2, 0x00000000);
  outWarp(PATTERN_RAM3, 0x00000000);
  outWarp(PATTERN_RAM4, 0x00000000);
  outWarp(PATTERN_RAM5, 0x00000000);
  outWarp(PATTERN_RAM6, 0x00000000);
  outWarp(PATTERN_RAM7, 0x00000000);

#ifdef BLAH
  outWarp(MASK_REG,0xffbfffff);
  outWarp(STATUS_REG,0x08000000);
  outWarp(STATUS_REG,0x0);
  outWarp(MASK_REG,0xffdfffff);
  outWarp(REF0_REG,0x61000000);       /* rfrsh dram & vram, en scrn rfrs */   
  outWarp(REF1_REG,0x00000000);

  outWarp(MASK_REG,0x00000000);       /* open everything up to be modified */
  outWarp(STATUS_REG,0);              /* start 'er up */

  outWarp(MASK_B_REG,0x003fffff);     /* open everything up (except rsvd) */
#endif

  outWarp(CONTROL_REG,0x180c0c40);    /* 0x18020c42 */ /* start 'er up */  
  outWarp(STATUS_REG,0x00009100);       /* int on Idle & vsync */
/*  outWarp(STATUS_REG,0x00008100); */      /* int on Idle only */ 
  outWarp(C_AND_A_REG,0xffff0000);
  outWarp(OFFSET_REG  ,0x00000100);   /* 0x00 */ /* offset for alpha blending */
  outWarp(BNDH_REG    ,0xff);         /* color masking or saturation high */
  outWarp(BNDL_REG    ,0x0);          /* color masking or saturation low */
  outWarp(PMASK_REG   ,0x0);          /* plane mask register */
  outWarp(DASH_REG    ,0x0);          /* pat color or alpha */
  outWarp(DISABLE_REG ,0x0);          /* pat color or alpha */
  outWarp(HOST_PITCH_REG,4096); /* 0x00000400);*/ /* num to inc with inc in y when bltng */


  outWarp(BANK_ENABLE_REG, 0x00000000);  /* enable all 6 memory banks */
  outWarp(BANK_MASK_REG,   0x00000008);  /* 0x00000000 */ /* why 8 ?? */
  outWarp(BANK_COMPARE_REG,0x00008888);  /* 0x88888888 */ /* ?? */

  outWarp(MASK_REG,0xffffffff);       /* lock everything back up */
  outWarp(MASK_B_REG,0xf7ffffff);     /*  "        "      "    " */

  /* find the second meg of host memory */
  for(i=4; i<17; i++)
  {
    t=(BYTE*)(clgd547xLogicalBase + i*0x40000);
    *((unsigned int *)clgd547xLogicalBase)=0xa5a5a5a5;
    
    *((unsigned int *)t)=0x12345678;
    *((unsigned int *)(t+8))=0xfedcba98;
    if ((*((unsigned int *)clgd547xLogicalBase)==0xa5a5a5a5) &&
        (*((unsigned int *)t)==0x12345678) &&
        (*((unsigned int *)(t+8))==0xfedcba98)) {
      clgd547xLogicalBase2=(char*)t;
      clgd5470LogicalBase2=(unsigned int*)t;
      break;
    }
  }       

#ifdef BLAH
  printf("clgd5470Init()-testing first megabyte of host memory\n");
  printf("clgd5470Init()-%d errors\n",clgd547xTestMegabyte(clgd5470LogicalBase));
  printf("clgd5470Init()-testing second megabyte of host memory\n");
  printf("clgd5470Init()-%d errors\n",clgd547xTestMegabyte(clgd5470LogicalBase2));
  
  /* test again */
  printf("clgd5470Init()-testing both banks again\n");
  for(i=0;i<1024*1024/4;i++) {
    clgd5470LogicalBase[i]=i;
  }
  for(i=0;i<1024*1024/4;i++) {
    clgd5470LogicalBase2[i]=i+262144;
  }
  for(i=0;i<1024*1024/4;i++) {
    if(clgd5470LogicalBase[i]!=i)
      printf("clgd5470LogicalBase, location %d failed\n",i);
  }
  for(i=0;i<1024*1024/4;i++) {
    if(clgd5470LogicalBase2[i]!=(i+262144))
      printf("clgd5470LogicalBase2, location %d failed\n",i);
  }   
#endif
   
  /* test warp execution engine */
  printf("clgd5470Init() - testing 5470 execution engine\n");
  
  listPtr=clgd5470LogicalBase;
  
  for(i=0;i<40;i++) {
    *listPtr++=load_long_ro(PATTERN_RAM,1);
    *listPtr++=0xffffffff;
  }
  *listPtr=IDLE;
  
  outWarp(HOST_INSTRUCTION_REG,branch_ro(0x00));
  
  i=1000000;
  iopl(3);
  outb(0x3ce,0x3a);
  while((inb(0x3cf) & 0x02) && i--);
  iopl(0);
  
  status=inWarp(STATUS_REG);
  
  if (i==0) {
    printf("clgd5470Init() - 5470 execution engine not responding\n");
    return;
  }
  else {
    printf("clgd5470Init() - 5470 execution engine okay\n");
  }

  listPtr=clgd5470LogicalBase;  
  *listPtr++=load_short_ro(BANK_MASK_REG, BLUE_BANK);
  *listPtr++=load_long_ro(C_AND_A_REG,1);
  *listPtr++=0xffff0000;
  *listPtr++=blit_ro(BLTFZ, PRAMPAT+PATA_PATB+PRAMALF,A_ONLY);
  *listPtr++=0x03000000;
  *listPtr++=0xff001000;
  *listPtr++=0x00000000; 
  *listPtr++=0x003ff000;
  
  *listPtr++=load_short_ro(BANK_MASK_REG, GREEN_BANK | RED_BANK);
  *listPtr++=load_long_ro(C_AND_A_REG,1);
  *listPtr++=0xffff0000;
  *listPtr++=blit_ro(BLTFZ, PRAMPAT+PATA_PATB+PRAMALF, A_ONLY);
  *listPtr++=0x03000000;
  *listPtr++=0xff001000;
  *listPtr++=0x00000000;
  *listPtr++=0x003ff000;
  
  *listPtr++=IDLE;
  *listPtr++=IDLE;
  *listPtr++=IDLE;
  
  outWarp(HOST_INSTRUCTION_REG, branch_ro(0x00000));
  i=0x1000000;

  iopl(3);
  outb(0x3ce,0x3a);
  while((inb(0x3cf) & 0x02) && i--);
  iopl(0);

  listPtr0 = clgd5470LogicalBase;
  listMax0 = listPtr0 + 0x00080000/4 - MAXINSTR;
  listPtr1 = clgd5470LogicalBase + 0x00080000/4;
  listMax1 = listPtr1 + 0x00080000/4 - MAXINSTR;
  
  listNum = 0;
  listPtr = listPtr0;
  listMax = listMax0;
  
  printf("init5470: leaving\n");

}

