#include "proto.h"
#include <iostream>
#include <cmath>

int findidx(int xres, int x, int y) {
  return y*xres + x;
}

void setvals(std::vector<int> &img, int xres, int yres) {
  int x, y;
  for (y = 0; y < yres; y++) {
    for (x = 0; x < xres; x++) {
      int idx = findidx(xres, x, y);
      img[idx] = idx;
    }
  }
}

void looper(int &iters, complexnumber z, complexnumber c, float zreal, float zimag, float creal, float cimag) {
  for (int i = 0; i <= 7; i ++) {
    float
    ztemp = zreal;
    zreal = ((zreal * zreal) - (zimag * zimag));
    zimag = (ztemp * zimag);
    zimag += zimag;
    //adding c
    zreal += creal;
    zimag += cimag;
    if (zreal*zreal+zimag*zimag >= 4) {
      break;
    }
    iters++;
  }
}


void imgmandel(int maxiter, std::vector<int> &img, int xres, int yres) {
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
      int iters;
      for (iters = 0; iters < maxiter; iters+=7) {
        float zreal = z.getreal(), zimag = z.getimag(), ztemp = 0.0;
        float savereal = zreal, saveimag = zimag;
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
        //setting z
        z.setcomp(zreal, zimag);
        if (zreal*zreal+zimag*zimag >= 4.0) {
          iters -= 7;
          z.setcomp(savereal, saveimag);
          looper(iters, z, c, zreal, zimag, creal, cimag);
          break;
        }
      }
      img[((y*xres)+x)] = iters;
    }
  }
}

