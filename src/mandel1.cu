//This is mostly a test
#include <iostream>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <immintrin.h>
#include <memory>
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

__global__ void calc(int *img, int const maxiter, float const recdiv, float const imcdiv, float const cx0, float const cy0, int const xres);

void pgm(int maxiter, int* img, int xres, int yres);

void mandelbrot() {
  int* img;
  int xres = 3840, yres = 2160;
  int width = 3840, height = 2160, maxiter = 1000000;
  int* destimg = (int*)malloc(height * width * (sizeof(int)));
  size_t pitch;
  pitch = width * height * sizeof(int);
  cudaMalloc(&img, width * sizeof(int) * height);
  float const cx0 = -2, cx1 = 1, cy0 = -1.2, cy1 = 1.2;
  float const cw = cx1 - cx0, ch = cy1 - cy0;
  float const recdiv = cw / float(xres), imcdiv = ch / float(yres);
  dim3 const dimBlock(16, 16);
  dim3 const dimGrid(std::ceil(float(xres) / dimBlock.x), std::ceil(float(yres) / dimBlock.y));
  auto const start = std::chrono::high_resolution_clock::now();
  calc<<<dimGrid, dimBlock>>>(img, maxiter, recdiv, imcdiv, cx0, cy0, xres);
  cudaDeviceSynchronize();
  auto const end = std::chrono::high_resolution_clock::now();
  printf("Com time: %lums\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
  cudaMemcpy(destimg,img,pitch,cudaMemcpyDeviceToHost);
  pgm(1000000, destimg, 3840, 2160);
  cudaFree(img);
}

__global__ void calc(int *img, int const maxiter, float const recdiv, float const imcdiv, float const cx0, float const cy0, int const xres) {
  int iters = 0;
  float ztemp = 0, zreal = 0, zimag = 0;
  float creal = (blockIdx.x * blockDim.x + threadIdx.x) * recdiv + cx0;
  float cimag = (blockIdx.y * blockDim.y + threadIdx.y) * imcdiv + cy0;
  while (iters <= maxiter && ((zimag*zimag)+(zreal*zreal)) <= 4.0) {
    ztemp = zreal;
    zreal = ((zreal * zreal) - (zimag * zimag));
    zimag = (ztemp * zimag);
    zimag += zimag;
    zreal += creal;
    zimag += cimag;
    iters += 1;
  }
  img[(blockIdx.x * blockDim.x + threadIdx.x)+(blockIdx.y * blockDim.y + threadIdx.y)*xres] = iters;
}

//Needs editing to use cudaMemCopy
void pgm(int const maxiter, int* img, int const xres, int const yres) {
  const char filename[1024] = "smol.pgm";
  FILE* ofp;
  if ((ofp = fopen(filename, "w")) == NULL) {
        perror("FAILURE");
            return;

  }
  fprintf(ofp, "P2\n");
  fprintf(ofp, "%d %d \n", xres, yres);
  fprintf(ofp, "%d \n", maxiter);
  for (int i = 0; i < xres * yres; i++) {
    if (i % xres == 0) {
      fprintf(ofp, "\n");
    }
    fprintf(ofp, "%d ", img[i]);
  }
  free(img);
}


int main() {
  mandelbrot();
}
