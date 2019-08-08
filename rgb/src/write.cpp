#include "proto.h"
#include <string>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>


void rescale_colors_global(std::vector<float> &zeroimg, std::vector<int> &img, int xres, int yres, int maxiter, int res) {
//  float maxdiv = 1.0 / maxiter;
  for (int i = 0; i < (xres*yres); i++) {
//    zeroimg[i] = img[i] * maxdiv;
    //printf("%d, ", img[i]);
    zeroimg[i] = img[i];
    //if (i%1000) {
     // printf("%.2f, ", zeroimg[i]);
    //}
  }
  float total = res;
 // printf("%f", total);
  int i;
  auto rescaler = std::vector<float>(res+1);
  float totalinv = 1.0f/total;
  rescaler.resize(res+1);
  for (int j = 0; j < res; j++) {
    rescaler[zeroimg[j]]++;
  }
  float sum = 0.0;
  for (int j = 0; j < maxiter; j++) {
    sum += rescaler[j];
    rescaler[j] = sum * totalinv;
  }
  for (i = 0; i < res; i++) {
    zeroimg[i] = rescaler[i];
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
  fprintf(ofp, "%d \n", maxiter);
  int i; 
  for (i = 0; i < res; i++) {
    int r = 0, g = 0, b = 0;
    r = maxiter * zeroimg[i];
    //g = zeroimg[i];
    //b = zeroimg[i];
    //r = 255-zeroimg[i];
    //g = 255-zeroimg[i];
    //b = zeroimg[i];
    fprintf(ofp, "%d %d %d ", r, g, b);
    //fprintf(ofp, "%.1f %.1f %.1f  ", zeroimg[i], zeroimg[i], zeroimg[i]);
    if (i%xres == 0) {
      fprintf(ofp, "\n");
    }
    //fprintf(ofp, "%d ", zeroimg[i]);
  }
  fclose(ofp);
}
