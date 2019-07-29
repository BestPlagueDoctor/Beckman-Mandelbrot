#include "proto.h"
#include <vector>
#include <cstdio>
#include <cmath>


void rescale_colors_global(std::vector<float> &zeroimg, std::vector<int> &img, int xres, int yres, int maxiter, int res) {
  float maxdiv = 1.0 / maxiter;
  for (int i = 0; i < (xres*yres); i++) {
//    zeroimg[i] = img[i] * maxdiv;
    //printf("%d, ", img[i]);
    zeroimg[i] = img[i];
    //if (i%1000) {
     // printf("%.2f, ", zeroimg[i]);
    //}
  }
  float total = 0.0;
  for (int i = 0; i < res; i++) {
    total+=zeroimg[i];
  }
  printf("%f", total);
  int i;
  auto rescaler = std::vector<int>(res+1);
  for (i = 0; i < res; i+=14) {
    rescaler.clear();
    rescaler.resize(res+1);
    //printf("%.2f, ", zeroimg[i]);
    for (const auto& x: zeroimg) {
      rescaler[x]++;
    }
    zeroimg[i] += rescaler[i] / total;
    zeroimg[i+1] += rescaler[i+1] / total;
    zeroimg[i+2] += rescaler[i+2] / total;
    zeroimg[i+3] += rescaler[i+3] / total;
    zeroimg[i+4] += rescaler[i+4] / total;
    zeroimg[i+5] += rescaler[i+5] / total;
    zeroimg[i+6] += rescaler[i+6] / total;
    zeroimg[i+7] += rescaler[i+7] / total;
    zeroimg[i+8] += rescaler[i+8] / total;
    zeroimg[i+9] += rescaler[i+9] / total;
    zeroimg[i+10] += rescaler[i+10] / total;
    zeroimg[i+11] += rescaler[i+11] / total;
    zeroimg[i+12] += rescaler[i+12] / total;
    zeroimg[i+13] += rescaler[i+13] / total;
    ///printf("%.2f ", zeroimg[i]);
    printf("%d, ", i);
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
    int r = 0, g = 0, b = 0.0;
    //r = zeroimg[i];
    g = zeroimg[i];
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
