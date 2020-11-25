//Hard scrap, new plan is to fully vectorize
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


//Union set is a vector of 32bit floats
union set {
  __m256 vec;
  float lanes[LANE_SIZE];
};

//Union intset is a vector is signed 32bit ints, for when numbers had better be whole
union intset {
  __m256i vec;
  int32_t lanes[LANE_SIZE];
};

struct mainobj {
  // iters is the number of iterations the vector has undergone
  intset iters;

  // lanespeed is a flag to mark bailed lanes
  intset magcmp;

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

void calcloop(mainobj &mainset, __m256i &eight) {
  mainset.savereal.vec = mainset.zreal.vec;
  mainset.saveimag.vec = mainset.zimag.vec;
  mainset.zimag.vec = _mm256_add_ps(_mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), mainset.zimag.vec), mainset.creal.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(_mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), mainset.zimag.vec), mainset.creal.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(_mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), mainset.zimag.vec), mainset.creal.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(_mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), mainset.zimag.vec), mainset.creal.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(_mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), mainset.zimag.vec), mainset.creal.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(_mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), mainset.zimag.vec), mainset.creal.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(_mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), mainset.zimag.vec), mainset.creal.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(_mm256_mul_ps(_mm256_add_ps(mainset.zreal.vec, mainset.zreal.vec), mainset.zimag.vec), mainset.creal.vec);
  mainset.zreal.vec = _mm256_add_ps(_mm256_sub_ps(mainset.zrsq.vec, mainset.zisq.vec), mainset.creal.vec);
  mainset.zrsq.vec =  _mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec);
  mainset.zisq.vec =  _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.iters.vec = _mm256_add_epi32(mainset.iters.vec, eight);
}


void cleanup(mainobj &mainset, int *img, int &sentinel, __m256 &four, __m256i &one, __m256i &eight, __m256i &zero) {
  mainset.zmag2.vec = _mm256_add_ps(mainset.zrsq.vec, mainset.zisq.vec);
  mainset.magcmp.vec = _mm256_cmp_ps(mainset.zmag2.vec, mainset.four.vec, _CMP_NLE_OQ); 
  mainset.iters.vec = _mm256_add_epi32()

  
  
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
      mainset.zreal.lanes[i] = 0.0;
      mainset.zimag.lanes[i] = 0.0;
      mainset.ztemp.lanes[i] = 0.0;
      mainset.zmag2.lanes[i] = 0.0;
      //mainset.zrsq.lanes[i] = 0;
      //mainset.zisq.lanes[i] = 0;
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
  mainset.magcmp.vec = _mm256_set1_epi32(0);
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
  __m256i zero = _mm256_set1_epi32(0);
  __m256i one = _mm256_set1_epi32(1);
  __m256 four = _mm256_set1_ps(4.0);
  __m256i eight = _mm256_set1_epi32(8);


  mainset.lanespeed.vec = _mm256_set1_epi32(0);
  int sentinel = 0;
  mainset.zrsq.vec = _mm256_set1_ps(0);
  mainset.zisq.vec = _mm256_set1_ps(0);
  fillset(mainset);
  //calcloop(mainset);
  while (sentinel < 8) {
    cleanup(mainset, img, sentinel, four, one, eight, zero);
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
