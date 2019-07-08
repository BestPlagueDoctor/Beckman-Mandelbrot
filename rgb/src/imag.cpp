#include "proto.h"

void rescale_colors_global(std::vector<float> &zeroimg, std::vector<int> &img, int xres, int yres, int maxiter) {
  float maxdiv = 1.0 / maxiter;
  for (int i = 0; i < (xres*yres); i++) {
    zeroimg[i] = img[i] * maxdiv;
  }
}
/*void rescale_colors_pgm(int *img, int res, int maxiter) {
  for (int i = 0; i < res; i++) {
    img[i] = maxiter-img[i];
  }
}*/

void rescale_colors_tri_ppm(int *img, int maxiter, int i, int r, int g, int b) {
  r = img[i] / maxiter;
  g = img[i] / maxiter;
  b = img[i] / maxiter;
}
float rescale_r_ppm(std::vector<float> &zeroimg, int i, int &r) {
  r = zeroimg[i] * 255;
  return r;
}
float rescale_g_ppm(std::vector<float> &zeroimg, int i, int &g) { 
  g = zeroimg[i] * 255;
  return g;
}
float rescale_b_ppm(std::vector<float> &zeroimg, int i, int &b) {  
  b = zeroimg[i] * 255; 
  return b;
}
