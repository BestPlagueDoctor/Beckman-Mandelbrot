#include "proto.h"
#include <stdio.h>

void mandlebrot_pgm(float xmin, float ymin, float xmax, float ymax, int xres, int yres, int maxiter, const char *filename) {
  printf("Writing Mandelbrot set to filename '%s'....\n", filename);
  FILE *ofp;
  if ((ofp = fopen(filename, "w")) == NULL) {
    perror("Failed to open output file!\n");
    return;
  }
  //pgm file header (magic number n' res etc) 
  fprintf(ofp, "P2\n");
  fprintf(ofp, "%d %d \n", xres, yres);
  int maxval = maxiter + 1;
  //Need code to handle logscale here
  fprintf(ofp, "%d \n", maxval);
  float xinc = (xmax - xmin) / float(xres);  
  float yinc = (ymax - ymin) / float(yres);
  int y;
  float imc;
  for (y=0, imc=ymin; y<yres; y++,imc+=yinc) {
    int x;
    float rec;
      for (x=0, rec=xmin; x<xres; x++,rec+=xinc) {
        int iter=pointcheck(maxiter, rec, imc); 
      //Need more logscale handling here
      fprintf(ofp, "%d ", maxval-iter);
      }
    fprintf(ofp, "\n");
    printf("Calculating line: %d            \r", y);
    fflush(stdout);
  }
  fclose(ofp);
}

