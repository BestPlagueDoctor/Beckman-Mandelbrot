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
#include <GL/glew.h>
#define GLEW_STATIC
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cuda_gl_interop.h>

__global__ void calc(int *img, int const maxiter, float const recdiv, float const imcdiv, float const cx0, float const cy0, int const xres);

void pgm(int maxiter, int* img, int xres, int yres);

void mandelbrot() {
  int xres = 1920, yres = 1080;
  int width = 1920, height = 1080, maxiter = 4096;
  //GLEW STUFF
  sf::Window window(sf::VideoMode(1,1), "OpenGL");
  glewExperimental = GL_TRUE;
  glewInit();
  glViewport(0,0,xres,yres);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,1.0f,0,1.0f,-1.0f,1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_DEPTH_TEST);
  glClearColor(1.0f,1.0f,1.0f,1.5f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  cudaGLSetGLDevice(1);
  GLuint mainbuffer;
  glGenBuffers(1, &mainbuffer);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mainbuffer);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, xres * yres * sizeof(int), NULL, GL_DYNAMIC_COPY);
  cudaGLRegisterBufferObject(mainbuffer);
  glEnable(GL_TEXTURE_2D);
  unsigned int textureID;
  glGenTextures(1,&textureID);
  glBindTexture(GL_TEXTURE_2D,textureID);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,xres,yres,0,GL_BGRA,GL_UNSIGNED_BYTE,NULL);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  //END GLEW STUFF
  int* img;
  cudaGraphicsGLRegisterBuffer(img, mainbuffer);
  int* destimg = (int*)malloc(height * width * (sizeof(int)));
  size_t pitch;
  pitch = width * height * sizeof(int);
  cudaMalloc(&img, width * sizeof(int) * height);
  //Coordinate update for more interesting images
  float const cx0 = -2, cx1 = 1, cy0 = -1.2, cy1 = 1.2;
  float const cw = cx1 - cx0, ch = cy1 - cy0;
  float const recdiv = cw / float(xres), imcdiv = ch / float(yres);
  dim3 const dimBlock(32, 8);
  dim3 const dimGrid(std::ceil(float(xres) / dimBlock.x), std::ceil(float(yres) / dimBlock.y));
  auto const start = std::chrono::high_resolution_clock::now();
  calc<<<dimGrid, dimBlock>>>(img, maxiter, recdiv, imcdiv, cx0, cy0, xres);
  cudaDeviceSynchronize();
  auto const end = std::chrono::high_resolution_clock::now();
  printf("Com time: %lums\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
  cudaMemcpy(destimg,img,pitch,cudaMemcpyDeviceToHost);
  pgm(maxiter, destimg, xres, yres);
  cudaFree(img);
}

__global__ void calc(int *img, int const maxiter, float const recdiv, float const imcdiv, float const cx0, float const cy0, int const xres) {
  int iters = 0;
  float ztemp = 0, zreal = 0, zimag = 0;
  float creal = (blockIdx.x * blockDim.x + threadIdx.x) * recdiv + cx0;
  float cimag = (blockIdx.y * blockDim.y + threadIdx.y) * imcdiv + cy0;
  while (iters <= maxiter && ((zimag*zimag)+(zreal*zreal)) <= 4.0) {
    //Update math algo, branch detection check
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
