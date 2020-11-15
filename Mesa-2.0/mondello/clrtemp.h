/*  clrtemp.h  -- template for clearArea functions 
 *
 *  #define's
 *    INDEX    in color indexed mode
 *    RGBA     in rgba mode
 */

#ifdef RGBA 
{
   int r1, r2;					
   int y_count;
   
#ifdef SETUP
   SETUP
#endif

#ifdef DEBUG_clgd547x
   printf("clear: x1=%d, y1=%d, x2=%d, y2=%d\n",cx1,cy1,cx2,cy2); 
#endif
   
   y_count = cy2-cy1;
   r1 = (y_count >> 8) << 24 | (cy1+yoffs) << FRACT;
   r2 = y_count<<24 | 0x1000;

   if (r==0 && 0 && r==g && r==b) {  /* r,g,b zero */
      *listPtr++ = SNOP;
      *listPtr++ = load_long_ro(C_AND_A_REG, 1);
      *listPtr++ = 0xffff0000;                  
      *listPtr++ = load_long_ro(MASK_B_REG, 1);
      *listPtr++ = 0xf7ffffff;
      *listPtr++ = load_short_ro(BANK_MASK_REG,RED_BANK|GREEN_BANK|BLUE_BANK);
      *listPtr++ = SNOP;
      *listPtr++ = blit_ro(BLTFZ,PRAMPAT+PATA_PATB+PRAMALF,A_ONLY);  
      *listPtr++ = r1;
      *listPtr++ = r2;
      *listPtr++ = cx1 << FRACT;
      *listPtr++ = (cx2-cx1) << FRACT;
      *listPtr++ = SNOP;
   }
   else if (0 && r==g && r==b) {  /* r,g,b same */
      r &= 0x000000ff;
      
      *listPtr++ = SNOP;
      *listPtr++ = load_long_ro(C_AND_A_REG, 1);
      *listPtr++ = 0xffff0000 | r << 8 | r;                  
      *listPtr++ = load_long_ro(MASK_B_REG, 1);
      *listPtr++ = 0xf7ffffff;
      *listPtr++ = load_short_ro(BANK_MASK_REG,RED_BANK|GREEN_BANK|BLUE_BANK);
      *listPtr++ = SNOP;
      *listPtr++ = blit_ro(BLTFZ,PRAMPAT+PATA_PATB+PRAMALF,A_ONLY);  
      *listPtr++ = r1;
      *listPtr++ = r2;
      *listPtr++ = cx1 << FRACT;
      *listPtr++ = (cx2-cx1) << FRACT;
      *listPtr++ = SNOP;
   }
   else /* r,g,b different */
   {   
      r &= 0x000000ff;
      g &= 0x000000ff;
      b &= 0x000000ff;
      
      *listPtr++ = SNOP;
      *listPtr++ = load_long_ro(C_AND_A_REG, 1);
      *listPtr++ = 0xffff0000 | r << 8 | r;                  
      *listPtr++ = load_long_ro(MASK_B_REG, 1);
      *listPtr++ = 0xf7ffffff;
      *listPtr++ = load_short_ro(BANK_MASK_REG, RED_BANK);
      *listPtr++ = SNOP;
      *listPtr++ = blit_ro(BLTFZ, PRAMPAT+PATA_PATB+PRAMALF,A_ONLY);  
      *listPtr++ = r1;				
      *listPtr++ = r2;				
      *listPtr++ = cx1 << FRACT;				/* r3 */
      *listPtr++ = (cx2-cx1) << FRACT;				/* r4 */
      *listPtr++ = SNOP;
      *listPtr++ = load_long_ro(C_AND_A_REG,1);
      *listPtr++ = 0xffff0000 | g << 8 | g;	
      *listPtr++ = load_short_ro(BANK_MASK_REG, GREEN_BANK);
      *listPtr++ = SNOP;
      *listPtr++ = blit_ro(BLTF,PRAMPAT+PATA_PATB+PRAMALF,A_ONLY) & ~0x7;
      *listPtr++ = SNOP;
      *listPtr++ = load_long_ro(C_AND_A_REG,1);
      *listPtr++ = 0xffff0101 | b << 8 | b;
      *listPtr++ = load_short_ro(BANK_MASK_REG, BLUE_BANK);
      *listPtr++ = SNOP;
      *listPtr++ = blit_ro(BLTF,PRAMPAT+PATA_PATB+PRAMALF,A_ONLY) & ~0x7;
      *listPtr++ = SNOP;
   }
   
   CHKEXEC;
}
#endif

