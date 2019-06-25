#include "proto.h"

void rescale_colors_pgm(int *img, int res, int *maxiter) {
  int maxval = (*maxiter) + 1;
  for (int i = 0; i < res; i++) {
    img[i] = maxval-img[i];
  }
}
