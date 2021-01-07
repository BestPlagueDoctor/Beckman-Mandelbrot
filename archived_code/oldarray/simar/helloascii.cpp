#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>

int findy(int xscl,int i) {
  int ycoord = floor(i / xscl);
  return ycoord; 
}

int findx(int xscl, int i) {
  int xcoord = floor(i%xscl);
  return xcoord;
}

int findidx(int xscl, int x, int y) {
  return y*xscl + x;
}

void setvals(int* ptr, int xscl, int yscl) {
  int x, y;
  for (y = 0; y < yscl; y++) {
    for (x = 0; x < xscl; x++) {
      int idx = findidx(xscl, x, y);
      ptr[idx] = idx;
    }
  }
}


int checkvals(int* ptr, int xscl, int yscl) {
  int i, x, y;
  int sz = xscl*yscl;
  for (i = 0; i < sz; i++) {
    x = findx(xscl, i); 
    y = findy(xscl, i);
    int idx = findidx(xscl, x, y);
    if (i != idx) {
      printf("indicies unequal, bailing out \n");
      return -1;
    }
    printf("(%d %c %d)", x, ',', y); 
    if (x == (xscl-1)) {
      printf("\n");
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  int i = 0, xscl = 10, yscl = 10;
  for (int arg = 1; arg < argc; arg++) {
    if (!strcmp(argv[arg], "-d")) {
      arg++;
      xscl = atoi(argv[arg]);
      arg++;
      yscl = atoi(argv[arg]);
    }
  }
 
  int sz = xscl * yscl;
#if 1
  int *ptr = new int[sz];
#elif 1
  int *ptr = (int*) malloc(sz*sizeof(int));
  memset(ptr, 0, sz * sizeof(int));
#elif 1
  int *ptr = (int*) calloc(1, (sz*sizeof(int));
#endif
  setvals(ptr, xscl, yscl);
  if (checkvals(ptr, xscl, yscl)) {
    return -1;
  }

#if 0
  free(ptr);
#elif 1
  delete [] ptr;
#endif

  return 0;
}
