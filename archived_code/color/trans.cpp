#include <cstdio>
#include <cstring>
#include <vector>
#include <iostream>
#include <math.h>
#include <string.h>
#include <cstdlib>



int main() {
  int res = 1024*768;
  //Read in old file here
  FILE *ifp;
  if ((ifp = fopen("test.ppm", "r")) == NULL) {
    perror("Failed to open output file\n");
    return 0;
  }


  int *pxl= (int*) malloc(res*3*sizeof(int));
  int doesntmatter;
  char whocares;
  fscanf(ifp, "%c%d\n", &whocares, &doesntmatter);
  fscanf(ifp, "%d %d \n", &doesntmatter, &doesntmatter);
  fscanf(ifp, "%d\n", &doesntmatter); 
  int i, temp;
  while( fscanf(ifp, "%d", &temp) != EOF) {
    pxl[i] = temp;
    i++;
  }

  fclose(ifp);
  char filename[1024];
  strcpy(filename, "newpxl.ppm");
  printf("Writing Mandelbrot set to filename '%s'  ... \n", filename);
  FILE *ofp;
  if ((ofp = fopen(filename, "w")) == NULL) {
    perror("Failed to open output file\n");
    return 0;
  }
  //header
  fprintf(ofp, "P3\n");
  fprintf(ofp, "%d %d \n", 1024, 768);
  fprintf(ofp, "%d \n", 255);
  for (int i = 0; i < res*3; i+=3) {
    int r, g, b;
    r = pxl[i];
    g = pxl[i+1];
    b = pxl[i+2];
    fprintf(ofp, "%d %d %d  ", r, g, b);
    if ((i*3)%1024== 0) {
      fprintf(ofp, "\n");
    }
  }

}
