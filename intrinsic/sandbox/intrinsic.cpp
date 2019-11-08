#include <immintrin.h>
#include <stdio.h>
#include <string.h>

union alley {
  __m256 vec;
  struct {
    float lanes[8];
  };
};

int main() {
  alley arrowhead;
  for(int i=0;i<8;i++){
    arrowhead.lanes[i] = i*31.1;
  }
  for(int i=0;i<8;i++){
    printf("%.2f   ", arrowhead.vec[i]);
  }
}

union zset {
  __m256 zreal;
  __m256 zimag;
  int iters[8];
};
