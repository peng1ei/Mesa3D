/*
  file: misc.c
*/

#include "mondello/compiler.h"  /* outb(), outw(), outl(), inb(), inw(), inl() */
#include "mondello/misc.h"

/*
 * PCI code gratuitously stolen from XF86's 
 * xc/programs/Xserver/hw/xf86/etc/scanpci.c. 
 * Original code written by Robin Cutshaw (robin@xfree86.org)
 * Adapted (read: ripped) by Peter McDermott for Mesa's Mondello driver
 * in Mesa 3.1.8.
 */

/*----------------------------------------------------------------------------
  PCILock - locks the PCI bus from stray writes
-----------------------------------------------------------------------------*/
void PCILock()
{
  outl(0xcf8,0x00000000);
}

#include <unistd.h>

/*----------------------------------------------------------------------------
  PCIUnlock - unlocks the PCI bus to allow configuration info to be read
-----------------------------------------------------------------------------*/
void PCIUnlock()
{
  long tmp1;
  long tmp2;


/*
  outb(0xcf8,0x00);		
  outb(0xcfa,0x00);
  tmp1 = inb(0xcf8);
  tmp2 = inb(0xcfa);
  if ((tmp1==0x00) && (tmp2==0x00)) {
    printf("PCI says configuration type 2\n");
  }
*/
  
  tmp1=inl(0xcf8);
  outl(0xcf8,0x80000000);
  tmp2=inl(0xcf8);
  outl(0xcf8,tmp1);
  if (tmp2!=0x80000000) {
    printf("PCIUnlock: No PCI available!");
  }
 
}

/*----------------------------------------------------------------------------
  PCIOut - sends an int at an address to the PCI bus (device=card #)
-----------------------------------------------------------------------------*/
void PCIOut(int device, int address, int data)
{
  int data_out;

  data_out=0x80000000;
  data_out|=device << 11;
  data_out|=(address & 0x3f);
  outl(0xcf8,data_out);
  outl(0xcfc,data);

}
 
/*----------------------------------------------------------------------------
  PCIIn - reads an int at an address from the PCI bus (device=card #)
-----------------------------------------------------------------------------*/
int PCIIn(int device, int address) 
{
  int data_out;

  data_out=0x80000000;
  data_out|=device << 11;
  data_out|=(address & 0x3f);
  outl(0xcf8,data_out);
  return inl(0xcfc);
}
