/* 
 * file: tritemp.h
 * auth: Peter McDermott
 * desc: template for triangle functions 
 *  
 * This is by far the most complicated algorithm I've ever had to create
 * and implement.  I'm graduating in 2 weeks and am now convinced that I
 * should have taken Numerical Analysis and Algorithms instead of Chinese,
 * and Networks.
 *
 * As it stands now, this function _STILL_ doesn't draw triangles correctly.
 * In fact, most of the time it's off by a pixel at at least one of the
 * end points.  It will hopefully be fixed shortly.
 *
 * #define's:
 *   OPCODE        actual opcode the 5470 is sent (in color-indexed mode)
 *   OPCODE1       opcode sent to 5470 for red & green banks in rgba mode
 *   OPCODE2       opcode sent to 5470 for blue bank in rgba mode 
 *   FLAT_INDEX    flat shaded, 8bpp
 *   INTERP_INDEX  gouraud shaded, 8pp
 *   FLAT_COLOR    flat shaded, 24 bpp
 *   INTERP_COLOR  gouraud shaded 24bpp
 *   NOVB	   function passes args as x1, y1, etc instead of three 
 *                 vertices v1, v2, v3
 *
 *  The six trianges:
 *
 *  left to right rendering:
 *
 *                         \                      /\
 *    |\                   \ \                   /..\
 *    | \                   \..\                /....\
 *    |  \                   \...\             /......\
 *    |   \                   \...|           /........\
 *     |  /                    \..|          /......../
 *     | /                      \..|        /...../
 *     |/                        \.|       /../         
 *                                \|      /.  
 *
 *
 * right to left rendering:
 *  
 */

/* #define DEBUG_TRITEMP_H */ 

