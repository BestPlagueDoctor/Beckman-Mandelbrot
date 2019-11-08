#include <immintrin.h>
#include <stdio.h>
#include <string.h>

union alley {
  __m256 vec;
  struct {
    float lanes[8];
  };
};

union lane {
  __m256 vec;
  float lane1 = 0.0;
  float lane2 = 0.0;
  float lane3 = 0.0;
  float lane4 = 0.0;
  float lane5 = 0.0;
  float lane6 = 0.0;
  float lane7 = 0.0;
  float lane8 = 0.0;
};

__m256 test = _mm256_set1_ps(5.0);

lane LANE;
int main() {
  alley arrowhead;
  for(int i=0;i<8;i++){
    arrowhead.lanes[i] = i*31.1;
  }
  for(int i=0;i<8;i++){
    printf("%.2f   ", arrowhead.vec[i]);
  for(int i = 0; i <= 8; i++) {

  }
}

union zset {
  __m256 zreal;
  __m256 zimag;
  int iters[8];
};
