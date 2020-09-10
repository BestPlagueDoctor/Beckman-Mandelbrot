//This version's goal is to use bitwise ops to avoid backstep overhead
//For example iter_vec += (vec_of_1s & too_far) | (vec_of_8s & ~too_far)
//And/Or, mainset.zreal.vec = (saved_after_1_calc & too_far) | (mainset.zreal.vec & ~too_far)
//Basically, the iteration incrementing as well as all other saved values can be incremented using bitwise ops to keep all lanes running at max efficiency
//Next step is to use fixed point

#include "proto.h"
#include <cmath>
#include <stdio.h>
#include <immintrin.h>
#include <cstdint>

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
  int32_t lanes[LANE_SIZE];
};

struct mainobj {
  // iters is the number of iterations the vector has undergone
  intset iters;

  // lanespeed is a flag to mark bailed lanes
  set lanespeed;

  // creal-ztemp are the operands of the set
  set creal;
  set cimag;
  set zreal;
  set zimag;
  set ztemp;
  set zrsq;
  set zisq;
  set zmag2;
  //sets for the purpose of manual unrolling
  set savereal;
  set saveimag;

  intset px; // pixel associated with this lane
  intset py; // pixel associated with this lane

  // maxiter is the bailout term, once past maxiter we no longer care to process more precision
  uint32_t maxiter;

  // x and y represent current progress through the img,
  // the next pixel to be processed in the "work queue"...
  uint32_t x;
  uint32_t y;

  //imcdiv/recdiv are the ratio of range/resolution, giving positional location
  uint32_t xres;
  uint32_t yres;
  uint32_t numpixels;
  float imcdiv;
  float recdiv;

  // Cartesian presets, the edges of the graph
  float cx0;
  float cy0;
  float cx1;
  float cy1;
  float cw;
  float ch;
};


void calcloop(mainobj &mainset) {
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zimag.vec = _mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec));
  mainset.savereal.vec = mainset.zreal.vec; mainset.saveimag.vec = mainset.zimag.vec;
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zimag.vec = _mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec));
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zimag.vec = _mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec));
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zimag.vec = _mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec));
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zimag.vec = _mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec));
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zimag.vec = _mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec));
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zimag.vec = _mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec));
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zimag.vec = _mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec));
}


void cleanup(mainobj &mainset, int *img, int &sentinel, __m256i &four, __m256i &one, __m256i &eight) {
  mainset.zmag2.vec = _mm256_add_ps(mainset.zrsq.vec, mainset.zisq.vec);
  for (int i=0; i<8; i++) {
    //do this as a vector comp?
    if (mainset.iters.lanes[i] > mainset.maxiter || mainset.zmag2.lanes[i] >= 4.0) {
      mainset.lanespeed.lanes[i] = 1;
      mainset.iters.lanes[i] -= 8;
    }
  }

    mainset.iters.vec += _mm256_or_si256(_mm256_and_si256(one, _mm256_castps_si256(mainset.lanespeed.vec)), _mm256_andnot_si256(_mm256_castps_si256(mainset.lanespeed.vec), eight));
    //printf("%d, %d, %d, %d, %d, %d, %d, %d \n", mainset.iters.lanes[0], mainset.iters.lanes[1], mainset.iters.lanes[2], mainset.iters.lanes[3], mainset.iters.lanes[4], mainset.iters.lanes[5], mainset.iters.lanes[6], mainset.iters.lanes[7]);
    mainset.zreal.vec =  _mm256_or_ps(_mm256_and_ps(mainset.savereal.vec, mainset.lanespeed.vec), _mm256_andnot_ps(mainset.lanespeed.vec, mainset.zreal.vec));
    mainset.zimag.vec =  _mm256_or_ps(_mm256_and_ps(mainset.saveimag.vec, mainset.lanespeed.vec), _mm256_andnot_ps(mainset.lanespeed.vec, mainset.zimag.vec));

  for (int i=0; i<8; i++) {
    if (mainset.iters.lanes[i] == mainset.maxiter) {
      //printf("Im equal!");
      //printf("%d", mainset.iters.lanes[i]);
      uint32_t pxind = mainset.py.lanes[i]*mainset.xres + mainset.px.lanes[i];
      //printf("%d\n", int(pxind));
      img[pxind] = mainset.iters.lanes[i];
      mainset.iters.lanes[i] = LANE_EMPTY;
      mainset.lanespeed.lanes[i] = 0;
      if (int(pxind) >= (mainset.numpixels - 8)) {
        printf("hello");
        mainset.iters.lanes[i] = LANE_FINISHED;
        sentinel++;
      }
    }
  }
}

void fillset(mainobj &mainset) {
  //This routine stages all lanes, filling pixels, XY coords, and scales C numbers before operation.
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

void init (int maxiter, int *img, int xres, int yres) {
  mainobj mainset;
  mainset.iters.vec = _mm256_set1_epi32(LANE_EMPTY);
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
  __m256i one = _mm256_set1_epi32(1);
  __m256i four = _mm256_set1_epi32(4);
  __m256i eight = _mm256_set1_epi32(8);

  mainset.lanespeed.vec = _mm256_set1_ps(0);
  int sentinel = 0;
  while (sentinel < 8) {
    cleanup(mainset, img, sentinel, four, one, eight);
    fillset(mainset);
    calcloop(mainset);
  }
}


void pgm(int maxiter, int *img, int xres, int yres) {
  const char filename[1024] = "smol.pgm";
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
  int *img = (int*) malloc(1920*1080*sizeof(int));
  init(4096, img, 1920, 1080);
  pgm(4096, img, 1920, 1080);
  free(img);
}
