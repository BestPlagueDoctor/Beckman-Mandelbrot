#include <immintrin.h>
#include <stdio.h>
#include <string.h>

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
  for(int i = 0; i <= 8; i++) {

  }
}
