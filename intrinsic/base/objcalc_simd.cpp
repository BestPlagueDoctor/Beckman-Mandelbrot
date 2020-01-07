#include "proto.h"
#include <cmath>
#include <stdio.h>
#include <immintrin.h>

#define CHECK_ERR { if (notborked) ldjflksjdflkjdsflkj; }
#define LANE_SIZE      8
#define COUNTERINIT    0
#define LANE_EMPTY    -1.0
#define LANE_FINISHED -2.0
#define X_SCALE      2.0
#define Y_SCALE      1.2
#define X_AXIS       3.0
#define Y_AXIS       2.4 

union set {
  __m256 vec;
  float lanes[LANE_SIZE];
};

struct mainobj {
  //iters is the number of iterations the vector has undergone
  set iters;
  //creal-ztemp are the operands of the set
  set creal;
  set cimag;
  set zreal;
  set zimag;
  set ztemp;
  //x and y represent current progress through the res
  float x;
  float y;
};


void fillset(mainobj &mainset, int xres, float imcdiv, float recdiv) {
  //This routine needs to be fixed so that it doesn't suck
  for(int i=0;i<LANE_SIZE;i++) {
    if (mainset.iters.lanes[i] == LANE_EMPTY) {
      mainset.x++;
      if(mainset.x == xres) {
        mainset.x = 0;
        mainset.y++;
      }
      mainset.iters.lanes[i] = 0;
      //printf("%d\n", mainset.x);
      mainset.zreal.lanes[i] = 0;
      mainset.zimag.lanes[i] = 0;
      mainset.ztemp.lanes[i] = 0;
      mainset.creal.lanes[i] = ((mainset.y*imcdiv)-Y_AXIS);
      mainset.cimag.lanes[i] = ((mainset.x*recdiv)-X_AXIS);
    }
    if (mainset.iters.lanes[i] == LANE_FINISHED) {
      //filler
    }
  }
}

void cleanup(mainobj &mainset, int xres, int yres, int *img, int maxiter, int &sentinal) {
  //This routine needs to clean finshed lanes and store the iteration count before flagging said lanes as finished.
  for (int i=0;i<LANE_SIZE;i++) {
    if (mainset.iters.lanes[i] == maxiter || (mainset.zreal.lanes[i] * mainset.zreal.lanes[i] \
         + mainset.zimag.lanes[i] + mainset.zimag.lanes[i]) > 4.0) {
      img[(int(mainset.y)*xres)+int(mainset.x)+i] = mainset.iters.lanes[i];
//      printf("%d\n", mainset.iters.lanes[i]);
      mainset.iters.lanes[i] = LANE_EMPTY;
      if (mainset.y*xres+mainset.x+i >= ((xres*yres)-8)) {
        mainset.iters.lanes[i] = LANE_FINISHED;
        sentinal++;
        printf("%d\n", sentinal);
      }
    }
  }
}

void calcloop(mainobj &mainset, __m256 one) {
  //may want to look into this actually
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
  //fillset(mainset, xres, imcdiv, recdiv, img); ????
}


void initloop(int maxiter, int *img, int xres, int yres) {
  //The init here needs to be cleaned and more readable
  printf("Im an init and i go here");
  float recdiv = 3.0/xres;
  float imcdiv = 2.4/yres;
  //float incrx[LANE_SIZE] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
  //__m256 incrxvec;
  //incrxvec = _mm256_loadu_ps(incrx);
  //incrxvec = _mm256_mul_ps(incrxvec, _mm256_set1_ps(imcdiv));
  __m256 one = _mm256_set1_ps(1.0);
  mainobj mainset;
  mainset.x = 0;
  mainset.y = 0;
  //AYYYYY this dont work at all
  mainset.iters.vec = _mm256_set1_ps(-1.0);
  int sentinal = 0;
  while (sentinal < 8) {
    cleanup(mainset, xres, yres, img, maxiter, sentinal);
    fillset(mainset, xres, imcdiv, recdiv);
    calcloop(mainset, one);
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
  //printf("size of img is %d \n", sizeof(img)/sizeof(int));
}

int main() {
  int *img = (int*) malloc(1024*768*sizeof(int));
  initloop(4096, img, 1024, 768);
  pgm(4096, img, 1024, 768);
  free(img);
}




