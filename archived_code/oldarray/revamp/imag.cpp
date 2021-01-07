#include "proto.h"

void rescale_colors_global(float *zeroimg, int *img, int xres, int yres, int *maxiter) {
  for (int i = 0; i < (xres*yres); i++) {
    float maxdiv = 1.0 / *maxiter;
    zeroimg[i] = img[i] * maxdiv;
  }
  //optional for rgb because those have to be ints, just use float code and convert to int
  /*for (int i = 0; i < xres*yres; i++) {
    img[i] = zeroimg[i];
  }*/
}
void rescale_colors_pgm(int *img, int res, int *maxiter) {
  int maxval = (*maxiter) + 1;
  for (int i = 0; i < res; i++) {
    img[i] = *maxiter-img[i];
  }
}

void rescale_colors_tri_ppm(int *img, int res, int xres, int yres, int *maxiter, int i, int r, int g, int b) {
  //r = img[i] / *maxiter;
  //g = img[i] / *maxiter;
  //b = img[i] / *maxiter;
}
float rescale_r_ppm(float *zeroimg, int *maxiter, int i, int r) {
  r = zeroimg[i] * 255;
  return r;
}
float rescale_g_ppm(float *zeroimg, int *maxiter, int i, int g) { 
  g = zeroimg[i] * 255;
  return g;
}
float rescale_b_ppm(float *zeroimg, int *maxiter, int i, int b) {  
  b = zeroimg[i] * 255; 
  return b;
}
