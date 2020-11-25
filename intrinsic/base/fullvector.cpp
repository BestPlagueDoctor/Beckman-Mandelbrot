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
  intset lanespeed;
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

void calcloop(mainobj &mainset) {
  mainset.savereal.vec = mainset.zreal.vec;
  mainset.saveimag.vec = mainset.zimag.vec;
  mainset.ztemp.vec = mainset.zreal.vec;
  mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));
  mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec);
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec);
  mainset.ztemp.vec = mainset.zreal.vec;
  mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));
  mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec);
  mainset.ztemp.vec = mainset.zreal.vec;
  mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));
  mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec);
  mainset.ztemp.vec = mainset.zreal.vec;
  mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));
  mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec);
  mainset.ztemp.vec = mainset.zreal.vec;
  mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));
  mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec);
  mainset.ztemp.vec = mainset.zreal.vec;
  mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));
  mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec);
  mainset.ztemp.vec = mainset.zreal.vec;
  mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));
  mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec);
  mainset.ztemp.vec = mainset.zreal.vec;
  mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));
  mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec);
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec);
  mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec);
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec);
}


void cleanup(mainobj &mainset, int *img, int &sentinel, __m256 &four, __m256i &one, __m256i &eight, __m256i &zero) {
  mainset.zmag2.vec = _mm256_add_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));
  //printf("Zmag2: %f, %f, %f, %f, %f, %f, %f, %f \n", mainset.zmag2.lanes[0], mainset.zmag2.lanes[1], mainset.zmag2.lanes[2], mainset.zmag2.lanes[3], mainset.zmag2.lanes[4], mainset.zmag2.lanes[5], mainset.zmag2.lanes[6], mainset.zmag2.lanes[7]);
  mainset.magcmp.vec = _mm256_castps_si256(_mm256_cmp_ps(mainset.zmag2.vec, four, _CMP_NLE_UQ));
  //printf("Magcmp: %d, %d, %d, %d, %d, %d, %d, %d \n", mainset.magcmp.lanes[0], mainset.magcmp.lanes[1], mainset.magcmp.lanes[2], mainset.magcmp.lanes[3], mainset.magcmp.lanes[4], mainset.magcmp.lanes[5], mainset.magcmp.lanes[6], mainset.magcmp.lanes[7]);
  mainset.lanespeed.vec = mainset.magcmp.vec;
  //mainset.iters.vec = _mm256_sub_epi32(mainset.iters.vec, _mm256_and_si256(mainset.lanespeed.vec, eight));
  __m256i maxitervec = _mm256_set1_epi32(4096);
  //Need to use a different check than <4096
  mainset.isreset.vec = _mm256_add_epi32(mainset.isreset.vec, _mm256_and_si256(_mm256_and_si256(_mm256_cmpgt_epi32(mainset.iters.vec, maxitervec), mainset.lanespeed.vec), one));
  mainset.iters.vec = _mm256_sub_epi32(mainset.iters.vec, _mm256_and_si256(_mm256_cmpeq_epi32(mainset.isreset.vec, one), eight));
  mainset.iters.vec = _mm256_add_epi32(mainset.iters.vec, _mm256_or_si256(_mm256_and_si256(mainset.lanespeed.vec, one), _mm256_andnot_si256(mainset.lanespeed.vec, eight)));
  mainset.zreal.vec = _mm256_or_ps(_mm256_and_ps(_mm256_castsi256_ps(mainset.lanespeed.vec), mainset.savereal.vec), _mm256_andnot_ps(_mm256_castsi256_ps(mainset.lanespeed.vec), mainset.zreal.vec));
  mainset.zimag.vec = _mm256_or_ps(_mm256_and_ps(_mm256_castsi256_ps(mainset.lanespeed.vec), mainset.saveimag.vec), _mm256_andnot_ps(_mm256_castsi256_ps(mainset.lanespeed.vec), mainset.zimag.vec));
  mainset.zmag2.vec = _mm256_add_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));
  //printf("Iters: %d, %d, %d, %d, %d, %d, %d, %d \n", mainset.iters.lanes[0], mainset.iters.lanes[1], mainset.iters.lanes[2], mainset.iters.lanes[3], mainset.iters.lanes[4], mainset.iters.lanes[5], mainset.iters.lanes[6], mainset.iters.lanes[7]);
  mainset.magcmp.vec = _mm256_castps_si256(_mm256_cmp_ps(mainset.zmag2.vec, four, _CMP_NLE_UQ));

  for (int i=0; i<8; i++) {
    //replace this with a true magnitude check
    if ((mainset.iters.lanes[i] >= mainset.maxiter || mainset.magcmp.lanes[i] == -1) && mainset.isreset.lanes[i] != 0) {
      int pxind = mainset.py.lanes[i]*mainset.xres + mainset.px.lanes[i];
      img[pxind] = mainset.iters.lanes[i];
      //printf("%d\n", pxind);
      mainset.iters.lanes[i] = LANE_EMPTY;
      mainset.lanespeed.lanes[i] = 0;
      mainset.isreset.lanes[i] = 0;
      if (pxind >= (mainset.numpixels - 8)) {
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
