// This is mostly a test
#include <GL/glew.h>
#include <GL/glut.h>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cuda_gl_interop.h>
#include <cuda_runtime.h>
#include <immintrin.h>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <thread>

int times = 0;
int xres = 1920, yres = 1080;
GLuint pbo = 0; // pixelbuffer obj
GLuint tex = 0; // texture obj
GLuint fbo = 0; // framebuffer obj
GLuint rbo = 0; // framebuffer obj
struct cudaGraphicsResource* cuda_pbo_resource;
struct uchar4;

__global__ void calc(uchar4* rgba, int maxiter, float recdiv, float imcdiv, float cx0, float cy0,
    int xres, float* hist, int* iterimg, int& total);
__global__ void color(uchar4* rgba, float* hist, int xres, int* iterimg, int& total);
void pgm(int maxiter, int* img, int xres, int& yres);
void startkernel(uchar4* rgba);
void pboinit();

void display() {
  // create uchar and do math
  uchar4* rgba = nullptr;
  cudaGraphicsMapResources(1, &cuda_pbo_resource, 0);
  cudaGraphicsResourceGetMappedPointer((void**)&rgba, NULL, cuda_pbo_resource);
  startkernel(rgba);
  cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);

  glBindTexture(GL_TEXTURE_2D, tex);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, xres * yres * 4 * sizeof(GLubyte), 0, GL_STREAM_DRAW);

  glTexSubImage2D(
      GL_TEXTURE_2D, 0, 0, 0, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, 0); // Null or pbo? hm
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(0, 0);
  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(0, yres);
  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(xres, yres);
  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(xres, 0);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  glutSwapBuffers();
}

void initglut(int* argc, char** argv) {
  glutInit(argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(xres, yres);
  glutCreateWindow("mandelbrot");
  glewInit();
  pboinit();
}

void pboinit() {
  glGenBuffers(1, &pbo);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, xres * yres * 4 * sizeof(GLubyte), 0, GL_STREAM_DRAW);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard);

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void exitfunc() {
  if (pbo) {
    cudaGraphicsUnregisterResource(cuda_pbo_resource);
    glDeleteBuffers(1, &pbo);
    glDeleteTextures(1, &tex);
  }
}

void startkernel(uchar4* rgba) {
  int maxiter = 4096;
  float const cx0 = -2, cx1 = 1, cy0 = -1.2, cy1 = 1.2;
  float const cw = cx1 - cx0, ch = cy1 - cy0;
  float const recdiv = cw / float(xres), imcdiv = ch / float(yres);
  dim3 const dimBlock(32, 8);
  dim3 const dimGrid(std::ceil(float(xres) / dimBlock.x), std::ceil(float(yres) / dimBlock.y));

  float* hist;
  int* img;
  int* total;
  cudaMalloc(&hist, xres * yres * sizeof(float));
  cudaMalloc(&img, xres * yres * sizeof(int));
  cudaMalloc(&total, sizeof(int));

  calc<<<dimGrid, dimBlock>>>(rgba, maxiter, recdiv, imcdiv, cx0, cy0, xres, hist, img, *total);
  color<<<dimGrid, dimBlock>>>(rgba, hist, xres, img, *total);

  cudaFree(hist);
  cudaFree(img);
  cudaFree(total);
}

__global__ void calc(uchar4* rgba, int maxiter, float recdiv, float imcdiv, float cx0, float cy0,
    int xres, float* hist, int* img, int& total) {
  int iters = 0;
  float zmag2 = 0, zrsq = 0, zisq = 0, zreal = 0, zimag = 0;
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
  unsigned int ind = y * xres + x;
  float creal = x * recdiv + cx0;
  float cimag = y * imcdiv + cy0;

  while (iters <= maxiter && zmag2 <= 4) {
    zimag = ((zreal + zreal) * zimag) + cimag;
    zreal = (zrsq - zisq) + creal;
    zisq = zimag * zimag;
    zrsq = zreal * zreal;
    zmag2 = zrsq + zisq;
    iters++;
  }

  hist[iters]++;
  img[ind] = iters;
}

__global__ void color(uchar4* rgba, float* hist, int xres, int* img, int& total) {
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
  unsigned int ind = y * xres + x;
  float hue;
  total = 0;

  // TODO figure out why total changes
  float res = 96000;
  for (int i = 0; i <= img[ind]; i++) {
    hue += hist[i] / res;
  }

  rgba[ind].y = hue * 255;
  rgba[ind].x = 0;   // R
  rgba[ind].z = 0;   // B
  rgba[ind].w = 255; // A
}

void mandelbrot(int argc, char** argv) {
  // GLUT STUFF
  initglut(&argc, argv);
  gluOrtho2D(0, xres, yres, 0);
  glutDisplayFunc(display);
  glutMainLoop();
  atexit(exitfunc);
}

int main(int argc, char** argv) { mandelbrot(argc, argv); }
