#include "mondello/clgd547x.h"
#include "mondello/clgd5471.h"
#include "mondello/clgd5472.h"
#include "mondello/graphics.h"
#include <stdio.h>

int p[][6] = {
  {100,100,200,200,100,300}, /* 45 deg, mirrored bot, l-r, y+ */
  {200,100,100,200,200,300}, /* 45 deg, mirrored bot, r-l, y+ */
  {100,100,100,200,200,200}, /* 45 degree, no bot, l-r, y+ */
  {200,100,200,200,100,200}, /* 45 degree, no bot, r-l, y+ */
  {200,100,200,102,100,102}, /* no bot, count1=2, wide, r-l */
  {100,100,100,102,200,102}, /* no bot, count1=2, wide, l-r */
  {100,100,100,300,200,300}, /* 60 deg, no bot, l-r, y+ */
  {200,100,200,300,100,300}, /* 60 deg, no bot, r-l, y+ */
  {200,10,200,810,100,810},  /* tall, no bot, r-l, y+ */
  {100,10,100,810,200,810},  /* tall, no bot, l-r, y+ */
  {100,100,200,300,300,200},
  {100,100,200,200,200,300},  
  {320,100,120,105,520,105},
  {82,69,185,85,82,58},
  {343,76,257,92,346,86},
  {351,118,257,92,259,104},
  {141,201,132,210,199,280}, /* a good one */  
  {-1,-1,-1,-1,-1,-1}
};

void main()
{
  int i,j;
  int ch;
  clgd547x_init();

  i=0;
/*  while(p[i][0]!=-1) {  
    printf("## (%d,%d)  (%d,%d)  (%d,%d)\n",
      p[i][0],p[i][1],p[i][2],p[i][3],p[i][4],p[i][5]);

    triangle24_3fc(p[i][0],p[i][1],0,p[i][2],p[i][3],0,p[i][4],p[i][5],0,0xff,0,0,0);    
    i++;
    ch=getchar();
  } 
*/

  for(i=0;i<640;i+=10) {
    j+=20;
    triangle24_3fc(319,199,0, 319,199,0,i,0,0,   0xff,0,0,0);
    triangle24_3fc(319,199,0, 319,199,0,640,j,0, 0xff,0,0,0);
    triangle24_3fc(319,199,0, 319,199,0,i,399,0, 0xff,0,0,0);
    triangle24_3fc(319,199,0, 319,199,0,0,j,0,   0xff,0,0,0);
  }

  clgd547x_done();  
  
}
