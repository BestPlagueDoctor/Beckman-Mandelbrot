#include "proto.h"
#include <vector>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <string>

void rescale_colors_global(float *zeroimg, int *img, int xres, int yres, int &maxval, int res) {
  float totalinv = 1.0f/float(res);
  float *histogram = (float*) malloc(res*sizeof(float));
  for (int j = 0; j < res; j++) {
    histogram[img[j]]++;
  }
  float sum = 0.0;
  for (int j = 0; j <= maxval; j++) {
    sum += histogram[j];
    histogram[j] = sum * totalinv;
  }

  for (int i = 0; i < res; i++) {
    zeroimg[i] = histogram[img[i]];
  }

  //maxval = histogram[maxval];
}

void write_pgm(const char *filename, int* img, int xres, int yres, int res, int maxval) {
  printf("Writing Mandelbrot set to filename '%s' ... \n", filename);
  FILE *ofp;
  if ((ofp = fopen(filename, "w")) == NULL) {
    perror("Failed to open output file\n");
    return;
  }
  //PGM header
  fprintf(ofp, "P2\n");
  fprintf(ofp, "%d %d \n", xres, yres);
  fprintf(ofp, "%d \n", maxval);
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

/*float rescale_r_ppm(std::vector<float> &zeroimg, int i, int &r) {
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
*/

void write_ppm(const char *filename, float *zeroimg, int xres, int yres, int res, int maxval) {
  FILE *ofp;
  if ((ofp = fopen(filename, "w")) == NULL) {
    perror("Failed to open output file\n");
    return;
  }
  //ppm header
  fprintf(ofp, "P3\n");
  fprintf(ofp, "%d %d \n", xres, yres);
#if 1
  fprintf(ofp, "%d \n", maxval);
#else
  fprintf(ofp, "%d \n", int(ceilf(10.0f * (logf(maxval)))));
#endif
  int i; 
  for (i = 0; i < res; i++) {
    int r = 0, g = 0, b = 0;
#if 1
    r = maxval - (maxval * zeroimg[i]);
    b = maxval * zeroimg[i];
#else 
    r = 10.0f * (logf((1 * zeroimg[i])));
#endif
    fprintf(ofp, "%d %d %d ", r, g, b);
    if (i%xres == 0) {
      fprintf(ofp, "\n");
    }
  }
  fclose(ofp);
}
