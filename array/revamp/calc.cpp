#include "proto.h"
#include <cmath>
int findx(int xres, int i) {
  int xcoord = floor(i%xres);
  return xcoord;
}
int findy(int xres, int i) {
  int ycoord = floor(i/xres);
  return ycoord;
}

int findidx(int xres, int x, int y) {
  return y*xres + x;
}

void setvals(int* img, int xres, int yres) {
  int x, y;
  for (y = 0; y < yres; y++) {
    for (x = 0; x < xres; x++) {
      int idx = findidx(xres, x, y);
      img[idx] = idx;
    }
  }
}

void printcoord(int *img, int xres, int yres, int res) {
  int i, x, y;
  for (i = 0; i < res; i++) {
    x = findx(xres, i);
    y = findy(yres, i);
    int idx = findidx(xres, x, y);
    printf("(%d %c %d)", x, ',', y);
    if (x == (xres-1)) {
      printf("\n");
    }
  }
}

void imgmandel(int maxiter, int *img, int res, int xres, int yres) {
  setvals(img, xres, yres);
  int i = 0;
  complexnumber c;
  complexnumber z;
  complexnumber zsq;
  for (i = 0; i < res; i++) {
    int iters = 0;
    float rec = findx(xres, i), imc = findy(xres, i);
    z.setcomp(0.0, 0.0); //z=(0+0i)
    zsq.setcomp(0.0, 0.0); //zsq=(0+0i)
    c.setcomp(rec, imc); //c=(r+i)
    for (iters = 0; iters < maxiter; iters++) {
      zsq.multcomp(z, z); //zsq=z*z
      z.addcomp(zsq, c); //z=zsq+c
      if (z.sqmagnit() >= 4) {
        break;
      }
    }
    img[i] = iters;  
    printf("%d \n", img[i]);
  }
}
