//Unroll time
#include "proto.h"
#include <cmath>
#include <stdio.h>
#include <immintrin.h>
#include <cstdint>

#define CHECK_ERR { if (notborked) ldjflksjdflkjdsflkj; }
#define LANE_SIZE      8
#define COUNTERINIT    0
#define LANE_EMPTY    -1
#define LANE_FINISHED -2
#define X_SCALE      2.0
#define Y_SCALE      1.2
#define X_AXIS       3.0
#define Y_AXIS       2.4 

union set {
  __m256 vec;
  float lanes[LANE_SIZE];
};

union intset {
  __m256i vec;
  int32_t lanes[8];
};

struct mainobj {
  // iters is the number of iterations the vector has undergone
  intset iters;

  // creal-ztemp are the operands of the set
  set creal;
  set cimag;
  set zreal;
  set zimag;
  set ztemp;

  intset px; // pixel associated with this lane
  intset py; // pixel associated with this lane

  uint32_t maxiter;

  // x and y represent current progress through the img,
  // the next pixel to be processed in the "work queue"...
  uint32_t x;
  uint32_t y;

  uint32_t xres;
  uint32_t yres;
  uint32_t numpixels;
  float imcdiv;
  float recdiv;

  float cx0;
  float cy0;
  float cx1;
  float cy1;
  float cw;
  float ch;
};


void cleanup(mainobj &mainset, int *img, int &sentinel, __m256 &zmag2, __m256 &fourvec) {
  //This routine needs to clean finshed lanes and store the iteration count before flagging said lanes as finished.
  //Vector comparison tbh see -> for inspo: comp.vec = (_mm256_cmp_ps(_mm256_add_ps(_mm256_mul_ps(zreal, zreal), _mm256_mul_ps(zimag, zimag)), fourcomp, 2));
  //zmag2 = (_mm256_cmp_ps(_mm256_add_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec)), fourvec, 2));
  for (int i=0; i<LANE_SIZE; i++) {
    float zmag3 = mainset.zreal.lanes[i]*mainset.zreal.lanes[i] + mainset.zimag.lanes[i]*mainset.zimag.lanes[i];
    if (mainset.iters.lanes[i] >= mainset.maxiter || zmag3 > 4.0) {
      uint32_t pxind = mainset.py.lanes[i]*mainset.xres + mainset.px.lanes[i];
      img[pxind] = mainset.iters.lanes[i];
      mainset.iters.lanes[i] = LANE_EMPTY;
      if (pxind >= (mainset.numpixels - 8)) {
        mainset.iters.lanes[i] = LANE_FINISHED;
        sentinel++;}
    }
  }
}


void fillset(mainobj &mainset) {
  for (int i=0; i<LANE_SIZE; i++) {
    if (mainset.iters.lanes[i] == LANE_EMPTY) {
      mainset.x++;
      if(mainset.x == mainset.xres) {
        mainset.x = 0;
        mainset.y++;
      }

      mainset.iters.lanes[i] = 0;
      mainset.zreal.lanes[i] = 0;
      mainset.zimag.lanes[i] = 0;
      mainset.ztemp.lanes[i] = 0;
      mainset.px.lanes[i] = mainset.x;
      mainset.py.lanes[i] = mainset.y;
      mainset.creal.lanes[i] = (mainset.px.lanes[i]*mainset.recdiv + mainset.cx0);
      mainset.cimag.lanes[i] = (mainset.py.lanes[i]*mainset.imcdiv + mainset.cy0);
    }
  }
}


void calcloop(mainobj &mainset) {
  __m256i one = _mm256_set1_epi32(1);
  mainset.ztemp.vec = mainset.zreal.vec;                                   //zreal = ((zreal * zreal) - (zimag * zimag));
  mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));                                                  //zimag = (ztemp * zimag);
  mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec); //zimag += zimag;
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec); //adding c
  mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec); //zreal += creal;
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec); //zimag += cimag;
  mainset.iters.vec = _mm256_add_epi32(mainset.iters.vec, one);               // increment iteration count
  //printf("%d, %d, %d, %d, %d, %d, %d, %d \n", mainset.iters.vec[0], mainset.iters.vec[1], mainset.iters.vec[2], mainset.iters.vec[3], mainset.iters.vec[4], mainset.iters.vec[5], mainset.iters.vec[6], mainset.iters.vec[7]);
}


void initloop(int maxiter, int *img, int xres, int yres) {
  mainobj mainset;
  mainset.iters.vec = _mm256_set1_epi32(1);
  printf("%d, %d, %d, %d, %d, %d, %d, %d \n", mainset.iters.vec[0], mainset.iters.vec[1], mainset.iters.vec[2], mainset.iters.vec[3], mainset.iters.vec[4], mainset.iters.vec[5], mainset.iters.vec[6], mainset.iters.vec[7]);
  mainset.maxiter = maxiter;
  mainset.x = 0;
  mainset.y = 0;
  mainset.xres = xres;
  mainset.yres = yres;  
  mainset.numpixels = xres*yres;
  mainset.px.vec = _mm256_set1_epi32(0);
  mainset.py.vec = _mm256_set1_epi32(0);

  // initialize cartesian coordinate window
  mainset.cx0 = -2;
  mainset.cy0 = -1.2;
  mainset.cx1 = 1;
  mainset.cy1 = 1.2;
  mainset.cw = mainset.cx1-mainset.cx0; 
  mainset.ch = mainset.cy1-mainset.cy0; 

  mainset.recdiv = mainset.cw/mainset.xres;
  mainset.imcdiv = mainset.ch/mainset.yres;

  // initialize comparison vectors
  __m256 zmag2 = _mm256_set1_ps(0.0);
  __m256 fourvec = _mm256_set1_ps(4.0);


  int sentinel = 0;
  while (sentinel < 8) {
    cleanup(mainset, img, sentinel, zmag2, fourvec);
    fillset(mainset);
    calcloop(mainset);
  }
}


void pgm(int maxiter, int *img, int xres, int yres) {
  const char filename[1024] = "mande.pgm";
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
}

int main() {
  int *img = (int*) malloc(1024*768*sizeof(int));
  initloop(4096, img, 1024, 768);
  pgm(4096, img, 1024, 768);
  free(img);
}




