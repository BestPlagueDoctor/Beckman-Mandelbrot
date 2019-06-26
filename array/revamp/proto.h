//This is for prototyping all funcs and objects eh
#include <stdio.h>

class complexnumber {
  float real, imag;
  public:
    void setcomp (float, float);
    void getcomp ();
    void addcomp(complexnumber, complexnumber);
    void multcomp(complexnumber, complexnumber);
    float getreal();
    float getimag();  
    float sqmagnit();
};
void imgmandel(int maxiter, int *img, int res, int xres, int yres);
void write_pgm(const char *filename, int* img, int xres, int yres, int res, int maxiter);
void rescale_colors_pgm(int *img, int res, int *maxiter);
void printcoord(int *img, int xres, int yres, int res); 
int rescale_r_ppm(int *img, int *maxiter, int i, int r); 
int rescale_g_ppm(int *img, int *maxiter, int i, int g); 
int rescale_b_ppm(int *img, int *maxiter, int i, int b); 
void write_ppm(const char *filename, int *img, int xres, int yres, int res, int maxiter);
