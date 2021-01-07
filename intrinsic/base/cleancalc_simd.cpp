#include "proto.h"
#include <cmath>
#include <immintrin.h>

union set {
  __m256 vec;
  float lanes[8];
};

void fill(set pixels, int &x, int &y) {
  for(int i=0;i<8;i++) {
    if(pixels.lanes[i] == -1) {
      pixels.lanes[i] = x*y;
      x++;
      if(x == y) {
        y++;
        x=0;
      }
    }
  }
}

void compmbset(set pixels, set iterations, int x, int y) {
 
}

void initloop(int maxiter, int *img, int xres, int yres) {
  float recdiv = 3.0/xres;
  float imcdiv = 2.4/yres;
  float incrx[8] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
  __m256 incrxvec;
  incrxvec = _mm256_loadu_ps(incrx);
  incrxvec = _mm256_mul_ps(incrxvec, _mm256_set1_ps(imcdiv));
  __m256 zerovec;
  zerovec = _mm256_setzero_ps();
  float zr=0, zi=0;
  set iters;
  set comp;
  set pxlcnt;
  comp.vec = _mm256_setzero_ps();
  pxlcnt.vec = _mm256_set1_ps(-1.0);
  int xcoord = 0;
  int ycoord = 0;
  while(xcoord*ycoord < (xres*yres)){
    fill(pxlcnt, xcoord, ycoord);
    compmbset(pxlcnt, iters, xcoord, ycoord);
  }
}

void pgm(int maxiter, int *img, int xres, int yres) {
    const char filename[1024] = "haha.pgm";
    FILE *ofp;
    if ((ofp = fopen(filename, "w")) == NULL) {
    perror("FAILURE");
    return;
    }
    fprintf(ofp, "P2\n");
    fprintf(ofp, "%d %d \n", xres, yres);
    fprintf(ofp, "%d \n", maxiter);
    for (int i = 0; i < xres*yres; i++) {
    if(i%xres == 0) {
        fprintf(ofp, "\n");
    }
    fprintf(ofp, "%d ", img[i]);
    }
    printf("size of img is %d \n", sizeof(img)/sizeof(int));
}

int main() {
    int *img = (int*) malloc(1024*768*sizeof(int));
    initloop(4096, img, 1024, 768);
    pgm(4096, img, 1024, 768);
    free(img);
}
