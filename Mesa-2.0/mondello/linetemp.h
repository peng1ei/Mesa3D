/* linetemp.h - line drawing template (see graphics.c)

   #define's
   
   THREED	- use z-values, otherwise z=0
   OPCODE       - the opcode for the given line
   FLAT_INDEX   - line is flat shaded (single color), 8bpp
   INTERP_INDEX - line is gouraud shaded (multi color), 8bpp
   FLAT_COLOR   - line is flat shaded (single color), 24bpp
   INTERP_COLOR - line is gouraud shaded (multi color) ,24bpp
   
 */
 
{

  int adx, ady; 
  int dx,dy; 
  uint reg1; 
  uint reg2; 
  uint reg3; 
  uint reg4;
  uint mlen;
  
#ifdef THREED
  uint reg5, reg6;
  z1=FN(z1);
  z2=FN(z2);
#endif
  
  y2+=yoffs;
  y1+=yoffs;
  
  dx = x2-x1;                                   
  dy = y2-y1;                                   
  adx=abs(dx);                                  	/* TODO - opt */
  ady=abs(dy);                                  	/* TODO - opt */
  reg3=(dx & 0x80000000) | ((dy & 0x80000000) >> 1) | (x1 << 12); 
  if (adx > ady) {
    reg1=(adx>>8)<<24 | y1 << 12;                 
    if (!dx) {			/* TODO - this case never happens! */
      reg2=adx<<24 | ((dy<<12)&0x00ffffff);
      mlen=1;
    }
    else {
      reg2=adx<<24 | (((dy<<12)/adx)&0x00ffffff);
      mlen=adx;
    }
    if(adx!=dx)                                 
      reg4=0x00fff000;                            
    else                                        
      reg4=0x00001000;                            
  } else {
    reg1=(ady>>8)<<24 | y1 << 12;                 
    if(ady!=dy)                                 
      reg2=ady<<24 | 0x00fff000;                  
    else                                        
      reg2=ady<<24 | 0x00001000;                  
    if (!dy) {
      reg4=dx<<12;
      mlen=1;
    }
    else {
      reg4=(dx<<12)/ady;
      mlen=ady;
    }
  } 
#ifdef THREED
  reg5=z1<<12;
  reg6=(z2-z1)<<12;
#endif

/* setup for first of possible several draws */

#ifdef FLAT_INDEX
  *listPtr++=load_short_ro(BANK_MASK_REG,BLUE_BANK);
#endif

#ifdef INTERP_INDEX
  *listPtr++=load_short_ro(BANK_MASK_REG,BLUE_BANK);
#endif

#ifdef FLAT_COLOR
  *listPtr++=load_short_ro(BANK_MASK_REG,RED_BANK);
#endif

#ifdef INTERP_COLOR
  *listPtr++=load_short_ro(BANK_MASK_REG,RED_BANK);
#endif

/* draw first line */

  *listPtr++ = OPCODE;					/* instruction 	*/
  *listPtr++ = reg1; 					/* y-start, y-count */
  *listPtr++ = reg2; 					/* y-delta, y-count */
  *listPtr++ = reg3; 					/* x-start 	*/
  *listPtr++ = reg4;					/* x-delta 	*/

/* do vars for tail end of 1st line (dz, di, etc) and following lines */

#ifdef THREED
  *listPtr++ = reg5;					/* z-start */
  *listPtr++ = reg6;					/* z-delta */
#else   
  *listPtr++ = 0;   					/* z-start */
  *listPtr++ = 0;					/* z-delta */
#endif

#ifdef FLAT_INDEX
  *listPtr++ = cindex << 12;				/* i-start */
  *listPtr++ = 0;					/* i-delta */
#endif

#ifdef INTERP_INDEX
  *listPtr++ = i1 << 12;				/* i-start */
  *listPtr++ = ((i2-i1+1) << 12)/mlen & 0x000fffff;	/* i-delta */
#endif

#ifdef FLAT_COLOR
  *listPtr++ = r << 12;					/* i-start */
  *listPtr++ = 0;					/* i-delta */
  
  *listPtr++ = load_short_ro(BANK_MASK_REG, GREEN_BANK);/* bank select 	*/
  *listPtr++ = load_long_ro(IMAIN_REG,1);		/* load i-start */
  *listPtr++ = g << 12;					/* i-start (r7)	*/  
  *listPtr++ = OPCODE & ~0x7f;				/* strip loads 	*/

  *listPtr++ = load_short_ro(BANK_MASK_REG, BLUE_BANK);	/* bank select 	*/
  *listPtr++ = load_long_ro(IMAIN_REG,1);		/* load i-start */
  *listPtr++ = b << 12;					/* i-start (r7)	*/  
  *listPtr++ = OPCODE & ~0x7f;				/* strip loads 	*/
#endif

#ifdef INTERP_COLOR
  *listPtr++ = r1 << 12;				/* i-start (r7)	*/
  *listPtr++ = ((r2-r1+1) << 12)/mlen;			/* i-delta (r8)	*/

  *listPtr++ = load_short_ro(BANK_MASK_REG, GREEN_BANK);/* bank select	*/
  *listPtr++ = load_long_ro(IMAIN_REG,2);		/* load i's 	*/
  *listPtr++ = g1 << 12;				/* i-start (r7)	*/
  *listPtr++ = ((g2-g1+1) << 12)/mlen;			/* i-delta (r8)	*/
  *listPtr++ = OPCODE & ~0x7f;				/* strip loads 	*/  
  
  *listPtr++ = load_short_ro(BANK_MASK_REG, BLUE_BANK);	/* bank select 	*/
  *listPtr++ = load_long_ro(IMAIN_REG,2);		/* load i's	*/
  *listPtr++ = b1 << 12;				/* i-start (r7)	*/
  *listPtr++ = ((b2-b1+1) << 12)/mlen;			/* i-delta (r8) 	*/
  *listPtr++ = OPCODE & ~0x7f;				/* strip loads 	*/
#endif
  
  CHKEXEC;						/* execute	*/
}

#undef FLAT_INDEX
#undef INTERP_INDEX
#undef FLAT_COLOR
#undef INTERP_COLOR
#undef THREED
#undef OPCODE


