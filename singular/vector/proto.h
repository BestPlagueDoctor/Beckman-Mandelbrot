//This is for prototyping all funcs and objects eh
#include <cstdio>
#include <vector>
#include <cstdint>

using Pixel = int32_t;
using Image = std::vector<std::vector<Pixel>>;
  
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
void imgmandel(int maxiter, Image &img, int res, int xres, int yres);
void write_pgm(const char *filename, Image &img, int xres, int yres, int res, int maxiter);
void rescale_colors_pgm(Image &img, int res, int *maxiter);
void printcoord(Image &img, int xres, int yres, int res); 
int rescale_r_ppm(Image &img, int *maxiter, int i, int r); 
int rescale_g_ppm(Image &img, int *maxiter, int i, int g); 
int rescale_b_ppm(Image &img, int *maxiter, int i, int b); 
void write_ppm(const char *filename, Image &img, int xres, int yres, int res, int maxiter);
