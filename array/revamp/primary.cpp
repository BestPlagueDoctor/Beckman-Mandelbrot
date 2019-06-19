//This is the main file that calls all other functions (as of right now)
#include "proto.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string.h>


int main(int argc, char **argv) {
  printf("Hello world, I will rise\n");
  int maxiter = 4096, xres = 1024, yres = 768;
  float xmin = -2.0, xmax = 1.0, ymin = -1.2, ymax = 1.2;
  int res = xres*yres;
  //outputmode being skipped for now
  char filename[1024];
  strcpy(filename, "test.pgm");
  /*for (int arg=1; arg<argc;arg++) {
    if (!strcmp(argv[arg], "-h") || !strcmp(argv[arg], "--help")) {
      printf("Usage: \n");
      printf("%s [-pgm filename.pgm] [-maxiter -N] \\ \n", argv[0]);
      printf("   [-xres XXX] [-yres YYY]\n");
    }
    if (!strcmp(argv[arg], "-xres")) {
      arg++;
      xres = atoi(argv[arg]);
    }
    if (!strcmp(argv[arg], "-yres")) {
      arg++;
      yres = atoi(argv[arg]);
    }
    if (!strcmp(argv[arg], "-maxiter")) {
      arg++;
      maxiter = atoi(argv[arg]);
    }
    if (!strcmp(argv[arg], "-pgm")) {
      arg++;
      strcpy(filename, argv[arg]);
      printf("Will output to PGM file: '%s'\n", filename);
    }
  }
  */
  printf("'%s': Running Mandelbrot set with params:\n", argv[0]);
  printf(" xres: %d yres: %d maxiter %d\n", xres, yres, maxiter);
  printf(" xmin: %.2f xmax: %.2f ymin %.2f ymax %.2f\n", xmin, xmax, ymin, ymax);
  printf("----------------------------------------------------------\n");
  //allocate memory
  //call calcs(calc file, using arrays)
  //mandlebrot(img, xmin, ymin, xmax, ymax, xres, yres, maxiter);
  //rescale colors (image processing)
  //rescale_colors_log(img, xres, yres, &maxiter);
  //rescale_colors_log_log(img, xres, yres, &maxiter);
  //write colors (image) to disk
  //write_pgm(filename, img, xres, yres, maxiter)
  //free memory
  int *img= (int*) malloc(res*sizeof(int));
  //calc
  //printcoord(img, xres, yres, res);
  imgmandel(maxiter, img, res, xres, yres);
  rescale_colors_pgm(img, res, maxiter);
  write_pgm(filename, img, xres, yres, res, maxiter);
  free(img);
  return 0;
}
