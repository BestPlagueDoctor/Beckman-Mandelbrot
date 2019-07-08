#include "proto.h"
#include <iostream>
#include <cmath>
void looper(int &iters, complexnumber z, complexnumber c);

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

void imgmandel(int maxiter, std::vector<int> &img, int xres, int yres) {
  setvals(img, xres, yres);
  float recdiv = 3.0/(xres);
  float imcdiv = 2.4/yres;
  //printf("%.10f, %.10f", recdiv, imcdiv);
  complexnumber c;
  complexnumber z;
  complexnumber save;
  int y;
  for (y = 0; y < yres; y++) {
    int x;
      for (x = 0; x < xres; x++) {
        float rec = x, imc = y;
        float cimag = ((imc*imcdiv)-1.2);
        float creal = (((rec*recdiv)-2)); 
        c.setcomp(creal, cimag);
        z.setcomp(0.0,0.0);
        save.setcomp(0.0, 0.0);
        int iters;
        std::string str;
        for (iters = 0; iters < maxiter; iters+=4) {
          float zreal = z.getreal(), zimag = z.getimag(), ztemp = 0.0;
          save.setcomp(z.getreal(), z.getimag());
          //z.multcomp(z, z); z.addcomp(z, c); z.multcomp(z, z); z.addcomp(z, c); z.multcomp(z, z); z.addcomp(z, c); z.multcomp(z, z); z.addcomp(z, c);
          //squaring z
          ztemp = zreal;
          zreal = ((zreal * zreal) - (zimag * zimag));
          zimag = ((ztemp * zimag) );
          zimag += zimag;
          //adding c
          zreal += creal;
          zimag += cimag;
          //squaring z
          ztemp = zreal;
          zreal = ((zreal * zreal) - (zimag * zimag));
          zimag = ((ztemp * zimag) );
          zimag += zimag;
          //adding c
          zreal += creal;
          zimag += cimag;
          //squaring z
          ztemp = zreal;
          zreal = ((zreal * zreal) - (zimag * zimag));
          zimag = ((ztemp * zimag) );
          zimag += zimag;
          //adding c
          zreal += creal;
          zimag += cimag;
          //squaring z
          ztemp = zreal;
          zreal = ((zreal * zreal) - (zimag * zimag));
          zimag = ((ztemp * zimag) );
          zimag += zimag;
          //adding c
          zreal += creal;
          zimag += cimag;
          //setting z
          z.setcomp(zreal, zimag);
          if (z.sqmagnit() >= 4.0) {
            iters -= 4;
            z.setcomp(save.getreal(), save.getimag());
            looper(iters, z, c);
            break;
          }
        }
        img[((y*xres)+x)] = iters;
      }
    }
  }

void looper(int &iters, complexnumber z, complexnumber c) {
  for (int i = 0; i <= 4; i ++) {
    z.multcomp(z, z);
    z.addcomp(z, c);
    if (z.sqmagnit() >= 4) {
      break;
    }
    iters++;
  }
}

