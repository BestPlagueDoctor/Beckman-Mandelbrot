#include "proto.h"
#include <cmath>
  

void imgmandel(int maxiter, int *img, int res, int xres, int yres) {
  int i = 0;
  int newit = 0;
  complexnumber c;
  complexnumber z;
  complexnumber zsq;
  for (i = 0; i < res; i++) {
    //printf("I am iterating the loop for the %dth time \n", i);
  //  float rec = findx(xres, i);
   // float imc = findy(xres, i);
    z.setcomp(0.0, 0.0); //z=(0+0i)
//    zsq.setcomp(0.0, 0.0);
    //float nrec = (((rec*3)/xres)-2), nimc = (((imc*2.4)/yres)-1.2);
    //c.setcomp(nrec, nimc);
    int iters = 0;
    for (iters = 0; iters < 4096; iters++) {
      //if (iters == 4095) {
        //newit+=1;
        //printf("I hit the ceiling for the %dth time\n", newit);
      //}
      z.multcomp(z, z); //zsq=z*z
      z.addcomp(z, c); //z=zsq+c
      if (z.sqmagnit() >= 4.0) {
        break;
      }
    }
    //printf("%d", iters);
    //img[i] = iters;  
  }
}
