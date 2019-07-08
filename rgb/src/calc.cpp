#include "proto.h"
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
        float nimc = ((imc*imcdiv)-1.2);
        float nrec = (((rec*recdiv)-2)); 
        c.setcomp(nrec, nimc);
        z.setcomp(0.0,0.0);
        save.setcomp(0.0, 0.0);
        int iters;
        for (iters = 0; iters < maxiter; iters+=4) {
          float zreal = z.getreal(), zimag = z.getimag();
          float creal = c.getreal(), cimag = c.getimag();
          save.setcomp(z.getreal(), z.getimag());
          //z.multcomp(z, z); z.addcomp(z, c); z.multcomp(z, z); z.addcomp(z, c); z.multcomp(z, z); z.addcomp(z, c); z.multcomp(z, z); z.addcomp(z, c);
          //squaring z
          zreal = (zreal * zreal) - (zimag * zimag);
          zimag = (zreal * zimag) + (zimag * zreal);
          //adding c
          zreal = zreal + creal;
          zimag = zimag + cimag;
          zreal = (zreal * zreal) - (zimag * zimag);
          zimag = (zreal * zimag) + (zimag * zreal);
          zreal = zreal + creal;
          zimag = zimag + cimag;
          zreal = (zreal * zreal) - (zimag * zimag);
          zimag = (zreal * zimag) + (zimag * zreal);
          zreal = zreal + creal;
          zimag = zimag + cimag;
          zreal = (zreal * zreal) - (zimag * zimag);
          zimag = (zreal * zimag) + (zimag * zreal);
          zreal = zreal + creal;
          zimag = zimag + cimag;
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

