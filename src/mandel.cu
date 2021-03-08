// This is mostly a test
// take fasdf asdfas lmao
#define GL_GLEXT_PROTOTYPES
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
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

int xres = 1920, yres = 1080;
GLuint pbo = 0; // pixelbuffer obj
GLuint tex = 0; // texture obj
struct cudaGraphicsResource* cuda_pbo_resource;
struct uchar4;

__global__ void calc(
    uchar4* dout, int maxiter, float recdiv, float imcdiv, float cx0, float cy0, int xres);
void pgm(int maxiter, int* img, int xres, int yres);
void startkernel(uchar4* dout);

void render() {
  uchar4* dout = 0;
  cudaGraphicsMapResources(1, &cuda_pbo_resource, 0);
  cudaGraphicsResourceGetMappedPointer((void**)&dout, NULL, cuda_pbo_resource);
  startkernel(dout);
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

void exitfunc() {
  if (pbo) {
    cudaGraphicsUnregisterResource(cuda_pbo_resource);
    glDeleteBuffers(1, &pbo);
    glDeleteTextures(1, &tex);
  }
}

void startkernel(uchar4* dout) {
  int xres = 1920, yres = 1080;
  int maxiter = 4096;
  float const cx0 = -2, cx1 = 1, cy0 = -1.2, cy1 = 1.2;
  float const cw = cx1 - cx0, ch = cy1 - cy0;
  float const recdiv = cw / float(xres), imcdiv = ch / float(yres);
  dim3 const dimBlock(32, 8);
  dim3 const dimGrid(std::ceil(float(xres) / dimBlock.x), std::ceil(float(yres) / dimBlock.y));
  calc<<<dimGrid, dimBlock>>>(dout, maxiter, recdiv, imcdiv, cx0, cy0, xres);
}

__global__ void calc(
    uchar4* dout, int maxiter, float recdiv, float imcdiv, float cx0, float cy0, int xres) {
  int iters = 0;
  float ztemp = 0, zreal = 0, zimag = 0;
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
  unsigned int ind = y * xres + x;
  float creal = x * recdiv + cx0;
  float cimag = y * imcdiv + cy0;
  while (iters <= maxiter && ((zimag * zimag) + (zreal * zreal)) <= 4.0) {
    // Update math algo, branch detection check
    ztemp = zreal;
    zreal = ((zreal * zreal) - (zimag * zimag));
    zimag = (ztemp * zimag);
    zimag += zimag;
    zreal += creal;
    zimag += cimag;
    iters += 1;
  }
  dout[ind].x = iters;
  dout[ind].y = iters;
  dout[ind].z = iters;
  dout[ind].w = iters;
}

void mandelbrot(int argc, char** argv) {
  int xres = 1920, yres = 1080;
  // GLUT STUFF
  initglut(&argc, argv);
  gluOrtho2D(0, xres, yres, 0);
  glutDisplayFunc(display);
  pboinit();
  glutMainLoop();
  atexit(exitfunc);
}

int main(int argc, char** argv) { mandelbrot(argc, argv); }
