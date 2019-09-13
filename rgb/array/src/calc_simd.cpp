#include "proto.h"
#include <cmath>
#include <immintrin.h>


void imgmandel_simd(int maxiter, int *img, int xres, int yres) {
  float recdiv = 3.0/(xres);
  float imcdiv = 2.4/yres;
  __m256 truevec[8] = {1, 1, 1, 1, 1, 1, 1, 1};
  __m256 fourcomp = _mm256_set1_ps(4.0);
  float incrx[8] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
  __m256 incrxvec;
  incrxvec = _mm256_loadu_ps(incrx);
  incrxvec = _mm256_mul_ps(incrxvec, _mm256_set1_ps(imcdiv));
  float cr, ci;
  float zr, zi;
  float saver, savei;
  int y;
  for (y = 0; y < yres; y++) {
    int x;
    for (x = 0; x < xres; x+=1) {
      int iters=0;
      float ci = ((y*imcdiv)-1.2);
      float cr = (((x*recdiv)-2.0));
      __m256 creal  = _mm256_set1_ps(cr);
      __m256 cimag  = _mm256_set1_ps(ci);
      //float zreal = zr, zimag = zi;
      __m256 zreal = _mm256_set1_ps(zr);
      zreal = _mm256_add_ps(zreal, incrxvec);
      __m256 zimag = _mm256_set1_ps(zi);
      __m256 ztemp = _mm256_setzero_ps();
      __m256 savereal = zreal, saveimag = zimag;
      __m256 comp = _mm256_setzero_ps();
      
      comp = (_mm256_cmp_ps(_mm256_add_ps(_mm256_mul_ps(zreal, zreal), _mm256_mul_ps(zimag, zimag)), fourcomp, 2));

#if 1
       while ((iters < maxiter) && comp[0] == 1) {
        //squaring z
        ztemp = zreal;
        //zreal = ((zreal * zreal) - (zimag * zimag));
        zreal = _mm256_sub_ps(_mm256_mul_ps(zreal, zreal), _mm256_mul_ps(zimag, zimag));
        //zimag = (ztemp * zimag);
        zimag = _mm256_mul_ps(ztemp, zimag);
        //zimag += zimag;
        zimag = _mm256_add_ps(zimag, zimag);

        //adding c
        //zreal += creal;
        zreal = _mm256_add_ps(zreal, creal);
        //zimag += cimag;
        zimag = _mm256_add_ps(zimag, cimag);
        iters += 1;
        comp = (_mm256_cmp_ps(_mm256_add_ps(_mm256_mul_ps(zreal, zreal), _mm256_mul_ps(zimag, zimag)), fourcomp, 2));
      }
#endif
#if 0
       while((iters < maxiter) && (zreal*zreal+zimag*zimag) <= 4.0) {
         ztemp = _mm256_set1_ps(zr);
         zreal = _mm256_
#endif

#if 0
      if ((iters >=maxiter) || (zreal*zreal+zimag*zimag >= 4.0)) {
        iters -= 1;
        zreal = savereal;
        zimag = saveimag;
#endif
#if 0
        float zrsq = zreal*zreal;
        float zisq = zimag*zimag;
        while ((iters < maxiter) && (zrsq+zisq <= 4.0)) {
          ztemp = zreal;
          zreal = (zrsq) - (zisq);
          zimag = (ztemp * zimag);
          zimag += zimag;
          //adding c
          zreal += creal;
          zimag += cimag;
          zrsq = zreal*zreal;
          zisq = zimag*zimag;
          iters++;
        }
#endif
#if 1
      //}
#endif

      img[((y*xres)+x)] = iters;
      //printf("%d, ", iters);
      //  printf("%d, %d)  ", ((y*xres)+x), iters);
    //}
  //}
  }
}
}

void pgm(int maxiter, int *img, int xres, int yres) {
  const char filename[1024] = "haha.pgm";
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
  printf("size of img is %d \n", sizeof(img)/sizeof(int));
}

int main() {
  int *img = (int*) malloc(1024*768*sizeof(int));
  imgmandel_simd(4096, img, 1024, 768);
  pgm(4096, img, 1024, 768);
  free(img);
}