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
  int32_t lanes[LANE_SIZE];
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
  //sets for the purpose of manual unrolling
  set savereal;
  set saveimag;

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
void calcloop(mainobj &mainset, __m256i &one);


//Grand refactor: use bitwise ops to increment iters, zreal/zimag.

void cleanup(mainobj &mainset, int *img, int &sentinel, __m256 &zmag2, __m256i &fourvec, __m256i &one, __m256i &eight) {
  //This routine needs to clean finshed lanes and store the iteration count before flagging said lanes as finished.
  //Vector comparison tbh see -> for inspo: comp.vec = (_mm256_cmp_ps(_mm256_add_ps(_mm256_mul_ps(zreal, zreal), _mm256_mul_ps(zimag, zimag)), fourcomp, 2));
  //zmag2 = (_mm256_cmp_ps(_mm256_add_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec)), fourvec, 2));
  for (int i=0; i<LANE_SIZE; i++) {
    float zmag3 = mainset.zreal.lanes[i]*mainset.zreal.lanes[i] + mainset.zimag.lanes[i]*mainset.zimag.lanes[i];
    if (mainset.iters.lanes[i] >= mainset.maxiter || zmag3 >= 4.0) {
      //mainset.iters.vec = _mm256_sub_epi32(mainset.iters.vec, eight);
      while (mainset.iters.lanes[i] < mainset.maxiter && zmag3 < 4.0) {
        //printf("I'm inside a while!");
        mainset.zreal.vec = mainset.savereal.vec;
        mainset.zimag.vec = mainset.saveimag.vec;
        calcloop(mainset, one);
        zmag3 = mainset.zreal.lanes[i]*mainset.zreal.lanes[i] + mainset.zimag.lanes[i]*mainset.zimag.lanes[i];
      }
      uint32_t pxind = mainset.py.lanes[i]*mainset.xres + mainset.px.lanes[i];
      img[pxind] = mainset.iters.lanes[i];
      mainset.iters.lanes[i] = LANE_EMPTY;
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
      mainset.zreal.lanes[i] = 0;
      mainset.zimag.lanes[i] = 0;
      mainset.ztemp.lanes[i] = 0;
      mainset.px.lanes[i] = mainset.x;
      mainset.py.lanes[i] = mainset.y;
      mainset.creal.lanes[i] = (mainset.px.lanes[i]*mainset.recdiv + mainset.cx0);
      mainset.cimag.lanes[i] = (mainset.py.lanes[i]*mainset.imcdiv + mainset.cy0);
      //printf("%f", mainset.cimag.lanes[i]);
    }
  }
}

//finna unroll
void newcalcloop (mainobj &mainset, __m256i &eight) {
  mainset.savereal.vec = mainset.zreal.vec;
  mainset.saveimag.vec = mainset.zimag.vec;
  for (int l=0;l<8;l++) {
    mainset.ztemp.vec =  mainset.zreal.vec;                                   //zreal = ((zreal * zreal) - (zimag * zimag));
    mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));                                                  //zimag = (ztemp * zimag);
    mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec); //zimag += zimag;
    mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec); //adding c
    mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec); //zreal += creal;
    mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec); //zimag += cimag;
  }
  mainset.iters.vec = _mm256_add_epi32(mainset.iters.vec, eight);               // increment iteration count
}

void calcloop(mainobj &mainset, __m256i &one) {
  mainset.ztemp.vec = mainset.zreal.vec;                                   //zreal = ((zreal * zreal) - (zimag * zimag));
  mainset.zreal.vec = _mm256_sub_ps(_mm256_mul_ps(mainset.zreal.vec, mainset.zreal.vec), _mm256_mul_ps(mainset.zimag.vec, mainset.zimag.vec));                                                  //zimag = (ztemp * zimag);
  mainset.zimag.vec = _mm256_mul_ps(mainset.ztemp.vec, mainset.zimag.vec); //zimag += zimag;
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.zimag.vec); //adding c
  mainset.zreal.vec = _mm256_add_ps(mainset.zreal.vec, mainset.creal.vec); //zreal += creal;
  mainset.zimag.vec = _mm256_add_ps(mainset.zimag.vec, mainset.cimag.vec); //zimag += cimag;
  mainset.iters.vec = _mm256_add_epi32(mainset.iters.vec, one);               // increment iteration count
  //printf("Im doing cleanup");
  //printf("%d, %d, %d, %d, %d, %d, %d, %d \n", mainset.iters.lanes[0], mainset.iters.lanes[1], mainset.iters.lanes[2], mainset.iters.lanes[3], mainset.iters.lanes[4], mainset.iters.lanes[5], mainset.iters.lanes[6], mainset.iters.lanes[7]);
}


void initloop(int maxiter, int *img, int xres, int yres) {
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
  __m256 zmag2 = _mm256_set1_ps(0.0);
  __m256i one = _mm256_set1_epi32(1);
  __m256i fourvec = _mm256_set1_epi32(4);
  __m256i eight = _mm256_set1_epi32(8);


  int sentinel = 0;
  while (sentinel < 8) {
    cleanup(mainset, img, sentinel, zmag2, fourvec, one, eight);
    fillset(mainset);
    newcalcloop(mainset, eight);
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
  initloop(4096, img, 1920, 1080);
  pgm(4096, img, 1920, 1080);
  free(img);
}




