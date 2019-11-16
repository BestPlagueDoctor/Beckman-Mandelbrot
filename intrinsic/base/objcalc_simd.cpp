#include "proto.h"
#include <cmath>
#include <immintrin.h>

union set {
  __m256 vec;
  float lanes[8];
};

struct mainobj {
  set vector;
  set iters;
  set pixels;
  set creal;
  set cimag;
  set zreal;
  set zimag;
  set ztemp;
};

void fillset(mainobj mainset, int &x, int &y, int xres, float imcdiv, float recdiv, int *img) {
  for(int i=0;i<8;i++) {
    if(mainset.iters.lanes[i] == -1) {
      img[int(mainset.pixels.lanes[i])] = int(mainset.iters.lanes[i]);
      mainset.pixels.lanes[i] = x;
      mainset.iters.lanes[i] = 0;
      x++;
      if(x >= xres) {
        x=0;
        y++;
      }
      mainset.creal.lanes[i] = ((y*imcdiv)-1.2);
      mainset.cimag.lanes[i] = ((x*recdiv)-2.0);
      mainset.zreal.lanes[i] = 0;
      mainset.zimag.lanes[i] = 0;
      mainset.ztemp.lanes[i] = 0;
    }
  }
}

bool isempty(set iters) {
  for (int i=0; i<8; i++) {
    if (iters.lanes[i] == -1) {
      return true;
    }
    else {
      return false;
    }
  }
}

void calcloop(mainobj mainset, int &x, int &y, int xres, float imcdiv, float recdiv, __m256 one, int *img) {
  mainset.ztemp.vec = mainset.zreal.vec;
  //zreal = ((zreal * zreal) - (zimag * zimag));
  mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));
  //zimag = (ztemp * zimag);
  mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec);
  //zimag += zimag;
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec);
  //adding c
  //zreal += creal;
  mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec);
  //zimag += cimag;
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec);
  mainset.iters.vec = _mm256_add_ps(mainset.iters.vec, one);
  fillset(mainset, x, y, xres, imcdiv, recdiv, img);
}

void initloop(int maxiter, int *img, int xres, int yres) {
  float recdiv = 3.0/xres;
  float imcdiv = 2.4/yres;
  float incrx[8] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
  __m256 incrxvec;
  incrxvec = _mm256_loadu_ps(incrx);
  incrxvec = _mm256_mul_ps(incrxvec, _mm256_set1_ps(imcdiv));
  __m256 one = _mm256_set1_ps(1.0);
  __m256 zerovec;
  zerovec = _mm256_setzero_ps();
  int xcoord = 0;
  int ycoord = 0;
  mainobj mainset;
  mainset.pixels.vec = _mm256_set1_ps(-1.0);
  fillset(mainset, xcoord, ycoord, xres, imcdiv, recdiv, img);
  while ((xcoord*ycoord)<(xres*yres)) {
    calcloop(mainset, xcoord, ycoord, xres, imcdiv, recdiv, one, img);
    //TODO storefunc;
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
