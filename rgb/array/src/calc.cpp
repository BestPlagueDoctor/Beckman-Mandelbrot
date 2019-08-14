#include "proto.h"
#include <cmath>

int findidx(int xres, int x, int y) {
  return y*xres + x;
}

void setvals(int *img, int xres, int yres) {
  int x, y;
  for (y = 0; y < yres; y++) {
    for (x = 0; x < xres; x++) {
      int idx = findidx(xres, x, y);
      img[idx] = idx;
    }
  }
}



void imgmandel(int maxiter, int *img, int xres, int yres) {
  setvals(img, xres, yres);
  float recdiv = 3.0/(xres);
  float imcdiv = 2.4/yres;
  //float recdiv = .4/(xres);
  //float imcdiv = .2/yres;
  complexnumber c;
  complexnumber z;
  complexnumber save;
  int y;
  for (y = 0; y < yres; y++) {
    int x;
    for (x = 0; x < xres; x++) {
      float rec = x, imc = y;
      float cimag = ((imc*imcdiv)-1.2);
      float creal = (((rec*recdiv)-2.0));
      //float cimag = ((imc*imcdiv)-0.0);
      //float creal = (((rec*recdiv)-0.90));
      c.setcomp(creal, cimag);
      z.setcomp(0.0,0.0);
      save.setcomp(0.0, 0.0);
      int iters=0;
      float zreal = z.getreal(), zimag = z.getimag();
      float ztemp = 0.0;
      float savereal=zreal, saveimag=zimag;
#if 1      
       while ((iters < maxiter) && (zreal*zreal+zimag*zimag <= 4.0)) {
        savereal = zreal;
        saveimag = zimag;
        //squaring z
        ztemp = zreal;
        zreal = ((zreal * zreal) - (zimag * zimag));
        zimag = (ztemp * zimag);
        zimag += zimag;
        //adding c
        zreal += creal;
        zimag += cimag;
        //squaring z
        ztemp = zreal;
        zreal = ((zreal * zreal) - (zimag * zimag));
        zimag = (ztemp * zimag);
        zimag += zimag;
        //adding c
        zreal += creal;
        zimag += cimag;
        //squaring z
        ztemp = zreal;
        zreal = ((zreal * zreal) - (zimag * zimag));
        zimag = (ztemp * zimag);
        zimag += zimag;
        //adding c
        zreal += creal;
        zimag += cimag;
        //squaring z
        ztemp = zreal;
        zreal = ((zreal * zreal) - (zimag * zimag));
        zimag = (ztemp * zimag);
        zimag += zimag;
        //adding c
        zreal += creal;
        zimag += cimag;
        //squaring z
        ztemp = zreal;
        zreal = ((zreal * zreal) - (zimag * zimag));
        zimag = (ztemp * zimag);
        zimag += zimag;
        //adding c
        zreal += creal;
        zimag += cimag;
        //squaring z
        ztemp = zreal;
        zreal = ((zreal * zreal) - (zimag * zimag));
        zimag = (ztemp * zimag);
        zimag += zimag;
        //adding c
        zreal += creal;
        zimag += cimag;
        iters += 7;
      }
#endif


#if 1  
      if ((iters >=maxiter) || (zreal*zreal+zimag*zimag >= 4.0)) {
        iters -= 7;
        zreal = savereal;
        zimag = saveimag; 
#endif
#if 1
        float zrsq = zreal*zreal;
        float zisq = zimag*zimag;
        while ((iters < maxiter) && (zrsq+zisq <= 4.0)) {
          ztemp = zreal;
          zreal = (zrsq) - (zisq);
          zimag = (ztemp * zimag);
          zimag += zimag;
          //adding c
          zreal += creal;
          zimag += cimag;
          zrsq = zreal*zreal;
          zisq = zimag*zimag;
          iters++;
        }
#endif
#if 1
      }
#endif

      img[((y*xres)+x)] = iters;
      //printf("%d, ", iters);
      //  printf("%d, %d)  ", ((y*xres)+x), iters);
    }
  }
}

