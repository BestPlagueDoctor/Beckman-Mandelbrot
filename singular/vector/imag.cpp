#include "proto.h"

void rescale_colors_pgm(int *img, int res, int *maxiter) {
  int maxval = (*maxiter) + 1;
  for (int i = 0; i < res; i++) {
    img[i] = *maxiter-img[i];
  }
}

void rescale_colors_tri_ppm(int *img, int res, int xres, int yres, int *maxiter, int i, int r, int g, int b) {
  r = img[i] / *maxiter;
  g = img[i] / *maxiter;
  b = img[i] / *maxiter;
}
int rescale_r_ppm(int *img, int *maxiter, int i, int r) {
  r = (img[i] * 255) / *maxiter;
  return r;
}
int rescale_g_ppm(int *img, int *maxiter, int i, int g) { 
  g = (img[i] * 255) / *maxiter;
  return g;
}
int rescale_b_ppm(int *img, int *maxiter, int i, int b) {  
  b = (img[i] * 255) / *maxiter;
  return b;
}
