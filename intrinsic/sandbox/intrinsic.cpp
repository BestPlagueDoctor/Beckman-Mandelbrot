#include <immintrin.h>
#include <cstdio>

int main() {
  __m256 test1 = {1, 2, 3, 4};
  __m256 test2 = {1, 2, 3, 4};
  __m256 test3 = _mm256_add_ps (test1, test2);
  printf("%.2f %.2f %.2f %.2f", test3[0], test3[1], test3[2], test3[3]);
  
}
