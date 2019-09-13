#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string.h>



//
// TEA, a tiny encryption algorithm.
// D. Wheeler and R. Needham, 2nd Intl. Workshop Fast Software Encryption,
// LNCS, pp. 363-366, 1994.
//
// GPU Random Numbers via the Tiny Encryption Algorithm
// F. Zafar, M. Olano, and A. Curtis.
// HPG '10 Proceedings of the Conference on High Performance Graphics,
// pp. 133-141, 2010.
//
template<unsigned int N> static __inline__
unsigned int tea(unsigned int val0, unsigned int val1) {
  unsigned int v0 = val0;
  unsigned int v1 = val1;
  unsigned int s0 = 0;

  for( unsigned int n = 0; n < N; n++ ) {
    s0 += 0x9e3779b9;
    v0 += ((v1<<4)+0xa341316c)^(v1+s0)^((v1>>5)+0xc8013ea4);
    v1 += ((v0<<4)+0xad90777d)^(v0+s0)^((v0>>5)+0x7e95761e);
  }

  return v0;
}


 
class complexnumber {
  float real, imag;

  public:
    void printcomp();  
    void setcomp (float, float);
    void getcomp ();
    void addcomp(complexnumber, complexnumber);
    void multcomp(complexnumber, complexnumber);
    float getreal();
    float getimag();  
    float sqmagnit();
}; 


void complexnumber::printcomp () {
  std::cout << "Real: " << real << "\n";
  std::cout << "Imag: " << imag << "\n";
}  

void complexnumber::setcomp (float x, float y) {
  real = x;
  imag = y;
}

void complexnumber::addcomp(complexnumber x, complexnumber y) {
  real = x.getreal() + y.getreal();  
  imag = x.getimag() + y.getimag();  
}

void complexnumber::multcomp(complexnumber x, complexnumber y) {
  real = ((x.getreal() * y.getreal()) - (x.getimag() * y.getimag()));
  imag = ((x.getreal() * y.getimag()) + (x.getimag() * y.getreal()));
}  

float complexnumber::getreal () {
  return real;
}

float complexnumber::getimag() { return imag;
}

float complexnumber::sqmagnit() {
  return ((real * real) + (imag * imag));
}

int pointcheck(int maxiter, float rec, float imc) {
  complexnumber c;
  complexnumber z;
  complexnumber zsq;  
  z.setcomp(0.0, 0.0);   // z   = (0 + 0i)
  zsq.setcomp(0.0, 0.0); // zsq = (0 + 0i)
  c.setcomp(rec, imc);   // c   = (r + i)
  int i;
  for (i = 0; i < maxiter; i++) {
    zsq.multcomp(z, z);        //  zsq = z * z
    z.addcomp(zsq, c);         //  z = zsq + c
    if (z.sqmagnit() >= 4) {   // if |z| >= 2.0 ...
      break;
    }
    //z.printcomp();
  }

  return i;
}


void mandlebrot_text(float xmin, float ymin, float xmax, float ymax, int xres, int yres, int maxiter) {
  float xinc = (xmax - xmin) / float(xres);  
  float yinc = (ymax - ymin) / float(yres);
  for (float imc = ymin; imc <= ymax; imc+=yinc) {
    for (float rec = xmin; rec <= xmax; rec+=xinc) {
      int iter=pointcheck(maxiter, rec, imc); 

      if (iter >= maxiter) {
        printf("*");
      }

      if (iter < maxiter) {
        printf(" ");
      }
    }
    printf("\n");
  }
}


void mandlebrot_text_count(float xmin, float ymin, float xmax, float ymax, int xres, int yres, int maxiter) {
  float xinc = (xmax - xmin) / float(xres);  
  float yinc = (ymax - ymin) / float(yres);
  for (float imc = ymin; imc < ymax; imc+=yinc) {
    for (float rec = xmin; rec < xmax; rec+=xinc) {
      int iter=pointcheck(maxiter, rec, imc); 
      printf("%5d ", iter);
    }
    printf("\n");
  }
}

