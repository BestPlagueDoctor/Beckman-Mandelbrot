// Hard scrap, new plan is to fully vectorize
#include "proto.h"
#include <chrono>
#include <cmath>
#include <cstdint>
#include <immintrin.h>
#include <memory>
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#define LANE_SIZE 8
#define COUNTERINIT 0
#define LANE_EMPTY -1
#define LANE_FINISHED -2
#define X_SCALE 2.0
#define Y_SCALE 1.2
#define X_AXIS 3.0
#define Y_AXIS 2.4

// Union set is a vector of 32bit floats
union set {
  __m256 vec;
  float lanes[LANE_SIZE];
};

// Union intset is a vector is signed 32bit ints, for when numbers had better be
// whole
union intset {
  __m256i vec;
  int32_t lanes[LANE_SIZE];
};

struct mainobj {

  // Maxiter is a representation of precision used in calculation
  uint32_t maxiter;

  // imcdiv/recdiv are the ratio of range/resolution, giving positional location
  uint32_t xres;
  uint32_t yres;
  uint32_t numpixels;
  float imcdiv;
  float recdiv;

  // Cartesian presets, the edges of the graph
  float cx0;
  float cy0;
  float cx1;
  float cy1;
  float cw;
  float ch;
};

void calc(mainobj mainset, int* img, __m256i one, __m256 four, __m256i maxitervec,
    std::atomic<int>& work, int threadnumber, int tilesize);

void init(int maxiter, int* img, int xres, int yres, int tilesize, int cores) {
  mainobj mainset;
  mainset.maxiter = maxiter;
  mainset.xres = xres;
  mainset.yres = yres;
  mainset.numpixels = xres * yres;

  // initialize cartesian coordinate window
  mainset.cx0 = -2;
  mainset.cy0 = -1.2;
  mainset.cx1 = 1;
  mainset.cy1 = 1.2;
  mainset.cw = mainset.cx1 - mainset.cx0;
  mainset.ch = mainset.cy1 - mainset.cy0;

  mainset.recdiv = mainset.cw / mainset.xres;
  mainset.imcdiv = mainset.ch / mainset.yres;

  // initialize comparison vectors
  __m256i one = _mm256_set1_epi32(1);
  __m256 four = _mm256_set1_ps(4.0);
  __m256i maxitervec = _mm256_set1_epi32(mainset.maxiter);

  // Threadcode goes here //
  auto threads = std::vector<std::jthread>{};
  // auto const threadcount = cores;
  auto const threadcount = std::thread::hardware_concurrency();
  // auto const div = mainset.numpixels / threadcount;
  auto work = std::atomic<int>{};
  for (unsigned int i = 0; i < threadcount; ++i) {
    threads.emplace_back(calc, mainset, img, one, four, maxitervec, std::ref(work), i, tilesize);
  }
}

