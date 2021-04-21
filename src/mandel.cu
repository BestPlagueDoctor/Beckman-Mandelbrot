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
__global__ void color(
    uchar4* rgba, float* hist, int xres, int* iterimg, float* huetest, int& total);
void pgm(int maxiter, int* img, int xres, int& yres);
void startkernel(uchar4* rgba);
void fboinit();
void pboinit();

void render() {
  uchar4* rgba = nullptr;
  cudaGraphicsMapResources(1, &cuda_pbo_resource, 0);
  cudaGraphicsResourceGetMappedPointer((void**)&rgba, NULL, cuda_pbo_resource);
  startkernel(rgba);
  cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);
}

void drawTex() {
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
}

void display() {
  render();
  drawTex();
  glutSwapBuffers();
}

void initglut(int* argc, char** argv) {
  glutInit(argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(xres, yres);
  glutCreateWindow("mandelbrot");
  glewInit();
  pboinit();
  //  fboinit();
}

void pboinit() {
  glGenBuffers(1, &pbo);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, xres * yres * 4 * sizeof(GLubyte), 0, GL_STREAM_DRAW);
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard);
}

void fboinit() {
  glGenFramebuffers(1, &fbo);
  glGenTextures(1, &tex);
  glGenRenderbuffers(1, &rbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, xres, yres);
  glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
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

  float* hist;
  int* img;
  cudaMalloc(&img, xres * yres * sizeof(int));
  cudaMalloc(&hist, xres * yres * sizeof(float));

  float* huetest;
  cudaMalloc(&huetest, xres * yres * sizeof(float));

  int* total;
  cudaMalloc(&total, sizeof(int));

  dim3 const dimBlock(32, 8);
  dim3 const dimGrid(std::ceil(float(xres) / dimBlock.x), std::ceil(float(yres) / dimBlock.y));
  calc<<<dimGrid, dimBlock>>>(rgba, maxiter, recdiv, imcdiv, cx0, cy0, xres, hist, img, *total);
  color<<<dimGrid, dimBlock>>>(rgba, hist, xres, img, huetest, *total);
  cudaFree(img);
  cudaFree(hist);
  cudaFree(total);
  cudaFree(huetest);
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
  // rgba[ind].x = 0; // R
  // rgba[ind].y = float(iters) * (255.0f / 4096.0f);
  // rgba[ind].z = 0;   // B
  // rgba[ind].w = 255; // A
}

__global__ void color(uchar4* rgba, float* hist, int xres, int* img, float* huetest, int& total) {
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
  unsigned int ind = y * xres + x;
  float hue;
  total = 0;

#if 1
  for (int i = 0; i < 4096; i++) {
    total += hist[i];
  }
#endif

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