{

   uint min, mid, max;

   int dy_main, dx_main;
   int count1, span1, rung1, count2, span2, rung2, px, width, t1, t2;
   int rungmain,dxm,wbp,dw,wopp,dwopp,xbp;  
   int tf; int i;

#ifdef INTERP_COLOR
   int rmin,gmin,bmin, rmid,gmid,bmid, rmax,gmax,bmax;
#endif

#ifdef NOVB
   /* the following code is used for the 'user-friendly' polygonalization
    * routines.  It's not actually compiled in the routines called by Mesa.
    * Mesa passes 3 (or 4) vertices, whereas the 'user-friendly' routines pass
    * all of the parameters as parameters and not vertices.  This setup code
    * fakes the Mesa interface. 
    */
   
   uint v0=0;			/* fake the four vertices passed to the routine */
   uint v1=1;
   uint v2=2;
   uint pv=0;			/* used to selecting color in flat-shaded tris */
   
   struct vertex_buff {  	/* create the vertex_buff struct */
     float Win[3][3];
     int   Win2[3][3];
     uint  Color[3][4];
     uint  Index[3];
   };
	
   struct vertex_buff VB; 	/* fake the vertex buffer */

   /* fill in the 3 vertices in the faked vertex buffer */
   VB.Win2[0][0]=x1<<12;	
   VB.Win2[0][1]=y1<<12;
   VB.Win2[0][2]=((-z2 & 0x7fff) >> 1)<<12;
   VB.Win2[1][0]=x2<<12;
   VB.Win2[1][1]=y2<<12;
   VB.Win2[1][2]=((-z2 & 0x7fff) >> 1)<<12;
   VB.Win2[2][0]=x3<<12;
   VB.Win2[2][1]=y3<<12;
   VB.Win2[2][2]=((-z3 & 0x7fff) >> 1)<<12;
   VB.Color[0][0]=VB.Color[1][0]=VB.Color[2][0]=0xff; /* color=red */ 
   VB.Color[0][1]=VB.Color[0][2]=VB.Color[0][3]=VB.Color[1][1]=VB.Color[1][2]=
   VB.Color[1][3]=
   VB.Color[2][1]=VB.Color[2][2]=VB.Color[2][3]=0;
   VB.Index[0]=VB.Index[1]=VB.Index[2]=2<<12; /* color index=2 */
#else
   
   /* 
    * Mesa uses floating point math! (and it's still fast?!)  Convert floats
    * to fixed point numbers and fixup the z-coords since, the Mondello's
    * z-buffer is a little broken.
    */

   /* unrolling ensues */   
   
   /* note: fix Mesa to do this for us.  As it is, many vertices are 
      converted from floats to fixed more than once wasting cycles!
    */
    
   VB.Win2[v0][0]=(uint)VB.Win[v0][0] << 12;
   VB.Win2[v0][1]=(uint)FLIP(VB.Win[v0][1]) << 12;
   VB.Win2[v0][2]=(uint)VB.Win[v0][2] << 12;
   i=(uint)(VB.Win[v0][2]*DEPTH_SCALE);
   VB.Win2[v0][2]=((-i & 0x7fff) >> 1) << 12;

   VB.Win2[v1][0]=(uint)VB.Win[v1][0] << 12;
   VB.Win2[v1][1]=(uint)FLIP(VB.Win[v1][1]) << 12;
   VB.Win2[v1][2]=(uint)VB.Win[v1][2] << 12;
   i=(uint)(VB.Win[v1][2]*DEPTH_SCALE);
   VB.Win2[v1][2]=((-i & 0x7fff) >> 1) << 12;

   VB.Win2[v2][0]=(uint)VB.Win[v2][0] << 12;
   VB.Win2[v2][1]=(uint)FLIP(VB.Win[v2][1]) << 12;
   VB.Win2[v2][2]=(uint)VB.Win[v2][2] << 12;   
   i=(uint)(VB.Win[v2][2]*DEPTH_SCALE);
   VB.Win2[v2][2]=((-i & 0x7fff) >> 1) << 12;

#endif 

#ifdef SETUP_CODE
   SETUP_CODE
#endif

#ifndef DEBUG_TRITEMP_H
   VB.Win2[v0][1]+=yoffs << 12;
   VB.Win2[v1][1]+=yoffs << 12;
   VB.Win2[v2][1]+=yoffs << 12;
#endif

#ifdef DEBUG_TRITEMP_H
   clear24(0,0,0);
   line24_2fc(VB.Win2[v0][0]>>12,0,VB.Win2[v0][0]>>12,400,0xa5,0xa5,0xa5,0);
   line24_2fc(VB.Win2[v1][0]>>12,0,VB.Win2[v1][0]>>12,400,0xa5,0xa5,0xa5,0);
   line24_2fc(VB.Win2[v2][0]>>12,0,VB.Win2[v2][0]>>12,400,0xa5,0xa5,0xa5,0);
/*   line24_2fc(0,VB.Win2[v0][1]>>12,640,VB.Win2[v0][1]>>12,0xa5,0xa5,0xa5,0);
   line24_2fc(0,VB.Win2[v1][1]>>12,640,VB.Win2[v1][1]>>12,0xa5,0xa5,0xa5,0);
   line24_2fc(0,VB.Win2[v2][1]>>12,640,VB.Win2[v2][1]>>12,0xa5,0xa5,0xa5,0);
*/
#endif

   if( VB.Win2[v0][1] <= VB.Win2[v1][1] ) {
      if( VB.Win2[v0][1] < VB.Win2[v2][1] ) {
         if ( VB.Win2[v1][1] < VB.Win2[v2][1] ) {	/* 0,1,2 */
	    min=v0; mid=v1; max=v2;
         }
         else {		/* 0,2,1 */
            min=v0; mid=v2; max=v1;
         }
      }
      else if( VB.Win2[v0][1] == VB.Win2[v2][1] && 
               VB.Win2[v0][1] == VB.Win2[v1][1]) { /* flat line */
         return;
      }
      else {     	/* 2,0,1 */
         min=v2; mid=v0; max=v1;
      }
   }
   else {
      if( VB.Win2[v1][1] < VB.Win2[v2][1] ) {
         if ( VB.Win2[v0][1] < VB.Win2[v2][1]) {	/* 1,0,2 */
            min=v1; mid=v0; max=v2;
         }
         else {		/* 1,2,0 */
            min=v1; mid=v2; max=v0;
         }
      }
      else {		/* 2,1,0 */
         min=v2; mid=v1; max=v0;
      } 
   }

#ifdef DEBUG_TRITEMP_H
   printf("# (%d,%d)  (%d,%d)  (%d,%d)\n",
          VB.Win2[min][0]>>12,VB.Win2[min][1]>>12,
          VB.Win2[mid][0]>>12,VB.Win2[mid][1]>>12,
          VB.Win2[max][0]>>12,VB.Win2[max][1]>>12);
#endif

#ifdef INTERP_COLOR   
   rmin=VB.Color[min][0] >> 3;
   gmin=VB.Color[min][1] >> 3;
   bmin=VB.Color[min][2] >> 3; 

   rmid=VB.Color[mid][0] >> 3;
   gmid=VB.Color[mid][1] >> 3;
   bmid=VB.Color[mid][2] >> 3; 

   rmax=VB.Color[max][0] >> 3;
   gmax=VB.Color[max][1] >> 3;
   bmax=VB.Color[max][2] >> 3; 
#endif
   
   /* note: Win2[min][0] <= Win2[mid][0] <= Win2[max][0] */

   count1=VB.Win2[mid][1]-VB.Win2[min][1];	/* distance btw min and mid */
   count2=VB.Win2[max][1]-VB.Win2[mid][1];   	/* distance btw max and mid */
   dy_main=VB.Win2[max][1]-VB.Win2[min][1];  	/* distance btw max and min */

   dx_main=VB.Win2[max][0]-VB.Win2[min][0];	
   tf=dx_main/(dy_main>>12);			/* dx/dy */
   px=VB.Win2[min][0]+(count1>>6)*(tf>>6);	/* x-intercept on main @ mid */ 
   width=VB.Win2[mid][0]-px;			/* may be neg */
   
   /* note: (count1 >= 0) && (count2 >= 0) && (dy_main >= 0) */
   
   if (dx_main) {
     if (dx_main >= 0)  
       rungmain=(dx_main+0x1000)/((dy_main>>12)+1);
     else
       rungmain=(dx_main-0x1000)/((dy_main>>12)+1);
   }
   else
     rungmain=0;

   /* width is always const or increasing from starting point */
   
   t1=VB.Win2[mid][0] - VB.Win2[min][0];
   if (t1 <0) t1=-t1;				/* t1 = abs(t1) */
   rung1=(t1+0x1000)/((count1>>12)+1); 	        /* rung1 is pos */

   /* width may be increasing or decreasing after mid point */

   t1=VB.Win2[max][0] - VB.Win2[mid][0];
  
   if (t1 >=0 ) {    				/* maxx >= midx */
     rung2=(t1+0x1000)/((count2>>12)+1);  
   }
   else {					/* maxx < midx */ 
     rung2=(t1-0x1000)/((count2>>12)+1);
   }

   /* note: (rung1 >= 0) && (rung2 may be neg) */

   /* fixup the width for a _very_ thin ( < 1 pixel ) line */

   t1 = (width<0) ? -width : width;           	/* t1 = abs(width)    */
   t2 = (rungmain<0) ? -rungmain : rungmain;	/* t2 = abs(rungmain) */
   
   if (t1<t2)
     width=rungmain;

   if (width>0) {
      wopp=width+rung2-rungmain+0x1000;		/* width count+1 scan line */
      if (wopp<t2)
        wopp=t2;

      if (rungmain < 0) {
        wbp=rung1-rungmain;			/* width 1st scan line */
        xbp=VB.Win2[min][0]+rungmain;		/* subtract a little */
	span2=wopp+rung2;
      }
      else {
        wbp=rung1;				/* width 1st scan line */
        xbp=VB.Win2[min][0];
        if ( rung2 > 0 )
          span2=wopp-rungmain;
        else
          span2=wopp+rung2-rungmain;
      }
/*      if (wbp<0x1000) {
        wbp+=0x1000;
      } */
      span1=width+0x1800-wbp;

   }
   else { 					/* width < 0 */
      wopp=-width-rung2+rungmain+0x1000;	/* width of count+1 line */
      if (wopp<t2)
        wopp=t2;
        
      if (rungmain > 0) {
        wbp=rung1+rungmain;			/* width 1st scan line */
        xbp=VB.Win2[min][0]+rungmain|0x80000000;/* subtract a little */
	span2=wopp-rung2;
      }
      else {
        wbp=rung1;				/* width 1st scan line */
        xbp=VB.Win2[min][0] | 0x80000000;
        if (rung2 < 0)
          span2=wopp+rungmain;
        else
          span2=wopp-rung2+rungmain;
      }
      width=-width;
/*      if (wbp<0x1000)
        wbp+=0x1000; */
      span1=width+0x1000-wbp;
   }     

   if (dy_main) {			/* dy_main has no fractional part */
     if (rungmain > 0) 
       dxm=(VB.Win2[max][0]-(xbp & 0x7fffffff))/((dy_main>>12)+1);
     else
       dxm=(VB.Win2[max][0]-(xbp & 0x7fffffff))/((dy_main>>12)+1);
   }
   else
     dxm=0;

   if (count1>>12) {
     dw=span1/(count1>>12);
   }
   else
     dw=0;
   
   if (count2>>12) {
     dwopp=span2/-(count2>>12);
   }
   else
     dwopp=0;
             
#ifdef FLAT_INDEX
   *listPtr++ = load_short_ro(BANK_MASK_REG, BLUE_BANK);
#endif
#ifdef INTERP_INDEX
   *listPtr++ = load_short_ro(BANK_MASK_REG, BLUE_BANK);
#endif
#ifdef FLAT_COLOR
   *listPtr++ = load_short_ro(BANK_MASK_REG, RED_BANK);
#endif
#ifdef INTERP_COLOR
   *listPtr++ = load_short_ro(BANK_MASK_REG, RED_BANK);
#endif

   *listPtr++ = OPCODE;
   count1 &= 0x00ffff00;
   dy_main=(dy_main>>12)+1;
   count2 &= 0x00ffff00;
   width=(width>>12)+1;

   *listPtr++ = ((count1 << 4) & 0xff000000) | VB.Win2[min][1]; /* r1 */
   *listPtr++ = (count1 << 12) | 0x00001000;			/* r2 */
   *listPtr++ = xbp; 						/* r3 */
   *listPtr++ = dxm;						/* r4 */
   *listPtr++ = VB.Win2[min][2];				/* r5 */
   *listPtr++ = (VB.Win2[max][2]-VB.Win2[min][2])/dy_main; /* r6 */
 #ifdef FLAT_INDEX
   *listPtr++ = VB.Index[pv];					/* r7 */
   *listPtr++ = 0;						/* r8 */
 #endif
 #ifdef INTERP_INDEX
   *listPtr++ = VB.Index[mid];					/* r7 */
   *listPtr++ = (VB.Index[max]-VB.Index[min])/dy_main; /* r8 */
 #endif
 #ifdef FLAT_COLOR
   *listPtr++ = VB.Color[pv][0]<<12;			        /* r7 */
   *listPtr++ = 0;						/* r8 */
 #endif
 #ifdef INTERP_COLOR
   *listPtr++ = rmin;						/* r7 */
   *listPtr++ = (rmax-rmin)/dy_main; 			        /* r8 */
 #endif
   
   *listPtr++ = wbp; 						/* r9 */
   *listPtr++ = dw;						/* r10 */
   *listPtr++ = (count2<<4) & 0xff000000 | wopp & 0x00ffffff;	/* r11 */
   *listPtr++ = (count2<<12) | (dwopp & 0x00ffffff);		/* r12 */
   *listPtr++ = 0;						/* r13 */
   *listPtr++ = (VB.Win2[mid][2]-VB.Win2[min][2])/width;        /* r14 */  
   *listPtr++ = 0;						/* r15 */
   
   
#ifdef FLAT_INDEX
   *listPtr++ = 0;						/* r16 */
#endif
#ifdef INTERP_INDEX
   *listPtr++ = 0;						/* r16 */
#endif

#ifdef FLAT_COLOR
   *listPtr++ = 0;						/* r16 */

   /* redraw triangle in green plane */
   *listPtr++ = load_short_ro(BANK_MASK_REG,GREEN_BANK);/* set bank = green */
   *listPtr++ = load_long_ro(IMAIN_REG,1);		/* load imain       */
   *listPtr++ = VB.Color[pv][1]<<12;			/* load green	    */ 
   *listPtr++ = OPCODE & ~0x3f;				/* exec, no loads   */
   
   /* redraw triangle in blue plane */
   *listPtr++ = load_short_ro(BANK_MASK_REG,BLUE_BANK);	/* set bank = blue  */
   *listPtr++ = load_long_ro(IMAIN_REG,1);		/* load imain       */
   *listPtr++ = VB.Color[pv][2]<<12;			/* load blue        */
   *listPtr++ = OPCODE2 & ~0x3f;			/* exec, no loads   */
#endif

#ifdef INTERP_COLOR
   *listPtr++ = 0;						/* r16 */

   /* redraw triangle in green plane */
   *listPtr++ = load_short_ro(BANK_MASK_REG,GREEN_BANK);/* set bank = green */
   *listPtr++ = load_long_ro(IMAIN_REG,2);		/* load imain, di   */
   *listPtr++ = gmin;					/* load green       */
   *listPtr++ = (gmax-gmin)/dy_main;			/* load del green   */
   *listPtr++ = load_long_ro(IORTHO_REG,1);		/* load iortho      */
   *listPtr++ = (gmid-gmin)/width;			/* load ortho green */
   *listPtr++ = OPCODE & ~0x3f;				/* exec, no loads   */
   
   /* redraw triangle in blue plane */
   *listPtr++ = load_short_ro(BANK_MASK_REG,BLUE_BANK); /* set bank = blue  */
   *listPtr++ = load_long_ro(IMAIN_REG,2);		/* load imain, di   */
   *listPtr++ = bmin;   				/* load blue        */
   *listPtr++ = (bmax-bmin)/dy_main;			/* load del blue    */
   *listPtr++ = load_long_ro(IORTHO_REG,1);             /* load iortho      */
   *listPtr++ = (bmid-bmin)/width;			/* load ortho blue  */
   *listPtr++ = OPCODE2 & ~0x3f;			/* exec, no loads   */
#endif
  

  if ((wopp + dwopp*(count2>>12))<0) {
    printf("** UNDERFLOW **\n");
    exit(-1);
  }

  CHKEXECPOLY;

  /*
   * check if dpy list full, if is start exec and switch to dpy list 2, 
   * else continue
   */
/*  CHKEXEC; */
}

#undef OPCODE
#undef FLAT_INDEX
#undef INTERP_INDEX
#undef FLAT_COLOR
#undef INTERP_COLOR
#undef NOVB

