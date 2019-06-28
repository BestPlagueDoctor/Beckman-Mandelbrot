#include "proto.h"
#include <cstdio>

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

void write_ppm(const char *filename, Image img, int xres, int yres, int res, int maxiter) {
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
    int r = 0, g = 0, b = 0;
    r = rescale_r_ppm(img, &maxiter, i, r);
    //g = rescale_r_ppm(img, &maxiter, i, g);
    //b = rescale_r_ppm(img, &maxiter, i, b);
    fprintf(ofp, "%d %d %d  ", r, g, b);
    if (i%xres == 0) {
      fprintf(ofp, "\n");
    }
  }
  fclose(ofp);
} 