// Cleanup//
void calc(mainobj mainset, int* img, __m256i one, __m256 four, __m256i maxitervec,
    std::atomic<int>& work, int threadnumber, int tilesize) {
  // Definitions per thread //
  intset iters, isfinished, px, py;
  set creal, cimag, zreal, zimag, ztemp, zmag2;
  uint32_t x, y, pxind;
  uint32_t start, end;
  const float incrlist[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  const __m256 incrvec = _mm256_load_ps(incrlist);
  const __m256 recdivvec = _mm256_set1_ps(mainset.recdiv);
  const __m256 cx0vec = _mm256_set1_ps(mainset.cx0);
  // Tilesize must be a multiple of 8, or fear the wrath of the segfault
  // auto const tilesize = 256;
  // End per thread definitions //

  while ((start = work.fetch_add(tilesize, std::memory_order_relaxed)) < mainset.numpixels) {
    end = start + tilesize;
    iters.vec = _mm256_set1_epi32(0);
    isfinished.vec = _mm256_set1_epi32(0);
    zreal.vec = _mm256_set1_ps(0.0F);
    zimag.vec = _mm256_set1_ps(0.0F);
    creal.vec = _mm256_set1_ps(0.0F);
    cimag.vec = _mm256_set1_ps(0.0F);
    pxind = start;

    while (pxind < end) {
      x = pxind % mainset.xres;
      y = pxind / mainset.xres;

      // New code to set c
      creal.vec = _mm256_add_ps(
          _mm256_mul_ps(_mm256_add_ps(_mm256_set1_ps(float(x)), incrvec), recdivvec), cx0vec);
      cimag.vec = _mm256_set1_ps(float(y) * mainset.imcdiv + mainset.cy0);
      //
      ztemp.vec = zreal.vec; // zreal = ((zreal * zreal) - (zimag * zimag));
      zreal.vec = _mm256_sub_ps(
          _mm256_mul_ps(zreal.vec, zreal.vec), _mm256_mul_ps(zimag.vec,
                                                   zimag.vec)); // zimag = (ztemp * zimag);
      zimag.vec = _mm256_mul_ps(ztemp.vec, zimag.vec);          // zimag += zimag;
      zimag.vec = _mm256_add_ps(zimag.vec, zimag.vec);          // adding c
      zreal.vec = _mm256_add_ps(zreal.vec, creal.vec);          // zreal += creal;
      zimag.vec = _mm256_add_ps(zimag.vec, cimag.vec);          // zimag += cimag;
      //
      zmag2.vec =
          _mm256_add_ps(_mm256_mul_ps(zreal.vec, zreal.vec), _mm256_mul_ps(zimag.vec, zimag.vec));
      isfinished.vec =
          _mm256_or_si256(_mm256_castps_si256(_mm256_cmp_ps(zmag2.vec, four, _CMP_NLE_UQ)),
              (_mm256_cmpgt_epi32(iters.vec, maxitervec)));
      iters.vec = _mm256_add_epi32(_mm256_andnot_si256(isfinished.vec, one), iters.vec);
      if (_mm256_movemask_epi8(isfinished.vec) == -1) {
        _mm256_stream_si256((__m256i*)(&img[pxind]), iters.vec);
        pxind += 8;
        isfinished.vec = _mm256_set1_epi32(0);
        iters.vec = _mm256_set1_epi32(0);
        zreal.vec = _mm256_set1_ps(0.0F);
        zimag.vec = _mm256_set1_ps(0.0F);
      }
    }
  }
}

void pgm(int maxiter, int* img, int xres, int yres) {
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
}

int main(int argc, char** argv) {
  int tilesize = 8;
  int cores;
  for (int arg = 1; arg < argc; arg++) {
    if (!strcmp(argv[arg], "-h") || !strcmp(argv[arg], "--help")) {
      printf("Usage: \n");
      printf("-tilesize N");
    }
    if (!strcmp(argv[arg], "-t")) {
      arg++;
      tilesize = atoi(argv[arg]);
    }
    if (!strcmp(argv[arg], "-c")) {
      arg++;
      cores = atoi(argv[arg]);
    }
  }
  auto img = std::unique_ptr<int[]>{new (std::align_val_t(64)) int[1920 * 1080]};
  init(4096, img.get(), 1920, 1080, tilesize, cores);
  pgm(4096, img.get(), 1920, 1080);
}

// Spare code snippets
#if 0
int main() {
  int* img = (int*)malloc(1920 * 1080 * sizeof(int));
  init(4096, img, 1920, 1080);
  pgm(4096, img, 1920, 1080);
  free(img);
}
#endif

#if 0
  pxind.vec = _mm256_and_si256(mainset.isfinished.vec, _mm256_add_epi32(_mm256_mul_epi32(xresvec, mainset.py.vec), mainset.px.vec));
  if (_mm256_testz_si256(mainset.isfinished.vec, zero) != 0) {
    //py*xres + px
    pxind.vec = _mm256_and_si256(mainset.isfinished.vec, _mm256_add_epi32(_mm256_mul_epi32(xresvec, mainset.py.vec), mainset.px.vec));
    for (int i = 0; i < 8; i++) {
      if (pxind.lanes[i] != 0) {
        img[pxind.lanes[i]] = mainset.iters.lanes[i];
        mainset.iters.lanes[i] = LANE_EMPTY;
      }
      if (pxind.lanes[i] >= mainset.numpixels - 8) {
        sentinel++;
      }
    }
    //0 out the dead lanes for iters (using bitwise logic?)
    //if pxind > numpixels - 8 {kill lanes, increment sentinel}
  }
#endif
