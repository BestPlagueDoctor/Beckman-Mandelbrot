#include "proto.h"
#include <cmath>

void imgmandel_simd(int maxiter, int *img, int xres, int yres) {
  float incrx[8] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
  _m256 incrxvec;
  incrxvec = _mm256_loadu_ps(incrx); 
  incrxvec = _mm256_mul_ps(incrx, _mm256_set1_ps(imcdiv));
  float recdiv = 3.0/(xres);
  float imcdiv = 2.4/yres;
  float cr, ci;
  float zr, zi;
  float saver, savei;
  int y;
  for (y = 0; y < yres; y++) {
    int x;
    for (x = 0; x < xres; x+=8) {
      float cimag = ((x*imcdiv)-1.2);
      float creal = (((y*recdiv)-2.0));
      cr = creal, ci = cimag;
      int iters=0;
      float zreal = zr, zimag = zi;
      __m256 zreal = _mm256_set1_ps(zr);
      zreal = _mm256_add_ps(zreal, incrxvec);
      __m256 zimag = _mm256_set1_ps(zi);
      __m256 ztemp = _mm256_setzero_ps();
      float savereal=zreal, saveimag=zimag;

#if 1      
       while ((iters < maxiter) && (zreal*zreal+zimag*zimag <= 4.0)) {
        //squaring z
        ztemp = zreal;
        zreal = ((zreal * zreal) - (zimag * zimag));
        zimag = (ztemp * zimag);
        zimag += zimag;
        //adding c
        zreal += creal;
        zimag += cimag;
        iters += 1;
      }
#endif


#if 1  
      if ((iters >=maxiter) || (zreal*zreal+zimag*zimag >= 4.0)) {
        iters -= 1;
        zreal = savereal;
        zimag = saveimag; 
#endif
#if 1
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
      }
#endif

      img[((y*xres)+x)] = iters;
      //printf("%d, ", iters);
      //  printf("%d, %d)  ", ((y*xres)+x), iters);
    }
  }
}

