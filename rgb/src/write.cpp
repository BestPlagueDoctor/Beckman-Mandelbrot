#include "proto.h"
#include <vector>
#include <cstdio>


void rescale_colors_global(std::vector<float> &zeroimg, std::vector<int> &img, int xres, int yres, int maxiter, int res) {
  float maxdiv = 1.0 / maxiter;
  for (int i = 0; i < (xres*yres); i++) {
   // zeroimg[i] = img[i] * maxdiv;
    zeroimg[i] = img[i];
  }
  float total = 0.0;
  for (int i = 0; i < res; i++) {
    total+=zeroimg[i];
  }
  printf("%f", total);
  int i;
  auto rescaler = std::vector<int>(sizeof(10000));
  for (i = 0; i < res; i++) {
    rescaler.clear();
    for (const auto& x: zeroimg) {
      rescaler[x]++;
    }
    zeroimg[i] += rescaler[i] / total;
    printf("%.2f ", zeroimg[i]);
  }
}

void write_pgm(const char *filename, int* img, int xres, int yres, int res, int maxiter) {
  printf("Writing Mandelbrot set to filename '%s' ... \n", filename);
  FILE *ofp;
  if ((ofp = fopen(filename, "w")) == NULL) {
    perror("Failed to open output file\n");
    return;
  }
  //PGM header
  fprintf(ofp, "P2\n");
  fprintf(ofp, "%d %d \n", xres, yres);
  fprintf(ofp, "%d \n", maxiter);
  for (int i = 0; i < res; i++) {
    if (i%xres == 0) {
      fprintf(ofp, "\n");
    }
    printf("Calculating line: %d              \r", i%xres);
    fflush(stdout);
    fprintf(ofp, "%d ", img[i]);
  }
  fclose(ofp);
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
  b = zeroimg[i] * 175;
  return b;
}


void write_ppm(const char *filename, std::vector<float> &zeroimg, int xres, int yres, int res, int maxiter) {
  FILE *ofp;
  if ((ofp = fopen(filename, "w")) == NULL) {
    perror("Failed to open output file\n");
    return;
  }
  //ppm header
  fprintf(ofp, "P3\n");
  fprintf(ofp, "%d %d \n", xres, yres);
  fprintf(ofp, "%d \n", 255);
  int i; 
  for (i = 0; i < res; i++) {
    int r = 0.0, g = 0.0, b = 0.0;
    //r = rescale_r_ppm(zeroimg, i, r);
    //g = (rescale_g_ppm(zeroimg, i, g));
    //b = (rescale_b_ppm(zeroimg, i, b));
    //r = (255 - rescale_r_ppm(zeroimg, i, r));
    //g = (255 - rescale_g_ppm(zeroimg, i, g));
    b = (175 - rescale_b_ppm(zeroimg, i, b));
    fprintf(ofp, "%d %d %d ", r, g, b);
    //fprintf(ofp, "%.1f %.1f %.1f  ", zeroimg[i], zeroimg[i], zeroimg[i]);
    if (i%xres == 0) {
      fprintf(ofp, "\n");
    }
    //fprintf(ofp, "%d ", zeroimg[i]);
  }
  fclose(ofp);
} 

//Total all iterations and use to divide the value of the hue by this total 
//Find a way to use hue lol
