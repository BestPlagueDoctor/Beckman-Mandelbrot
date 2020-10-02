#include <cmath>
#include <stdio.h>
#include <immintrin.h>
#include <cstdint>

union set {
  __m256 vec;
  float lanes[8];
};

union intset {
  __m256i vec;
  int32_t lanes[8];
};



int main() {
  intset iters;
  intset lanespeed;
  __m256i one = _mm256_set1_epi32(1);
  __m256i eight = _mm256_set1_epi32(8);
  int maxiter = 128;
  int testarray[8] = {1,5,7,3,5,7,4,11};
  int testspeed[8] = {0, -1, 0, -1, 0, -1, 0, -1};
  iters.vec = _mm256_loadu_si256((__m256i*) testarray);
  lanespeed.vec = _mm256_loadu_si256((__m256i*)testspeed);
  iters.vec += _mm256_or_si256(_mm256_and_si256(one, lanespeed.vec), _mm256_andnot_si256(lanespeed.vec, eight));
  printf("%d, %d, %d, %d, %d, %d, %d, %d \n", iters.lanes[0], iters.lanes[1], iters.lanes[2], iters.lanes[3], iters.lanes[4], iters.lanes[5], iters.lanes[6], iters.lanes[7]);
  iters.vec += _mm256_or_si256(_mm256_and_si256(one, lanespeed.vec), _mm256_andnot_si256(lanespeed.vec, eight));
  printf("%d, %d, %d, %d, %d, %d, %d, %d \n", iters.lanes[0], iters.lanes[1], iters.lanes[2], iters.lanes[3], iters.lanes[4], iters.lanes[5], iters.lanes[6], iters.lanes[7]);
  iters.vec += _mm256_or_si256(_mm256_and_si256(one, lanespeed.vec), _mm256_andnot_si256(lanespeed.vec, eight));
  printf("%d, %d, %d, %d, %d, %d, %d, %d \n", iters.lanes[0], iters.lanes[1], iters.lanes[2], iters.lanes[3], iters.lanes[4], iters.lanes[5], iters.lanes[6], iters.lanes[7]);

}