void mandlebrot_pgm_ascii(float xmin, float ymin, float xmax, float ymax, int xres, int yres, int maxiter, 
                          const char *filename) {
  printf("Writing Mandelbrot set to filename '%s'....\n", filename);
  FILE *ofp;
  if ((ofp = fopen(filename, "w")) == NULL) {
    perror("Failed to open output file!\n");
    return;
  }

  // file header pgm
  fprintf(ofp, "P2\n");
  fprintf(ofp, "%d %d \n", xres, yres);

  int maxval = maxiter + 1;

#if defined(LOGSCALE)
  maxval = (int) logf(maxiter)+1;
#elif defined(LOGLOGSCALE)
  maxval = (int) logf(logf(maxiter))+1;
#endif

  fprintf(ofp, "%d \n", maxval);

  float xinc = (xmax - xmin) / float(xres);  
  float yinc = (ymax - ymin) / float(yres);
  float imc;
  int y;
  for (y=0, imc=ymin; y<yres; y++,imc+=yinc) {
    int x;
    float rec;
    for (x=0, rec=xmin; x<xres; x++,rec+=xinc) {
      int iter=pointcheck(maxiter, rec, imc); 

#if defined(USELAMECODE)
#error Dont use lame code man!
#elif defined(LOGSCALE)
      fprintf(ofp, "%d ", maxval - ((int) logf(iter)));
#elif defined(LOGLOGSCALE)
      fprintf(ofp, "%d ", maxval - ((int) logf(logf(iter))));
#elif 0
      fprintf(ofp, "%d ", iter);
#elif 1
      fprintf(ofp, "%d ", maxval-iter);
#endif
    }

    fprintf(ofp, "\n");
    printf("Calculating line: %d             \r", y);
    fflush(stdout);
  }

  fclose(ofp);
}


int main(int argc, char **argv) {
  int maxiter = 4096, xres = 1024 , yres = 768;
  float xmin = -2.0, xmax = 1.0, ymin = -1.2, ymax = 1.2;
  enum outputmode { TEXT, TEXTCOUNT, PGMFILE };
  char filename[1024];

  outputmode mode = PGMFILE;
  strcpy(filename, "test.pgm");

  for (int arg=1; arg < argc; arg++) {
    if (!strcmp(argv[arg], "-h") ||
        !strcmp(argv[arg], "--help")) {
      printf("Usage:\n");
      printf("%s [-text | -textcounts | -pgm filename.pgm] [-maxiter N] \\ \n", argv[0]);
      printf("   [-xres XXX] [-yres YYY]\n");
    }

    if (!strcmp(argv[arg], "-xres")) {
      arg++;
      xres = atoi(argv[arg]);
    }

    if (!strcmp(argv[arg], "-yres")) {
      arg++;
      yres = atoi(argv[arg]);
    }


    if (!strcmp(argv[arg], "-text")) {
      printf("will output text.\n");
      mode = TEXT;
    }

    if (!strcmp(argv[arg], "-textcounts")) {
      printf("will output text counts.\n");
      mode = TEXTCOUNT;
    }

    if (!strcmp(argv[arg], "-pgm")) {
      arg++;
      strcpy(filename, argv[arg]);
      printf("Will output to PGM file: '%s'\n", filename);
      mode = PGMFILE;
    }

    if (!strcmp(argv[arg], "-maxiter")) {
      arg++;
      maxiter = atoi(argv[arg]);

      printf("read maxiter parameter '%s': %d\n", argv[arg], maxiter);
    }
  }

  printf("'%s': Running Mandelbrot set with params:\n", argv[0]);
  printf(" xres: %d  yres: %d  maxiter %d\n", xres, yres, maxiter);
  printf(" xmin: %.2f  xmax: %.2f  ymin: %.2f  ymax: %.2f\n", xmin, xmax, ymin, ymax);
  printf("-----------------------------------------------------------------------------------------\n");

  switch (mode) {
    case TEXT:
      mandlebrot_text(xmin, ymin, xmax, ymax, xres, yres, maxiter);
      break;

    case TEXTCOUNT:
      mandlebrot_text_count(xmin, ymin, xmax, ymax, xres, yres, maxiter);
      break;

    case PGMFILE:
      {
      mandlebrot_pgm_ascii(xmin, ymin, xmax, ymax, xres, yres, maxiter, filename);
      }
      break;

    default:
      printf("Bad calculation mode! Bailing out!\n");
      return -1;
  }

  return 0;
}
